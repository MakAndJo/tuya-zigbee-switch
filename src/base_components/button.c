#include "button.h"
#include "tl_common.h"
#include "millis.h"

bool btn_debounce(button_t *button, u8 is_pressed);
void btn_update_debounced(button_t *button, u8 is_pressed);

void btn_push_event(button_t *btn, int type, u8 param) {
  if (btn->event_count < BTN_MAX_EVENTS) {
    btn->event_queue[btn->event_count++] = (button_event_t){type, param};
  }
}

void btn_emit_events(button_t *btn) {
  for (u32 i = 0; i < btn->event_count; i++) {
    button_event_t *e = &btn->event_queue[i];
    switch(e->type) {
      case EV_PRESS:   if (btn->on_press) btn->on_press(btn->callback_param); break;
      case EV_RELEASE: if (btn->on_release) btn->on_release(btn->callback_param); break;
      case EV_LONG:    if (btn->on_long_press) btn->on_long_press(btn->callback_param); break;
      case EV_MULTI:   if (btn->on_multi_press) btn->on_multi_press(btn->callback_param, e->param); break;
    }
  }
  btn->event_count = 0;
}

bool btn_read(button_t *button) {
  u8 state = drv_gpio_read(button->pin);
  switch (button->mode) {
    case ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE_INVERSE:
    case ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY_INVERSE:
      state = !state;
      break;
  }
  return state;
}

void btn_init(button_t *button) {
  // During device startup, button may be already pressed, but this should not be detected
  // as user press. So, to avoid such situation, special init is required.
  u8 state = btn_read(button);
  if (!state) {
    button->pressed = true;
    button->long_pressed = true;
  }
}

void btn_update(button_t *button) {
  u8 state = btn_read(button);
  if (btn_debounce(button, state)) {
    btn_update_debounced(button, !state);
  }
}

bool btn_debounce(button_t *button, u8 is_pressed) {
  u32 now = millis();
  if (is_pressed != button->debounce_last_state) {
    button->debounce_last_state = is_pressed;
    button->debounce_last_change = now;
  }
  return (now - button->debounce_last_change) > DEBOUNCE_DELAY_MS;
}

void btn_update_debounced(button_t *button, u8 is_pressed)
{
  u32 now = millis();

  if (!button->pressed && is_pressed)
  {
    printf("Press detected\r\n");
    button->pressed_at_ms = now;
    btn_push_event(button, EV_PRESS, 0);
    // if (button->on_press != NULL)
    // {
    //   button->on_press(button->callback_param);
    // }
    if (now - button->released_at_ms < button->multi_press_duration_ms)
    {
      button->multi_press_cnt += 1;
      printf("Multi press detected: %d\r\n", button->multi_press_cnt);
      btn_push_event(button, EV_MULTI, button->multi_press_cnt);
      // if (button->on_multi_press != NULL)
      // {
      //   button->on_multi_press(button->callback_param, button->multi_press_cnt);
      // }
    }
    else
    {
      button->multi_press_cnt = 1;
    }
  }
  else if (button->pressed && !is_pressed)
  {
    printf("Release detected\r\n");
    button->released_at_ms = now;
    button->long_pressed   = false;
    btn_push_event(button, EV_RELEASE, 0);
    // if (button->on_release != NULL)
    // {
    //   button->on_release(button->callback_param);
    // }
  }
  button->pressed = is_pressed;
  if (is_pressed && !button->long_pressed && (button->long_press_duration_ms > 0) && (button->long_press_duration_ms < (now - button->pressed_at_ms)))
  {
    button->long_pressed = true;
    printf("Long press detected\r\n");
    btn_push_event(button, EV_LONG, 0);
    // if (button->on_long_press != NULL)
    // {
    //   button->on_long_press(button->callback_param);
    // }
  }
  ;
}
