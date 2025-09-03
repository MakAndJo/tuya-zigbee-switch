#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "types.h"

typedef void (*ev_button_callback_t)(void *);
typedef void (*ev_button_multi_press_callback_t)(void *param, u8 press_cnt);

#define BTN_MAX_EVENTS 4

#define ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE                  0x00
#define ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY               0x01
#define ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MULTIFUNCTION           0x02 // unused
#define ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_TOGGLE_INVERSE          0x10 // toggle inverse
#define ZCL_ONOFF_CONFIGURATION_SWITCH_MODE_MOMENTARY_INVERSE       0x11 // momentary inverse

#define MULTI_PRESS_CNT_TO_RESET    10
#define MULTI_PRESS_BOTH            252
#define MULTI_PRESS_BOTH_RELEASE    250
#define MULTI_PRESS_BOTH_HOLD       254

#define DEBOUNCE_DELAY_MS 50

typedef struct {
  enum {EV_PRESS, EV_RELEASE, EV_LONG, EV_MULTI} type;
  u8 param;
} button_event_t;

typedef struct
{
  u8                               mode;
  u32                              pin;
  u8                               pressed;
  u8                               long_pressed;
  u32                              pressed_at_ms;
  u32                              released_at_ms;
  u32                              long_press_duration_ms;
  u32                              multi_press_duration_ms;
  u8                               multi_press_cnt;
  u8                               debounce_last_state;
  u32                              debounce_last_change;
  ev_button_callback_t             on_press;
  ev_button_callback_t             on_long_press;
  ev_button_callback_t             on_release;
  ev_button_multi_press_callback_t on_multi_press;
  void *                           callback_param;
  button_event_t                   event_queue[BTN_MAX_EVENTS];
  u32                              event_count;
}button_t;


void btn_init(button_t *button);
void btn_update(button_t *button);
void btn_push_event(button_t *btn, int type, u8 param);
void btn_emit_events(button_t *btn);

#endif
