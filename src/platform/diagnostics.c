#include "platform/diagnostics.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app/app_config.h"

LOG_MODULE_REGISTER(diagnostics, LOG_LEVEL_INF);

void diagnostics_log_boot_banner(void)
{
	const struct app_config *config = app_config_get();

	LOG_INF("Pet Tracker firmware boot");
	LOG_INF("Device ID: %s", config->device_id);
	LOG_INF("Firmware version: %s", config->firmware_version);
	LOG_INF("Telemetry interval: %d seconds", config->telemetry_interval_seconds);
}
