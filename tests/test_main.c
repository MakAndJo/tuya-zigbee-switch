// tests/support/test_main.c
#include "unity/unity.h"

extern void run_test_button(void); // provided by each suite via a small file or constructor
extern void run_test_button_both(void); // provided by each suite via a small file or constructor

int main(void) {
  UNITY_BEGIN();
  run_test_button();
  run_test_button_both();
  return UNITY_END();
}