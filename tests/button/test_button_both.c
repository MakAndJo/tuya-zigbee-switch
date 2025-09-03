#include "unity/unity.h"
#include "base_components/button.h"
#include "base_components/button_combiner.h"
#include "base_components/millis.h"
#include "stubs/gpio.h"
#include "stubs/clock.h"
#include "chip_8258/gpio.h"
#include <string.h>

static button_t btnA;
static button_t btnB;

static void tick_once() {
	stub_clock_advance(1);
	millis_update();
	btn_update(&btnA);
	btn_update(&btnB);
	both_btn_update(&btnA, &btnB);
	btn_emit_events(&btnA);
	btn_emit_events(&btnB);
}

void simulate_time_ms(u32 ms) {
  for (u32 i = 0; i < ms; i++) tick_once();
}


// stub для both
static u8 both_multi_press_last = 0;
void both_multi_callback(void *param, u8 cnt) {
  both_multi_press_last = cnt;
}

void setUpBoth(void) {
	memset(&btnA,0,sizeof(btnA));
	memset(&btnB,0,sizeof(btnB));

	btnA.pin = GPIO_PA0;
	btnB.pin = GPIO_PA1;

	btnA.multi_press_duration_ms = 200;
	btnB.multi_press_duration_ms = 200;

	btnA.on_multi_press = both_multi_callback;
	btnB.on_multi_press = both_multi_callback;

	gpio_state[btnA.pin] = 1;
	gpio_state[btnB.pin] = 1;

	stub_clock_set(0);
	millis_init();
	btn_init(&btnA);
	btn_init(&btnB);
	simulate_time_ms(1000); // чтобы не было "фейковых" нажатий
}

static void press_both_for(u32 ms) {
	gpio_state[btnA.pin] = 0;
	gpio_state[btnB.pin] = 0;
	tick_once(); // обновляем состояние и очередь
	simulate_time_ms(ms);
}

static void release_both_for(u32 ms) {
	gpio_state[btnA.pin] = 1;
	gpio_state[btnB.pin] = 1;
	tick_once();
	simulate_time_ms(ms);
}

void test_both_press_release(void) {
  setUpBoth();

	press_both_for(DEBOUNCE_DELAY_MS + 10);
	both_btn_update(&btnA, &btnB);
	btn_emit_events(&btnA);
	btn_emit_events(&btnB);

	TEST_ASSERT_EQUAL_INT(MULTI_PRESS_BOTH, both_multi_press_last);

	release_both_for(DEBOUNCE_DELAY_MS + 10);
	both_btn_update(&btnA, &btnB);
	btn_emit_events(&btnA);
	btn_emit_events(&btnB);

	TEST_ASSERT_EQUAL_INT(MULTI_PRESS_BOTH_RELEASE, both_multi_press_last);
}

void test_both_long_hold(void) {
	setUpBoth();

	// Устанавливаем длительность long press до начала нажатия
	btnA.long_press_duration_ms = 100;
	btnB.long_press_duration_ms = 100;

	press_both_for(DEBOUNCE_DELAY_MS + 10); // обычное нажатие, чтобы триггернулся BOTH
	tick_once(); // один тик после дебаунса

	simulate_time_ms(150); // удерживаем больше long_press_duration_ms
	tick_once(); // обновляем состояние и генерируем long press

	TEST_ASSERT_EQUAL_INT(MULTI_PRESS_BOTH_HOLD, both_multi_press_last);

	release_both_for(DEBOUNCE_DELAY_MS + 10);
	tick_once();
}


void run_test_button_both() {
	RUN_TEST(test_both_press_release);
	RUN_TEST(test_both_long_hold);
}