#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/debug.h"
#include "wireshark-to-buffer.h"

#define WIRESHARK_DATA_FILE "data/udp.txt"

#define LINE_BUF_LEN 80          /* Buffer size to store 1 line from file */
#define LINE_MAX_CONTENT 16      /* Each line has maximum 16 byte */
#define IPV4_MAX_SIZE UINT16_MAX /* IPv4 has maximum length of 65535 (UINT16_MAX)*/

#define LINE_DATA_START 6 /* Garbage value before this */
#define LINE_DATA_END 54  /* Garbage value after this */

#define FSEEK_OK 0
#define BASE_HEX 16

#define DYNAMIC_BUFFER_INIT_SIZE 128 /* Buffer is initialized with this capacity */

dynamic_buffer_t *get_next_packet()
{
    dynamic_buffer_t *buffer = NULL;
    FILE *file = NULL;
    long file_length = 0;
    static long current_pos = 0;
    char line_buf[LINE_BUF_LEN] = {0};
    uint8_t line_content[LINE_MAX_CONTENT] = {0};
    size_t line_content_index = 0;
    char *endptr = NULL;
    char *startptr = NULL;
    long temp_value = 0;
    bool success = false;

    debug("Getting next packet\n");

    file = fopen(WIRESHARK_DATA_FILE, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for reading: %s\n", WIRESHARK_DATA_FILE);
        goto cleanup;
    }

    if (fseek(file, 0, SEEK_END) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to start on file: %s\n", WIRESHARK_DATA_FILE);
        goto cleanup;
    }

    file_length = ftell(file);

    if (current_pos >= file_length)
    {
        goto cleanup;
    }

    if (fseek(file, current_pos, SEEK_SET) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to %ld on file: %s\n", current_pos, WIRESHARK_DATA_FILE);
        goto cleanup;
    }

    buffer = create_dynamic_buffer(DYNAMIC_BUFFER_INIT_SIZE);
    success = true;

    while (current_pos < file_length && success)
    {
        /* Read a line from file */
        if (!fgets(line_buf, LINE_BUF_LEN, file))
        {
            current_pos = ftell(file);
            break;
        }

        current_pos = ftell(file);
        line_buf[LINE_DATA_END] = '\0';

        if (strnlen(line_buf, LINE_DATA_END) < LINE_DATA_START)
        {
            break;
        }

        startptr = &line_buf[LINE_DATA_START];
        line_content_index = 0;

        while (*startptr != '\0')
        {
            temp_value = strtol(startptr, &endptr, BASE_HEX); /* Hex string to long */

            if (startptr == endptr || temp_value > UINT8_MAX)
            {
                break;
            }

            line_content[line_content_index++] = (char)temp_value;

            if (line_content_index >= LINE_MAX_CONTENT)
            {
                success = success && add_data(buffer, line_content, line_content_index);
                line_content_index = 0;
            }

            startptr = endptr;
        }

        success = success && add_data(buffer, line_content, line_content_index);
    }

    if (success == false)
    {
        fprintf(stderr, "Failed to add data to buffer\n");
        free_buffer(&buffer);
    }

cleanup:

    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    return buffer;
}