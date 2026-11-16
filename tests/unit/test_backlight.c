#ifdef NDEBUG
#error "These tests rely on assert(); do not compile with -DNDEBUG"
#endif

#include <assert.h>
#include <stdio.h>
#include "backlight.h"

/* ---- helpers: print names, never index out of range ---- */
static const char *sname(int s) { return (s >= ON && s <= OFF) ? (const char *[]){"ON", "DIM", "OFF"}[s] : "?"; }
static const char *ename(int e) { return (e >= KEY && e <= TIMEOUT) ? (const char *[]){"KEY", "TIMEOUT"}[e] : "?"; }
static const char *aname(int a) { return (a >= NONE && a <= STOP) ? (const char *[]){"NONE", "START", "RESTART", "STOP"}[a] : "?"; }

/* Run one step, print what happened, then check it. */
static bl_t step(bl_states_t s, bl_events_t e, bl_states_t want_s, bl_actions_t want_a, uint16_t want_d)
{
    bl_t r = backlight(s, e);
    printf("    %-3s + %-7s -> %-3s action=%-7s delay=%u\n",
           sname(s), ename(e), sname(r.new_state), aname(r.action), (unsigned)r.delay);
    assert(r.new_state == want_s);
    assert(r.action == want_a);
    assert(r.delay == want_d);
    return r;
}

/* ---- the six cells of the transition table ----
 * START   = the timer is not running (it was never started, or it just expired)
 * RESTART = the timer is running and a key arrived */
static void test_off_key(void)     { step(OFF, KEY,     ON,  START,   DIM_DELAY_MS); }
static void test_on_key(void)      { step(ON,  KEY,     ON,  RESTART, DIM_DELAY_MS); }
static void test_dim_key(void)     { step(DIM, KEY,     ON,  RESTART, DIM_DELAY_MS); }
static void test_on_timeout(void)  { step(ON,  TIMEOUT, DIM, START,   OFF_DELAY_MS); }
static void test_dim_timeout(void) { step(DIM, TIMEOUT, OFF, STOP,    0); }
static void test_off_timeout(void) { step(OFF, TIMEOUT, OFF, NONE,    0); }

/* ---- behavior over sequences ---- */
static void test_total_idle_time(void)
{
    printf("    %u + %u ms\n", DIM_DELAY_MS, OFF_DELAY_MS);
    assert(DIM_DELAY_MS + OFF_DELAY_MS == 5000);
}

static void test_full_idle_sequence(void)
{
    bl_t r = step(OFF, KEY,   ON,  START, DIM_DELAY_MS);
    r = step(r.new_state, TIMEOUT, DIM, START, OFF_DELAY_MS);
    r = step(r.new_state, TIMEOUT, OFF, STOP,  0);
    (void)r;
}

static void test_key_while_dim_restarts_full_timer(void)
{
    bl_t r = step(OFF, KEY,   ON,  START,   DIM_DELAY_MS);
    r = step(r.new_state, TIMEOUT, DIM, START,   OFF_DELAY_MS);
    r = step(r.new_state, KEY,     ON,  RESTART, DIM_DELAY_MS);   /* full 2000, not the remainder */
    r = step(r.new_state, TIMEOUT, DIM, START,   OFF_DELAY_MS);
    (void)r;
}

static void test_many_keys_stay_on(void)
{
    bl_states_t s = OFF;
    for (int i = 0; i < 100; i++) {
        bl_t r = backlight(s, KEY);
        assert(r.new_state == ON);
        assert(r.action == (i == 0 ? START : RESTART));   /* only the first key starts the timer */
        assert(r.delay == DIM_DELAY_MS);
        s = r.new_state;
    }
    printf("    100 keys in a row: still ON\n");
}

/* ---- every combination gives a valid result ---- */
static void test_every_combination_is_valid(void)
{
    int n = 0;
    for (int s = ON; s <= OFF; s++) {
        for (int e = KEY; e <= TIMEOUT; e++) {
            bl_t r = backlight((bl_states_t)s, (bl_events_t)e);
            assert(r.new_state == ON || r.new_state == DIM || r.new_state == OFF);
            assert(r.action == NONE || r.action == START || r.action == RESTART || r.action == STOP);
            if (r.action == START || r.action == RESTART) assert(r.delay > 0);
            else                                          assert(r.delay == 0);
            n++;
        }
    }
    printf("    %d combinations checked\n", n);
}

/* ---- invalid input ---- */
static void test_unknown_event_changes_nothing(void)
{
    int bad = 99;
    for (int s = ON; s <= OFF; s++) {
        bl_t r = backlight((bl_states_t)s, (bl_events_t)bad);
        printf("    %-3s + event %d -> %-3s action=%s\n", sname(s), bad, sname(r.new_state), aname(r.action));
        assert(r.new_state == (bl_states_t)s);
        assert(r.action == NONE);
        assert(r.delay == 0);
    }
}

static void test_unknown_state_falls_back_to_off(void)
{
    int bad = 99;
    for (int e = KEY; e <= TIMEOUT; e++) {
        bl_t r = backlight((bl_states_t)bad, (bl_events_t)e);
        printf("    state %d + %-7s -> %-3s action=%s\n", bad, ename(e), sname(r.new_state), aname(r.action));
        assert(r.new_state == OFF);
        assert(r.action == STOP);
        assert(r.delay == 0);
    }
}

#define RUN(t) do { printf("%s\n", #t); t(); printf("    ok\n"); } while (0)

int main(void)
{
    RUN(test_off_key);
    RUN(test_on_key);
    RUN(test_dim_key);
    RUN(test_on_timeout);
    RUN(test_dim_timeout);
    RUN(test_off_timeout);
    RUN(test_total_idle_time);
    RUN(test_full_idle_sequence);
    RUN(test_key_while_dim_restarts_full_timer);
    RUN(test_many_keys_stay_on);
    RUN(test_every_combination_is_valid);
    RUN(test_unknown_event_changes_nothing);
    RUN(test_unknown_state_falls_back_to_off);
    printf("all tests passed\n");
    return 0;
}