#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app/app_state.h"
#include "drivers/buzzer.h"
#include "drivers/vibration_motor.h"
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
		} else if (IS_ENABLED(CONFIG_PET_TRACKER_BUZZER_STARTUP_TONE)) {
			LOG_INF("Running buzzer startup tone");
			err = buzzer_startup_tone();
			if (err != 0) {
				LOG_ERR("Buzzer startup tone failed: %d", err);
			} else {
				LOG_INF("Buzzer startup tone complete");
			}
		}
	}

	if (IS_ENABLED(CONFIG_PET_TRACKER_VIBRATION_MOTOR)) {
		int err = vibration_motor_init();

		if (err != 0) {
			LOG_ERR("Vibration motor init failed: %d", err);
		} else if (IS_ENABLED(CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST)) {
			LOG_INF("Running vibration motor boot test");
			for (int i = 0; i < CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST_REPEATS; i++) {
				LOG_INF("Vibration motor boot test pulse %d/%d",
					i + 1,
					CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST_REPEATS);
				err = vibration_motor_pulse_ms(CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST_MS);
				if (err != 0) {
					LOG_ERR("Vibration motor boot test failed: %d", err);
					break;
				}
				k_msleep(500);
			}
			if (err == 0) {
				LOG_INF("Vibration motor boot test complete");
			}
		}
	}

	for (;;) {
		LOG_INF("Heartbeat: state=%s", app_state_name(app_state_get()));
		k_sleep(K_SECONDS(CONFIG_PET_TRACKER_HEARTBEAT_INTERVAL_SECONDS));
	}

	return 0;
}
