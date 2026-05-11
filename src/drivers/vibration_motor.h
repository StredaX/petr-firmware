#ifndef PET_TRACKER_VIBRATION_MOTOR_H_
#define PET_TRACKER_VIBRATION_MOTOR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int vibration_motor_init(void);
int vibration_motor_on(void);
int vibration_motor_off(void);
int vibration_motor_pulse_ms(uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif /* PET_TRACKER_VIBRATION_MOTOR_H_ */
