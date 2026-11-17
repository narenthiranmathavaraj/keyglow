#include "ringbuf.h"

void ringbuf_init(ringbuf_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool ringbuf_push(ringbuf_t *rb, char data) {

    if (ringbuf_is_full(rb)) {
        // Buffer is full
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->count++;
    rb->head = (rb->head + 1) % RINGBUF_CAPACITY;
    return true;
}

bool ringbuf_pop(ringbuf_t *rb, char *data) {
    if (ringbuf_is_empty(rb)) {
        // Buffer is empty
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->count--;

    rb->tail = (rb->tail + 1) % RINGBUF_CAPACITY;
    return true;
}

bool ringbuf_is_empty(ringbuf_t *rb) {
    return rb->count == 0;
}
bool ringbuf_is_full(ringbuf_t *rb) {
    return rb->count == RINGBUF_CAPACITY;
}