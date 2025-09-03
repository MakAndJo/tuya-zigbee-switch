#include "button.h"
#include "button_combiner.h"
#include "tl_common.h"
#include "millis.h"

void both_btn_update(button_t *btnA, button_t *btnB);

static u8 both_pressed = 0;
static u8 both_press_sent = 0;
static u8 both_release_sent = 0;
static u8 both_hold_sent = 0;

void both_btn_update(button_t *btnA, button_t *btnB) {
  if (!btnA || !btnB) return;

  if (btnA->pressed && btnB->pressed) { // BOTH PRESSED
    both_pressed = 1;
    if (!both_press_sent) {
      both_press_sent = 1;
      printf("BOTH pressed!\r\n");
			btn_push_event(btnA, EV_MULTI, MULTI_PRESS_BOTH); // 253 BOTH
			btn_push_event(btnB, EV_MULTI, MULTI_PRESS_BOTH);
      return;
    }
    if (btnA->long_pressed && btnB->long_pressed) { // BOTH LONG PRESSED
      if (!both_hold_sent) {
				both_hold_sent = 1;
        printf("BOTH hold!\r\n");
				btn_push_event(btnA, EV_MULTI, MULTI_PRESS_BOTH_HOLD); // 254 BOTH_HOLD
				btn_push_event(btnB, EV_MULTI, MULTI_PRESS_BOTH_HOLD);
        return;
      }
    }
    return;
  }

  if (both_pressed && !btnA->pressed && !btnB->pressed) { // BOTH RELEASED
    both_pressed = 0;
    if (!both_release_sent) {
			both_release_sent = 1;
      printf("BOTH release!\r\n");
			btn_push_event(btnA, EV_MULTI, MULTI_PRESS_BOTH_RELEASE); // 250 BOTH_RELEASE
			btn_push_event(btnB, EV_MULTI, MULTI_PRESS_BOTH_RELEASE);
    }
    return;
  }

  // RESET
  both_pressed = 0;
  both_press_sent = 0;
  both_release_sent = 0;
  both_hold_sent = 0;
  return;
}