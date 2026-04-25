#ifndef DFA_H
#define DFA_H

#include <stdbool.h>
#include <stddef.h>

typedef int DfaState;

typedef struct {
  DfaState from;
  char symbol;
  DfaState to;
} DfaTransition;

typedef enum {
  DFA_STATUS_OK = 0,
  DFA_STATUS_REJECTED,
  DFA_STATUS_INVALID_SYMBOL,
  DFA_STATUS_INVALID_ARGUMENT
} DfaStatus;

typedef struct {
  const DfaTransition *transitions;
  size_t transitions_count;

  const DfaState *accepting_states;
  size_t accepting_states_count;

  DfaState start_state;
} Dfa;

DfaStatus dfa_init(Dfa *dfa,
                   const DfaTransition *transitions,
                   size_t transitions_count,
                   const DfaState *accepting_states,
                   size_t accepting_states_count,
                   DfaState start_state);

bool dfa_check_string(const Dfa *dfa, const char *string, DfaStatus *status);

const char *dfa_status_to_string(DfaStatus status);

#endif
