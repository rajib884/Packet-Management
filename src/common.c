#include <stdio.h>

#include "common.h"

void print_data_f(uint8_t *data, size_t data_len)
{
    size_t i = 0;

    if (data_len == 0)
    {
        printf("\n    No Data");
    }

    while (i < data_len)
    {
        if (i % COMMON_DATA_PRINT_COL == 0)
        {
            printf("\n    ");
        }

        printf("%02x ", data[i++]);
    }

    printf("\n");
}