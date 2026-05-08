#include "app/app_config.h"

#include <zephyr/kernel.h>

static const struct app_config default_config = {
	.device_id = "dev-001",
	.firmware_version = CONFIG_PET_TRACKER_FIRMWARE_VERSION,
	.telemetry_interval_seconds = 300,
};

const struct app_config *app_config_get(void)
{
	return &default_config;
}
