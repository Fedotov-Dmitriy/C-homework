#include "CSV.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    FILE* stream = fopen(inp, "r");

    for (int i = 0; i < rows; i++) {
        fgets(buffer, 1024, stream);
        buffer[strcspn(buffer, "\n")] = 0;
        char* token = strtok(buffer, ",");
        for (int j = 0; j < columns; j++) {
            /* Если токен NULL — строка короче заголовка, считаем длину 0 */
            const char* cell = (token != NULL) ? token : "";
            int len = (int)strlen(cell);
            if (len > width[j]) {
                width[j] = len;
            }
            if (token != NULL) {
                token = strtok(NULL, ",");
            }
        }
    }

    fclose(stream);
    return width;
}

static void printRow(FILE* out, char* buffer, const int* widths, int columns, bool heading)
{
    fprintf(out, "| ");

    /* Разбиваем строку на токены заранее, чтобы не путать strtok с двумя вызовами */
    char* tokens[1024];
    int count = 0;
    char* token = strtok(buffer, ",");
    while (token != NULL && count < columns) {
        tokens[count++] = token;
        token = strtok(NULL, ",");
    }

    for (int j = 0; j < columns; j++) {
        /* Если столбца нет в строке — пустая ячейка */
        const char* cell = (j < count) ? tokens[j] : "";
        int len = (int)strlen(cell);

        char* endp = NULL;
        double res = strtod(cell, &endp);
        bool isNumber = !(*endp != 0 && res == 0.0) && !heading && strlen(cell) > 0;

        if (!isNumber) {
            /* Текст / заголовок: выравнивание влево */
            fprintf(out, "%s", cell);
            for (int q = 0; q < widths[j] - len; q++) {
                fprintf(out, " ");
            }
        } else {
            /* Число: выравнивание вправо */
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

    /* Разделитель после строки */
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

    /* Верхняя рамка */
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
