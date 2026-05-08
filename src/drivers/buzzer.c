#include "drivers/buzzer.h"

#include <errno.h>
#include <stdbool.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(buzzer, LOG_LEVEL_INF);

#define BUZZER_NODE DT_ALIAS(buzzer0)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
#error "Missing devicetree alias: buzzer0"
#endif

static const struct gpio_dt_spec buzzer = GPIO_DT_SPEC_GET(BUZZER_NODE, gpios);
static bool initialized;

int buzzer_init(void)
{
	int err;

	if (!gpio_is_ready_dt(&buzzer)) {
		LOG_ERR("Buzzer GPIO device is not ready");
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&buzzer, GPIO_OUTPUT_INACTIVE);
	if (err != 0) {
		LOG_ERR("Failed to configure buzzer GPIO: %d", err);
		return err;
	}

	initialized = true;
	LOG_INF("Buzzer initialized");

	return 0;
}

int buzzer_on(void)
{
	if (!initialized) {
		return -EACCES;
	}

	return gpio_pin_set_dt(&buzzer, 1);
}

int buzzer_off(void)
{
	if (!initialized) {
		return -EACCES;
	}

	return gpio_pin_set_dt(&buzzer, 0);
}

int buzzer_beep_ms(uint32_t on_ms, uint32_t off_ms, uint8_t repeats)
{
	int err;

	if (!initialized) {
		return -EACCES;
	}

	for (uint8_t i = 0; i < repeats; i++) {
		err = buzzer_on();
		if (err != 0) {
			return err;
		}

		k_msleep(on_ms);

		err = buzzer_off();
		if (err != 0) {
			return err;
		}

		if (i + 1U < repeats) {
			k_msleep(off_ms);
		}
	}

	return 0;
}

int buzzer_startup_melody(void)
{
	static const struct {
		uint16_t on_ms;
		uint16_t off_ms;
	} steps[] = {
		{ 70, 45 },
		{ 70, 45 },
		{ 120, 90 },
		{ 70, 45 },
		{ 70, 120 },
		{ 150, 70 },
		{ 80, 45 },
		{ 80, 45 },
		{ 180, 100 },
		{ 320, 0 },
	};

	int err;

	if (!initialized) {
		return -EACCES;
	}

	for (size_t i = 0; i < ARRAY_SIZE(steps); i++) {
		err = buzzer_on();
		if (err != 0) {
			return err;
		}

		k_msleep(steps[i].on_ms);

		err = buzzer_off();
		if (err != 0) {
			return err;
		}

		if (steps[i].off_ms > 0U) {
			k_msleep(steps[i].off_ms);
		}
	}

	return 0;
}
