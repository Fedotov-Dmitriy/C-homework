#include "CSV.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Разбивает строку по запятым, учитывая пустые поля */
static int splitCSV(char* buf, char** out_tokens, int max_tokens)
{
    int count = 0;
    char* p = buf;
    while (count < max_tokens) {
        out_tokens[count++] = p;
        char* comma = strchr(p, ',');
        if (comma == NULL) break;
        *comma = '\0';
        p = comma + 1;
    }
    return count;
}

static void countColumnsAndRows(const char* inp, int* rows, int* columns)
{
    FILE* file = fopen(inp, "r");
    char buf[1024];
    while (fgets(buf, 1024, file)) {
        (*rows)++;
        if ((*rows) == 1) {
            for (int i = 0; buf[i] != '\0'; i++) {
                if (buf[i] == ',') {
                    (*columns)++;
                }
            }
            (*columns)++;
        }
    }
    fclose(file);
}

static int* makeArrayOfWidth(const char* inp, int rows, int columns)
{
    int* width = NULL;
    if (columns) {
        width = (int*)calloc(columns, sizeof(int));
    }

    char buffer[1024];
    char* tokens[1024];
    FILE* stream = fopen(inp, "r");

    for (int i = 0; i < rows; i++) {
        fgets(buffer, 1024, stream);
        buffer[strcspn(buffer, "\n")] = 0;

        int count = splitCSV(buffer, tokens, columns);

        for (int j = 0; j < columns; j++) {
            const char* cell = (j < count) ? tokens[j] : "";
            int len = (int)strlen(cell);
            if (len > width[j]) {
                width[j] = len;
            }
        }
    }

    fclose(stream);
    return width;
}

static void printRow(FILE* out, char* buffer, const int* widths, int columns, bool heading)
{
    fprintf(out, "| ");

    char* tokens[1024];
    int count = splitCSV(buffer, tokens, columns);

    for (int j = 0; j < columns; j++) {
        const char* cell = (j < count) ? tokens[j] : "";
        int len = (int)strlen(cell);

        char* endp = NULL;
        double res = strtod(cell, &endp);
        bool isNumber = (len > 0) && !heading && !(*endp != 0 && res == 0.0);

        if (!isNumber) {
            fprintf(out, "%s", cell);
            for (int q = 0; q < widths[j] - len; q++) {
                fprintf(out, " ");
            }
        } else {
            for (int q = 0; q < widths[j] - len; q++) {
                fprintf(out, " ");
            }
            fprintf(out, "%s", cell);
        }

        fprintf(out, " |");
        if (j != columns - 1) {
            fprintf(out, " ");
        }
    }

    fprintf(out, "\n");

    for (int k = 0; k < columns; k++) {
        fprintf(out, "+");
        for (int q = 0; q < widths[k] + 2; q++) {
            fprintf(out, heading ? "=" : "-");
        }
    }
    fprintf(out, "+\n");
}

static void printRows(const char* inp, const char* out, int* widths, int rows, int columns)
{
    FILE* input = fopen(inp, "r");
    FILE* output = fopen(out, "w");

    for (int k = 0; k < columns; k++) {
        fprintf(output, "+");
        for (int q = 0; q < widths[k] + 2; q++) {
            fprintf(output, "=");
        }
    }
    if (rows != 0 && columns != 0) {
        fprintf(output, "+\n");
    }

    char buffer[1024];
    for (int i = 0; i < rows; i++) {
        fgets(buffer, 1024, input);
        buffer[strcspn(buffer, "\n")] = 0;
        printRow(output, buffer, widths, columns, i == 0);
    }

    fclose(output);
    fclose(input);
}

bool prettyPrinter(const char* inp, const char* out)
{
    FILE* input = fopen(inp, "r");
    if (input == NULL) {
        return false;
    }
    fclose(input);

    int columnsNum = 0;
    int rowsNum = 0;
    countColumnsAndRows(inp, &rowsNum, &columnsNum);

    int* width = makeArrayOfWidth(inp, rowsNum, columnsNum);
    printRows(inp, out, width, rowsNum, columnsNum);

    free(width);
    return true;
}
