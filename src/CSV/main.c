#include "CSV.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GREEN(string) "\x1b[32m" string "\x1b[0m"
#define RED(string) "\x1b[31m" string "\x1b[0m"

bool compareTxtFiles(const char* fi, const char* se)
{
    FILE* f1 = fopen(fi, "r");
    FILE* f2 = fopen(se, "r");
    if (f1 == NULL || f2 == NULL)
        return false;
    int ch1;
    int ch2;

    while ((ch1 = getc(f1)) != EOF && (ch2 = getc(f2)) != EOF) {
        if (ch1 != ch2) {
            return false;
        }
    }

    fclose(f1);
    fclose(f2);
    return true;
}

int main(int argc, char** argv)
{
    bool testMode = false;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--test") == 0) {
            testMode = true;
            break;
        }
    }

    if (testMode) {
        int ret = 0;
        const char* ins[3] = {
            "tests/input/input_1.csv",
            "tests/input/input_2.csv",
            "tests/input/input_3.csv"
        };
        const char* outs[3] = {
            "tests/output/output_1.txt",
            "tests/output/output_2.txt",
            "tests/output/output_3.txt"
        };
        for (int testNum = 0; testNum < 3; ++testNum) {
            prettyPrinter(ins[testNum], "output.txt");
            bool res = compareTxtFiles("output.txt", outs[testNum]);
            if (res) {
                printf(GREEN("Test %d passed!\n"), testNum + 1);
            } else {
                printf(RED("Test %d failed!\n"), testNum + 1);
                ret = 1;
            }
        }
        return ret;
    }
    bool res = prettyPrinter("input.csv", "output.txt");
    printf("%s", res ? "Done\n" : "File not found\n");
    return 0;
}
