#include <errno.h>

#include <zephyr/ztest.h>

#include "app/app_config.h"
#include "app/app_state.h"

ZTEST(app_state_tests, test_initializes_valid_state)
{
	zassert_ok(app_state_init(DEVICE_STATE_BOOT));
	zassert_equal(app_state_get(), DEVICE_STATE_BOOT);

	zassert_ok(app_state_init(DEVICE_STATE_IDLE));
	zassert_equal(app_state_get(), DEVICE_STATE_IDLE);
}

ZTEST(app_state_tests, test_rejects_invalid_initial_state)
{
	zassert_equal(app_state_init((device_state_t)-1), -EINVAL);
	zassert_equal(app_state_get(), DEVICE_STATE_ERROR);
}

ZTEST(app_state_tests, test_transitions_to_valid_state)
{
	zassert_ok(app_state_init(DEVICE_STATE_BOOT));
	zassert_ok(app_state_transition(DEVICE_STATE_ACTIVE, "unit test"));
	zassert_equal(app_state_get(), DEVICE_STATE_ACTIVE);
}

ZTEST(app_state_tests, test_rejects_invalid_transition_target)
{
	zassert_ok(app_state_init(DEVICE_STATE_IDLE));
	zassert_equal(app_state_transition((device_state_t)99, "unit test"), -EINVAL);
	zassert_equal(app_state_get(), DEVICE_STATE_ERROR);
}

ZTEST(app_state_tests, test_state_names_are_stable)
{
	zassert_str_equal(app_state_name(DEVICE_STATE_BOOT), "BOOT");
	zassert_str_equal(app_state_name(DEVICE_STATE_IDLE), "IDLE");
	zassert_str_equal(app_state_name(DEVICE_STATE_ACTIVE), "ACTIVE");
	zassert_str_equal(app_state_name(DEVICE_STATE_ALERT), "ALERT");
	zassert_str_equal(app_state_name(DEVICE_STATE_SLEEP), "SLEEP");
	zassert_str_equal(app_state_name(DEVICE_STATE_ERROR), "ERROR");
	zassert_str_equal(app_state_name((device_state_t)99), "UNKNOWN");
}

ZTEST(app_config_tests, test_default_config)
{
	const struct app_config *config = app_config_get();

	zassert_not_null(config);
	zassert_str_equal(config->device_id, "dev-001");
	zassert_str_equal(config->firmware_version, "test-0.1.0");
	zassert_equal(config->telemetry_interval_seconds, 300);
}

ZTEST_SUITE(app_state_tests, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(app_config_tests, NULL, NULL, NULL, NULL, NULL);
