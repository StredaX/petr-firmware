#include "app/app_state.h"

#include <errno.h>
#include <stdbool.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_state, LOG_LEVEL_INF);

static device_state_t current_state = DEVICE_STATE_BOOT;

static bool state_is_valid(device_state_t state)
{
	return state >= DEVICE_STATE_BOOT && state <= DEVICE_STATE_ERROR;
}

int app_state_init(device_state_t initial_state)
{
	if (!state_is_valid(initial_state)) {
		LOG_ERR("Invalid initial state: %d", initial_state);
		current_state = DEVICE_STATE_ERROR;
		return -EINVAL;
	}

	current_state = initial_state;
	LOG_INF("Device state initialized: %s", app_state_name(current_state));

	return 0;
}

int app_state_transition(device_state_t next_state, const char *reason)
{
	if (!state_is_valid(next_state)) {
		LOG_ERR("Invalid state transition target: %d", next_state);
		current_state = DEVICE_STATE_ERROR;
		return -EINVAL;
	}

	if (next_state == current_state) {
		LOG_DBG("Device state unchanged: %s", app_state_name(current_state));
		return 0;
	}

	LOG_INF("Device state: %s -> %s (%s)",
		app_state_name(current_state),
		app_state_name(next_state),
		reason != NULL ? reason : "no reason");

	current_state = next_state;

	return 0;
}

device_state_t app_state_get(void)
{
	return current_state;
}

const char *app_state_name(device_state_t state)
{
	switch (state) {
	case DEVICE_STATE_BOOT:
		return "BOOT";
	case DEVICE_STATE_IDLE:
		return "IDLE";
	case DEVICE_STATE_ACTIVE:
		return "ACTIVE";
	case DEVICE_STATE_ALERT:
		return "ALERT";
	case DEVICE_STATE_SLEEP:
		return "SLEEP";
	case DEVICE_STATE_ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}
