
#include <stdio.h>
#include <assert.h>
#include "ringbuf.h"

#define CAPACITY RINGBUF_CAPACITY

void test_empty(void)
{
    ringbuf_t rb;
    char value;

    ringbuf_init(&rb);

    assert(ringbuf_is_empty(&rb));
    assert(rb.count == 0);
    assert(rb.head == 0);
    assert(rb.tail == 0);
    assert(!ringbuf_pop(&rb, &value));

    printf("PASS: empty\n");
}

void test_full(void)
{
    ringbuf_t rb;

    ringbuf_init(&rb);

    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, (char)i));

    assert(ringbuf_is_full(&rb));
    assert(rb.count == CAPACITY);

    printf("PASS: full\n");
}

void test_overflow(void)
{
    ringbuf_t rb;

    ringbuf_init(&rb);

    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, (char)i));

    assert(!ringbuf_push(&rb, 'X'));
    assert(rb.count == CAPACITY);

    printf("PASS: overflow\n");
}

void test_wraparound(void)
{
    ringbuf_t rb;
    char value;

    ringbuf_init(&rb);

    // Fill the buffer
    for (int i = 0; i < CAPACITY; i++)
        assert(ringbuf_push(&rb, (char)('0' + i)));

    assert(rb.count == CAPACITY);

    // Remove first two elements
    for (int i = 0; i < 2; i++)
    {
        assert(ringbuf_pop(&rb, &value));
        assert(value == (char)('0' + i));
    }

    assert(rb.count == CAPACITY - 2);

    // Insert two more elements (wraparound)
    assert(ringbuf_push(&rb, 'A'));
    assert(ringbuf_push(&rb, 'B'));

    assert(rb.count == CAPACITY);

    // Verify FIFO order
    for (int i = 2; i < CAPACITY; i++)
    {
        assert(ringbuf_pop(&rb, &value));
        assert(value == (char)('0' + i));
    }

    assert(ringbuf_pop(&rb, &value));
    assert(value == 'A');

    assert(ringbuf_pop(&rb, &value));
    assert(value == 'B');

    assert(rb.count == 0);
    assert(ringbuf_is_empty(&rb));

    printf("PASS: wraparound\n");
}

void test_count_tracking(void)
{
    ringbuf_t rb;
    char value;

    ringbuf_init(&rb);

    assert(rb.count == 0);

    assert(ringbuf_push(&rb, 'A'));
    assert(rb.count == 1);

    assert(ringbuf_push(&rb, 'B'));
    assert(rb.count == 2);

    assert(ringbuf_pop(&rb, &value));
    assert(rb.count == 1);

    assert(ringbuf_pop(&rb, &value));
    assert(rb.count == 0);

    // Failed pop must not change count
    assert(!ringbuf_pop(&rb, &value));
    assert(rb.count == 0);

    printf("PASS: count tracking\n");
}

int main(void)
{
    test_empty();
    test_full();
    test_overflow();
    test_wraparound();
    test_count_tracking();

    printf("\nAll basic tests passed!\n");

    return 0;
}
