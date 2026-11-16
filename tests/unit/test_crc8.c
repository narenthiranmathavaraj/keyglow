
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "crc8.h"

void test_empty(void)
{
    uint8_t data[] = {};
    assert(crc8(data, 0) == 0x00);

    printf("PASS: empty\n");
}

void test_single_byte(void)
{
    uint8_t data[] = {0x31};

    assert(crc8(data, 1) == 0x97);

    printf("PASS: single byte\n");
}

void test_multiple_bytes(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03};

    assert(crc8(data, 3) == 0x48);

    printf("PASS: multiple bytes\n");
}

void test_known_vector(void)
{
    // Standard CRC-8 (poly 0x07) check value
    // ASCII "123456789" -> 0xF4

    uint8_t data[] = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9'
    };

    assert(crc8(data, 10) == 0x45);

    printf("PASS: known vector\n");
}
void test_alphabet(void)
{
    uint8_t data[] = "ABC";

    assert(crc8(data, 3) == crc8((uint8_t[]){0x41, 0x42, 0x43}, 3));

    printf("PASS: alphabet\n");
}
void test_different_data(void)
{
    uint8_t data1[] = {0x01, 0x02, 0x03};
    uint8_t data2[] = {0x01, 0x02, 0x04};

    assert(crc8(data1, 3) != crc8(data2, 3));

    printf("PASS: different data\n");
}

int main(void)
{
    test_empty();
    test_single_byte();
    test_multiple_bytes();
    test_known_vector();
    test_different_data();
    test_alphabet();
    printf("\nAll CRC8 tests passed!\n");

    return 0;
}
