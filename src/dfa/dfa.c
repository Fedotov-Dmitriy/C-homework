#include "dfa.h"

#include <stddef.h>

static bool dfa_is_accepting_state(const Dfa* dfa, DfaState state)
{
    for (size_t i = 0; i < dfa->accepting_states_count; ++i) {
        if (dfa->accepting_states[i] == state) {
            return true;
        }
    }

    return false;
}

static bool dfa_is_symbol_in_alphabet(const Dfa* dfa, char symbol)
{
    for (size_t i = 0; i < dfa->transitions_count; ++i) {
        if (dfa->transitions[i].symbol == symbol) {
            return true;
        }
    }

    return false;
}

static bool dfa_try_make_transition(const Dfa* dfa,
    DfaState current_state,
    char symbol,
    DfaState* next_state)
{
    for (size_t i = 0; i < dfa->transitions_count; ++i) {
        const DfaTransition transition = dfa->transitions[i];

        if (transition.from == current_state && transition.symbol == symbol) {
            *next_state = transition.to;
            return true;
        }
    }

    return false;
}

DfaStatus dfa_init(Dfa* dfa,
    const DfaTransition* transitions,
    size_t transitions_count,
    const DfaState* accepting_states,
    size_t accepting_states_count,
    DfaState start_state)
{
    if (dfa == NULL || transitions == NULL || transitions_count == 0 || accepting_states == NULL || accepting_states_count == 0) {
        return DFA_STATUS_INVALID_ARGUMENT;
    }

    dfa->transitions = transitions;
    dfa->transitions_count = transitions_count;
    dfa->accepting_states = accepting_states;
    dfa->accepting_states_count = accepting_states_count;
    dfa->start_state = start_state;

    return DFA_STATUS_OK;
}

bool dfa_check_string(const Dfa* dfa, const char* string, DfaStatus* status)
{
    if (status != NULL) {
        *status = DFA_STATUS_OK;
    }

    if (dfa == NULL || string == NULL) {
        if (status != NULL) {
            *status = DFA_STATUS_INVALID_ARGUMENT;
        }

        return false;
    }

    DfaState current_state = dfa->start_state;

    for (size_t i = 0; string[i] != '\0'; ++i) {
        const char symbol = string[i];

        if (!dfa_is_symbol_in_alphabet(dfa, symbol)) {
            if (status != NULL) {
                *status = DFA_STATUS_INVALID_SYMBOL;
            }

            return false;
        }

        DfaState next_state = current_state;

        if (!dfa_try_make_transition(dfa, current_state, symbol, &next_state)) {
            if (status != NULL) {
                *status = DFA_STATUS_REJECTED;
            }

            return false;
        }

        current_state = next_state;
    }

    if (!dfa_is_accepting_state(dfa, current_state)) {
        if (status != NULL) {
            *status = DFA_STATUS_REJECTED;
        }

        return false;
    }

    if (status != NULL) {
        *status = DFA_STATUS_OK;
    }

    return true;
}

const char* dfa_status_to_string(DfaStatus status)
{
    switch (status) {
    case DFA_STATUS_OK:
        return "корректная работа";
    case DFA_STATUS_REJECTED:
        return "строка не принадлежит языку";
    case DFA_STATUS_INVALID_SYMBOL:
        return "передан символ не из алфавита";
    case DFA_STATUS_INVALID_ARGUMENT:
        return "передан некорректный аргумент";
    }

    return "неизвестный статус";
}
