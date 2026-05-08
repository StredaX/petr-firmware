#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app/app_state.h"
#include "drivers/buzzer.h"
#include "platform/diagnostics.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	diagnostics_log_boot_banner();

	if (app_state_init(DEVICE_STATE_BOOT) != 0) {
		return 0;
	}

	if (app_state_transition(DEVICE_STATE_IDLE, "boot complete") != 0) {
		return 0;
	}

	if (IS_ENABLED(CONFIG_PET_TRACKER_BUZZER)) {
		int err = buzzer_init();

		if (err != 0) {
			LOG_ERR("Buzzer init failed: %d", err);
		} else if (IS_ENABLED(CONFIG_PET_TRACKER_BUZZER_STARTUP_MELODY)) {
			LOG_INF("Running buzzer startup melody");
			err = buzzer_startup_melody();
			if (err != 0) {
				LOG_ERR("Buzzer startup melody failed: %d", err);
			} else {
				LOG_INF("Buzzer startup melody complete");
			}
		}
	}

	for (;;) {
		LOG_INF("Heartbeat: state=%s", app_state_name(app_state_get()));
		k_sleep(K_SECONDS(CONFIG_PET_TRACKER_HEARTBEAT_INTERVAL_SECONDS));
	}

	return 0;
}
