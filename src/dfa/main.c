#include "dfa.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum {
    BUFFER_SIZE = 256,
    TRANSITIONS_COUNT = 98,
    ACCEPTING_STATES_COUNT = 3
};

typedef enum {
    STATE_START = 0,
    STATE_AFTER_MINUS = 1,
    STATE_INTEGER_PART = 2,
    STATE_DOT_WITHOUT_INTEGER = 3,
    STATE_DOT_AFTER_INTEGER = 4,
    STATE_FRACTIONAL_PART = 5,
    STATE_EXPONENT_MARK = 6,
    STATE_EXPONENT_SIGN = 7,
    STATE_EXPONENT_DIGITS = 8
} NumberDfaState;

static void remove_trailing_newline(char* string)
{
    const size_t length = strlen(string);

    if (length > 0 && string[length - 1] == '\n') {
        string[length - 1] = '\0';
    }
}

static void add_digit_transitions(DfaTransition* transitions,
    size_t* index,
    DfaState from,
    DfaState to)
{
    for (char digit = '0'; digit <= '9'; ++digit) {
        transitions[*index] = (DfaTransition) { .from = from, .symbol = digit, .to = to };
        ++(*index);
    }
}

static size_t build_number_dfa_transitions(DfaTransition* transitions)
{
    size_t index = 0;

    transitions[index++] = (DfaTransition) {
        .from = STATE_START, .symbol = '-', .to = STATE_AFTER_MINUS
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_START, .symbol = '.', .to = STATE_DOT_WITHOUT_INTEGER
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_AFTER_MINUS, .symbol = '.', .to = STATE_DOT_WITHOUT_INTEGER
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_INTEGER_PART, .symbol = '.', .to = STATE_DOT_AFTER_INTEGER
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_INTEGER_PART, .symbol = 'E', .to = STATE_EXPONENT_MARK
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_FRACTIONAL_PART, .symbol = 'E', .to = STATE_EXPONENT_MARK
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_EXPONENT_MARK, .symbol = '+', .to = STATE_EXPONENT_SIGN
    };

    transitions[index++] = (DfaTransition) {
        .from = STATE_EXPONENT_MARK, .symbol = '-', .to = STATE_EXPONENT_SIGN
    };

    add_digit_transitions(transitions, &index, STATE_START, STATE_INTEGER_PART);
    add_digit_transitions(
        transitions, &index, STATE_AFTER_MINUS, STATE_INTEGER_PART);
    add_digit_transitions(
        transitions, &index, STATE_INTEGER_PART, STATE_INTEGER_PART);
    add_digit_transitions(
        transitions, &index, STATE_DOT_WITHOUT_INTEGER, STATE_FRACTIONAL_PART);
    add_digit_transitions(
        transitions, &index, STATE_DOT_AFTER_INTEGER, STATE_FRACTIONAL_PART);
    add_digit_transitions(
        transitions, &index, STATE_FRACTIONAL_PART, STATE_FRACTIONAL_PART);
    add_digit_transitions(
        transitions, &index, STATE_EXPONENT_MARK, STATE_EXPONENT_DIGITS);
    add_digit_transitions(
        transitions, &index, STATE_EXPONENT_SIGN, STATE_EXPONENT_DIGITS);
    add_digit_transitions(
        transitions, &index, STATE_EXPONENT_DIGITS, STATE_EXPONENT_DIGITS);

    return index;
}

int main(void)
{
    DfaTransition transitions[TRANSITIONS_COUNT];
    const size_t TRANSITIONS_COUNT_BUILT = build_number_dfa_transitions(transitions);

    const DfaState ACCEPTING_STATES[ACCEPTING_STATES_COUNT] = {
        STATE_INTEGER_PART,
        STATE_FRACTIONAL_PART,
        STATE_EXPONENT_DIGITS,
    };

    Dfa dfa;
    DfaStatus status = dfa_init(&dfa, transitions, TRANSITIONS_COUNT_BUILT, ACCEPTING_STATES,
        ACCEPTING_STATES_COUNT, STATE_START);

    if (status != DFA_STATUS_OK) {
        printf("Ошибка инициализации ДКА: %s\n", dfa_status_to_string(status));
        return 1;
    }

    char input[BUFFER_SIZE];

    printf("Введите строку: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Ошибка чтения строки\n");
        return 1;
    }

    remove_trailing_newline(input);

    const bool IS_NUMBER = dfa_check_string(&dfa, input, &status);

    if (IS_NUMBER) {
        printf("Строка является числом\n");
        return 0;
    }

    printf("Строка не является числом\n");
    printf("Статус ДКА: %s\n", dfa_status_to_string(status));

    return 0;
}
