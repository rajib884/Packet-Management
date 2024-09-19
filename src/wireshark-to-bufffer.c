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

wireshark_file_t *wireshark_file_create(const char *file_path)
{
    wireshark_file_t *ws_file = NULL;
    FILE *file = NULL;

    if (file_path == NULL)
    {
        return NULL;
    }

    ws_file = (wireshark_file_t *)calloc(1, sizeof(wireshark_file_t));

    if (ws_file == NULL)
    {
        fprintf(stderr, "Could not allocate memory for wireshark_file_t\n");
        goto cleanup;
    }

    ws_file->file_path = (const char *)strdup(file_path);

    if (ws_file->file_path == NULL)
    {
        fprintf(stderr, "Could not allocate memory for file_path\n");
        goto cleanup;
    }

    file = fopen(ws_file->file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for reading: %s\n", ws_file->file_path);
        goto cleanup;
    }

    if (fseek(file, 0, SEEK_END) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to end on file: %s\n", ws_file->file_path);
        goto cleanup;
    }

    ws_file->file_length = ftell(file);

    fclose(file);
    file = NULL;

    return ws_file;

cleanup:

    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    if (ws_file != NULL)
    {
        free((void *)ws_file->file_path);
        ws_file->file_path = NULL;
    }

    free(ws_file);
    ws_file = NULL;

    return NULL;
}

bool wireshark_file_readable(wireshark_file_t *ws_file)
{
    if (ws_file == NULL)
    {
        return false;
    }

    return ws_file->file_length > ws_file->current_pos;
}

void wireshark_file_free(wireshark_file_t **ws_file_p)
{
    if (ws_file_p == NULL || (*ws_file_p) == NULL)
    {
        return;
    }

    free((void *)(*ws_file_p)->file_path);
    (*ws_file_p)->file_path = NULL;

    free((*ws_file_p));
    (*ws_file_p) = NULL;

    return;
}

dynamic_buffer_t *wireshark_file_get_next_packet(wireshark_file_t *ws_file)
{
    dynamic_buffer_t *buffer = NULL;
    FILE *file = NULL;
    char line_buf[LINE_BUF_LEN] = {0};
    uint8_t content[LINE_MAX_CONTENT] = {0};
    size_t content_pos = 0;
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
        fprintf(stderr, "Error on fseek to end on file: %s\n", ws_file->file_path);
        goto cleanup;
    }

    ws_file->file_length = ftell(file);

    if (ws_file->current_pos >= ws_file->file_length)
    {
        goto cleanup; /* Nothing more to read */
    }

    if (fseek(file, ws_file->current_pos, SEEK_SET) != FSEEK_OK)
    {
        fprintf(stderr, "Error on fseek to %ld on file: %s\n", ws_file->current_pos,
                ws_file->file_path);
        goto cleanup;
    }

    buffer = dynamic_buffer_create(DYNAMIC_BUFFER_INIT_SIZE);

    if (buffer == NULL)
    {
        fprintf(stderr, "Error creating dynamic buffer.\n");
        goto cleanup;
    }

    success = true;

    while ((ws_file->current_pos < ws_file->file_length) && success)
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
        content_pos = 0;

        while (*startptr != '\0')
        {
            temp_value = strtol(startptr, &endptr, BASE_HEX); /* Hex string to long */

            if (startptr == endptr || temp_value > UINT8_MAX)
            {
                break;
            }

            content[content_pos++] = (char)temp_value;

            if (content_pos > LINE_MAX_CONTENT)
            {
                /* Avoid array overflow */
                success = success && dynamic_buffer_add_data(buffer, content, content_pos);
                content_pos = 0;
            }

            startptr = endptr;
        }

        success = success && dynamic_buffer_add_data(buffer, content, content_pos);
    }

    if (success == false)
    {
        fprintf(stderr, "Failed to add data to buffer\n");
        dynamic_buffer_free(&buffer);
    }

cleanup:

    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    return buffer;
}