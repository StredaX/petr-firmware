#ifndef PET_TRACKER_APP_STATE_H_
#define PET_TRACKER_APP_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DEVICE_STATE_BOOT,
	DEVICE_STATE_IDLE,
	DEVICE_STATE_ACTIVE,
	DEVICE_STATE_ALERT,
	DEVICE_STATE_SLEEP,
	DEVICE_STATE_ERROR,
} device_state_t;

int app_state_init(device_state_t initial_state);
int app_state_transition(device_state_t next_state, const char *reason);
device_state_t app_state_get(void);
const char *app_state_name(device_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* PET_TRACKER_APP_STATE_H_ */
