#ifndef PET_TRACKER_APP_CONFIG_H_
#define PET_TRACKER_APP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

struct app_config {
	const char *device_id;
	const char *firmware_version;
	int telemetry_interval_seconds;
};

const struct app_config *app_config_get(void);

#ifdef __cplusplus
}
#endif

#endif /* PET_TRACKER_APP_CONFIG_H_ */
