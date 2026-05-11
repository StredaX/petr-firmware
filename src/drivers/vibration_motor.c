#include "drivers/vibration_motor.h"

#include <errno.h>
#include <stdbool.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(vibration_motor, LOG_LEVEL_INF);

#define VIBRATION_NODE DT_ALIAS(vibration0)

#if !DT_NODE_HAS_STATUS(VIBRATION_NODE, okay)
#error "Missing devicetree alias: vibration0"
#endif

static const struct gpio_dt_spec vibration = GPIO_DT_SPEC_GET(VIBRATION_NODE, gpios);
static bool initialized;

int vibration_motor_init(void)
{
	int err;

	if (!gpio_is_ready_dt(&vibration)) {
		LOG_ERR("Vibration motor GPIO device is not ready");
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&vibration, GPIO_OUTPUT_INACTIVE);
	if (err != 0) {
		LOG_ERR("Failed to configure vibration motor GPIO: %d", err);
		return err;
	}

	initialized = true;
	LOG_INF("Vibration motor initialized");

	return 0;
}

int vibration_motor_on(void)
{
	if (!initialized) {
		return -EACCES;
	}

	return gpio_pin_set_dt(&vibration, 1);
}

int vibration_motor_off(void)
{
	if (!initialized) {
		return -EACCES;
	}

	return gpio_pin_set_dt(&vibration, 0);
}

int vibration_motor_pulse_ms(uint32_t duration_ms)
{
	int err;
	uint32_t capped_duration_ms = duration_ms;

	if (!initialized) {
		return -EACCES;
	}

	if (duration_ms == 0U) {
		return -EINVAL;
	}

	if (duration_ms > CONFIG_PET_TRACKER_VIBRATION_MAX_PULSE_MS) {
		capped_duration_ms = CONFIG_PET_TRACKER_VIBRATION_MAX_PULSE_MS;
		LOG_WRN("Vibration pulse capped to %u ms", capped_duration_ms);
	}

	err = vibration_motor_on();
	if (err != 0) {
		return err;
	}

	k_msleep(capped_duration_ms);

	return vibration_motor_off();
}
