//#pragma once inside a header file (my_header.h) prevents redefinition errors
#include "crc8.h"
#include <stdio.h>

uint8_t crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00;
    size_t i, j;

    for (i = 0; i < len; i++)
    {
        crc ^= data[i];

        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x07);
            else
                crc <<= 1;
        }
    }

    return crc;
}

