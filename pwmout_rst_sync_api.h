#ifndef PWMOUT_RST_SYNC_API_H
#define PWMOUT_RST_SYNC_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pwmout_rst_sync_s {
    int pwm_type;
    int pwm_neg_ph;
} pwmout_rst_sync_t;

void pwmout_rst_sync_init         (pwmout_rst_sync_t* obj, PinName pin);
void pwmout_rst_sync_free         (pwmout_rst_sync_t* obj);

void  pwmout_rst_sync_write       (pwmout_rst_sync_t* obj, float percent);
float pwmout_rst_sync_read        (pwmout_rst_sync_t* obj);

void pwmout_rst_sync_period       (float seconds);
void pwmout_rst_sync_period_ms    (int ms);
void pwmout_rst_sync_period_us    (int us);

void pwmout_rst_sync_pulsewidth   (pwmout_rst_sync_t* obj, float seconds);
void pwmout_rst_sync_pulsewidth_ms(pwmout_rst_sync_t* obj, int ms);
void pwmout_rst_sync_pulsewidth_us(pwmout_rst_sync_t* obj, int us);

#ifdef __cplusplus
}
#endif

#endif


