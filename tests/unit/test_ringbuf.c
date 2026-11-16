
#include <stdio.h>
#include <assert.h>
#include "ringbuf.h"

#define CAPACITY 4

void test_empty(void)
{
    ringbuf_t rb;
    uint8_t value;

    ringbuf_init(&rb);

    assert(ringbuf_is_empty(&rb));
    assert(!ringbuf_pop(&rb, &value));

    printf("PASS: empty\n");
}

void test_full(void)
{
    ringbuf_t rb;

    ringbuf_init(&rb);

    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, i));

    assert(ringbuf_is_full(&rb));

    printf("PASS: full\n");
}

void test_overflow(void)
{
    ringbuf_t rb;

    ringbuf_init(&rb);

    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, i));

    assert(!ringbuf_push(&rb, 99));

    printf("PASS: overflow\n");
}

void test_wraparound(void)
{
    ringbuf_t rb;
    uint8_t value;

    ringbuf_init(&rb);

    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, i));

    for (int i = 0; i < 2; i++)
    {
        assert(ringbuf_pop(&rb, &value));
        assert(value == i);
    }

    assert(ringbuf_push(&rb, 10));
    assert(ringbuf_push(&rb, 11));

    for (int i = 2; i < 4; i++)
    {
        assert(ringbuf_pop(&rb, &value));
        assert(value == i);
    }

    assert(ringbuf_pop(&rb, &value));
    assert(value == 10);

    assert(ringbuf_pop(&rb, &value));
    assert(value == 11);

    printf("PASS: wraparound\n");
}

int main(void)
{
    test_empty();
    test_full();
    test_overflow();
    test_wraparound();

    printf("\nAll basic tests passed!\n");

    return 0;
}
