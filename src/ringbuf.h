
#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

#define RINGBUF_CAPACITY 4

typedef struct
{
    uint8_t buffer[RINGBUF_CAPACITY];

    uint8_t head;
    uint8_t tail;
    uint8_t count;

} ringbuf_t;

void ringbuf_init(ringbuf_t *rb);

bool ringbuf_push(ringbuf_t *rb, uint8_t data);

bool ringbuf_pop(ringbuf_t *rb, uint8_t *data);

bool ringbuf_is_empty(ringbuf_t *rb);

bool ringbuf_is_full(ringbuf_t *rb);

#endif
