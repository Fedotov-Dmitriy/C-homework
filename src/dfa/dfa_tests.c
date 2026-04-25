#include "dfa.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum {
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

static void add_digit_transitions(DfaTransition *transitions,
                                  size_t *index,
                                  DfaState from,
                                  DfaState to) {
  for (char digit = '0'; digit <= '9'; ++digit) {
    transitions[*index] =
        (DfaTransition){.from = from, .symbol = digit, .to = to};
    ++(*index);
  }
}

static size_t build_number_dfa_transitions(DfaTransition *transitions) {
  size_t index = 0;

  transitions[index++] = (DfaTransition){
      .from = STATE_START, .symbol = '-', .to = STATE_AFTER_MINUS};

  transitions[index++] = (DfaTransition){
      .from = STATE_START, .symbol = '.', .to = STATE_DOT_WITHOUT_INTEGER};

  transitions[index++] = (DfaTransition){
      .from = STATE_AFTER_MINUS, .symbol = '.', .to = STATE_DOT_WITHOUT_INTEGER};

  transitions[index++] = (DfaTransition){
      .from = STATE_INTEGER_PART, .symbol = '.', .to = STATE_DOT_AFTER_INTEGER};

  transitions[index++] = (DfaTransition){
      .from = STATE_INTEGER_PART, .symbol = 'E', .to = STATE_EXPONENT_MARK};

  transitions[index++] = (DfaTransition){
      .from = STATE_FRACTIONAL_PART, .symbol = 'E', .to = STATE_EXPONENT_MARK};

  transitions[index++] = (DfaTransition){
      .from = STATE_EXPONENT_MARK, .symbol = '+', .to = STATE_EXPONENT_SIGN};

  transitions[index++] = (DfaTransition){
      .from = STATE_EXPONENT_MARK, .symbol = '-', .to = STATE_EXPONENT_SIGN};

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

static Dfa create_number_dfa(DfaTransition *transitions,
                             const DfaState *accepting_states) {
  const size_t transitions_count = build_number_dfa_transitions(transitions);

  assert(transitions_count == TRANSITIONS_COUNT);

  Dfa dfa;
  const DfaStatus status =
      dfa_init(&dfa, transitions, transitions_count, accepting_states,
               ACCEPTING_STATES_COUNT, STATE_START);

  assert(status == DFA_STATUS_OK);

  return dfa;
}

static void expect_accepts(const Dfa *dfa, const char *string) {
  DfaStatus status = DFA_STATUS_REJECTED;
  const bool result = dfa_check_string(dfa, string, &status);

  assert(result);
  assert(status == DFA_STATUS_OK);
}

static void expect_rejects(const Dfa *dfa, const char *string) {
  DfaStatus status = DFA_STATUS_OK;
  const bool result = dfa_check_string(dfa, string, &status);

  assert(!result);
  assert(status == DFA_STATUS_REJECTED);
}

static void expect_invalid_symbol(const Dfa *dfa, const char *string) {
  DfaStatus status = DFA_STATUS_OK;
  const bool result = dfa_check_string(dfa, string, &status);

  assert(!result);
  assert(status == DFA_STATUS_INVALID_SYMBOL);
}

static void test_valid_numbers(void) {
  DfaTransition transitions[TRANSITIONS_COUNT];

  const DfaState accepting_states[ACCEPTING_STATES_COUNT] = {
      STATE_INTEGER_PART,
      STATE_FRACTIONAL_PART,
      STATE_EXPONENT_DIGITS,
  };

  const Dfa dfa = create_number_dfa(transitions, accepting_states);

  expect_accepts(&dfa, "0");
  expect_accepts(&dfa, "1");
  expect_accepts(&dfa, "123");
  expect_accepts(&dfa, "-123");

  expect_accepts(&dfa, "123.45");
  expect_accepts(&dfa, "-123.45");

  expect_accepts(&dfa, ".45");
  expect_accepts(&dfa, "-.45");

  expect_accepts(&dfa, "123E10");
  expect_accepts(&dfa, "123E+10");
  expect_accepts(&dfa, "123E-10");

  expect_accepts(&dfa, "-123E10");
  expect_accepts(&dfa, "-123E+10");
  expect_accepts(&dfa, "-123E-10");

  expect_accepts(&dfa, "123.45E10");
  expect_accepts(&dfa, "123.45E+10");
  expect_accepts(&dfa, "123.45E-10");

  expect_accepts(&dfa, ".45E10");
  expect_accepts(&dfa, ".45E+10");
  expect_accepts(&dfa, ".45E-10");

  expect_accepts(&dfa, "-.45E10");
  expect_accepts(&dfa, "-.45E+10");
  expect_accepts(&dfa, "-.45E-10");
}

static void test_invalid_number_structure(void) {
  DfaTransition transitions[TRANSITIONS_COUNT];

  const DfaState accepting_states[ACCEPTING_STATES_COUNT] = {
      STATE_INTEGER_PART,
      STATE_FRACTIONAL_PART,
      STATE_EXPONENT_DIGITS,
  };

  const Dfa dfa = create_number_dfa(transitions, accepting_states);

  expect_rejects(&dfa, "");
  expect_rejects(&dfa, "-");
  expect_rejects(&dfa, ".");
  expect_rejects(&dfa, "-.");

  expect_rejects(&dfa, "123.");
  expect_rejects(&dfa, "-123.");

  expect_rejects(&dfa, "E10");
  expect_rejects(&dfa, "-E10");

  expect_rejects(&dfa, "123E");
  expect_rejects(&dfa, "123E+");
  expect_rejects(&dfa, "123E-");

  expect_rejects(&dfa, ".E10");
  expect_rejects(&dfa, "-.E10");

  expect_rejects(&dfa, "123.45E");
  expect_rejects(&dfa, "123.45E+");
  expect_rejects(&dfa, "123.45E-");
}

static void test_invalid_symbols(void) {
  DfaTransition transitions[TRANSITIONS_COUNT];

  const DfaState accepting_states[ACCEPTING_STATES_COUNT] = {
      STATE_INTEGER_PART,
      STATE_FRACTIONAL_PART,
      STATE_EXPONENT_DIGITS,
  };

  const Dfa dfa = create_number_dfa(transitions, accepting_states);

  expect_invalid_symbol(&dfa, "abc");
  expect_invalid_symbol(&dfa, "1a");
  expect_invalid_symbol(&dfa, "123e10");
  expect_invalid_symbol(&dfa, "12,3");
  expect_invalid_symbol(&dfa, "+123");
  expect_invalid_symbol(&dfa, "12 3");
}

static void test_invalid_arguments(void) {
  DfaStatus status = DFA_STATUS_OK;

  assert(!dfa_check_string(NULL, "123", &status));
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  Dfa dfa;
  const DfaTransition transitions[] = {
      {.from = STATE_START, .symbol = '0', .to = STATE_INTEGER_PART},
  };
  const DfaState accepting_states[] = {
      STATE_INTEGER_PART,
  };

  status = dfa_init(&dfa, transitions, 1, accepting_states, 1, STATE_START);
  assert(status == DFA_STATUS_OK);

  assert(!dfa_check_string(&dfa, NULL, &status));
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  status = dfa_init(NULL, transitions, 1, accepting_states, 1, STATE_START);
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  status = dfa_init(&dfa, NULL, 1, accepting_states, 1, STATE_START);
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  status = dfa_init(&dfa, transitions, 0, accepting_states, 1, STATE_START);
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  status = dfa_init(&dfa, transitions, 1, NULL, 1, STATE_START);
  assert(status == DFA_STATUS_INVALID_ARGUMENT);

  status = dfa_init(&dfa, transitions, 1, accepting_states, 0, STATE_START);
  assert(status == DFA_STATUS_INVALID_ARGUMENT);
}

int main(void) {
  test_valid_numbers();
  test_invalid_number_structure();
  test_invalid_symbols();
  test_invalid_arguments();

  printf("All DFA tests passed.\n");

  return 0;
}
