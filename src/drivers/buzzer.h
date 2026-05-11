#ifndef PET_TRACKER_BUZZER_H_
#define PET_TRACKER_BUZZER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int buzzer_init(void);
int buzzer_on(void);
int buzzer_off(void);
int buzzer_beep_ms(uint32_t on_ms, uint32_t off_ms, uint8_t repeats);
int buzzer_startup_tone(void);

#ifdef __cplusplus
}
#endif

#endif /* PET_TRACKER_BUZZER_H_ */
