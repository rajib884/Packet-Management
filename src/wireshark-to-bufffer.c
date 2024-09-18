// OK AFAIK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "wireshark-to-buffer.h"

#define LINE_BUF_LEN 80     /* Buffer size to store 1 line from file */
#define LINE_MAX_CONTENT 16 /* Each line has maximum 16 byte */
#define LINE_DATA_START 6   /* Garbage value before this */
#define LINE_DATA_END 54    /* Garbage value after this */

#define FSEEK_OK 0
#define BASE_HEX 16

#define DYNAMIC_BUFFER_INIT_SIZE 128 /* Buffer is initialized with this capacity */

wireshark_file_t *init_wireshark_file(const char *file_path)
{
    wireshark_file_t *ws_file = NULL;

    if (file_path == NULL)
    {
        return NULL;
    }

    ws_file = (wireshark_file_t *)calloc(1, sizeof(wireshark_file_t));

    if (ws_file == NULL)
    {
        fprintf(stderr, "Could not allocate memory for wireshark_file_t\n");
        return NULL;
    }

    ws_file->file_path = (const char *)strdup(file_path);

    if (ws_file->file_path == NULL)
    {
        fprintf(stderr, "Could not allocate memory for file_path\n");
        return NULL;
    }

    return ws_file;
}

void free_wireshark_file(wireshark_file_t **ws_file_p)
{
    if (ws_file_p == NULL || (*ws_file_p) == NULL)
    {
        return;
    }

    free((*ws_file_p)->file_path);
    (*ws_file_p)->file_path = NULL;

    free((*ws_file_p));
    (*ws_file_p) = NULL;

    return;
}

dynamic_buffer_t *get_next_packet(wireshark_file_t *ws_file)
{
    dynamic_buffer_t *buffer = NULL;
    FILE *file = NULL;
    long file_length = 0;
    char line_buf[LINE_BUF_LEN] = {0};
    uint8_t line_content[LINE_MAX_CONTENT] = {0};
    size_t line_content_index = 0;
    char *endptr = NULL;
    char *startptr = NULL;
    long temp_value = 0;
    bool success = false;

    if (ws_file == NULL)
    {
        return NULL;
    }

    debug("Getting next packet\n");

    file = fopen(ws_file->file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for reading: %s\n", ws_file->file_path);
        goto cleanup;
    }

    if (fseek(file, 0, SEEK_END) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to start on file: %s\n", ws_file->file_path);
        goto cleanup;
    }

    file_length = ftell(file);

    if (ws_file->current_pos >= file_length)
    {
        goto cleanup; /* Nothing more to read */
    }

    if (fseek(file, ws_file->current_pos, SEEK_SET) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to %ld on file: %s\n", ws_file->current_pos,
                ws_file->file_path);
        goto cleanup;
    }

    buffer = create_dynamic_buffer(DYNAMIC_BUFFER_INIT_SIZE);

    if (buffer == NULL)
    {
        fprintf(stderr, "Error creating dynamic buffer.\n");
        goto cleanup;
    }

    success = true;

    while ((ws_file->current_pos < file_length) && success)
    {
        /* Read a line from file and store it in line_buf */
        if (!fgets(line_buf, LINE_BUF_LEN, file))
        {
            ws_file->current_pos = ftell(file);
            break;
        }

        ws_file->current_pos = ftell(file);
        line_buf[LINE_DATA_END] = '\0';

        if (strnlen(line_buf, LINE_DATA_END) < LINE_DATA_START)
        {
            break;
        }

        startptr = line_buf + LINE_DATA_START;
        line_content_index = 0;

        while (*startptr != '\0')
        {
            temp_value = strtol(startptr, &endptr, BASE_HEX); /* Hex string to long */

            if (startptr == endptr || temp_value > UINT8_MAX)
            {
                break;
            }

            line_content[line_content_index++] = (char)temp_value;

            if (line_content_index > LINE_MAX_CONTENT)
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