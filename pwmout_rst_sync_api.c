#include "mbed_assert.h"
#include "pwmout_rst_sync_api.h"
#include "pinmap.h"
#include "RZ_A1_Init.h"
#include "gpio_addrdefine.h"


#define MTU2_PWM_NUM            3

/*  PORT ID, MTU2_PWMType, Pin function */
static const PinMap PinMap_PWM[] = {
    {P7_9     , 0  , 7},
    {P8_11    , 0  , 4},
    {P3_5     , 0  , 6},
    {P7_12    , 1  , 7},
    {P3_8     , 1  , 6},
    {P4_4     , 1  , 3},
    {P11_0    , 1  , 2},
    {P7_13    , 2  , 7},
    {P3_9     , 2  , 6},
    {P4_5     , 2  , 3},
    {P11_1    , 2  , 2},
    {NC       , NC , 0}
};

static __IO uint16_t *MTU2_PWM_MATCH[MTU2_PWM_NUM] = {
    { &MTU2TGRD_3 },
    { &MTU2TGRC_4 },
    { &MTU2TGRD_4 } 
};

static int32_t  mtu2_period    = 1000;    /* default period 1ms */
static uint8_t  mtu2_toer      = 0xc0;
static uint8_t  init_end       = 0;

void pwmout_rst_sync_init(pwmout_rst_sync_t* obj, PinName pin) {
    /* Port setting for S/W I/O Contorol */
    int pwm_type   = pinmap_peripheral(pin, PinMap_PWM);
    MBED_ASSERT(pwm_type != NC);
    MBED_ASSERT((mtu2_toer & (0x01 << pwm_type)) == 0);

    obj->pwm_type   = pwm_type;
    mtu2_toer |= (0x01 << obj->pwm_type);

    /* Wire pinout */
    pinmap_pinout(pin, PinMap_PWM);
    *PMSR(PINGROUP(pin)) = ((1 << (pin  & 0xf)) << 16) | 0;

    /* Mosule stop 33(MTU2) canceling */
    CPGSTBCR3 &= ~(CPG_STBCR3_BIT_MSTP33);

    /* default duty 0.0f */
    if (init_end == 0) {
        MTU2TSTR     &= 0xBF;
        MTU2TGRB_3    = MTU2TGRD_3 = 0;
        MTU2TGRA_4    = MTU2TGRC_4 = 0;
        MTU2TGRB_4    = MTU2TGRD_4 = 0;
        init_end = 1;
    }

    pwmout_rst_sync_period_us(mtu2_period);
}

void pwmout_rst_sync_free(pwmout_rst_sync_t* obj) {
    pwmout_rst_sync_write(obj, 0);
    mtu2_toer &= ~(0x01 << obj->pwm_type);
    if (mtu2_toer == 0xc0) {
        init_end = 0;
    }
}

void pwmout_rst_sync_write(pwmout_rst_sync_t* obj, float value) {
    uint32_t wk_cycle;
    uint16_t wk_set_value;

    if (value < 0.0f) {
        value = 0.0f;
    } else if (value > 1.0f) {
        value = 1.0f;
    } else {
        /* Do Nothing */
    }
    wk_cycle = MTU2TGRA_3 & 0xffff;
    wk_set_value = (uint16_t)((float)wk_cycle * value);
    if ((wk_set_value == wk_cycle) && (wk_set_value != 0)) {
        wk_set_value -= 1;
    }
    /* set channel match to percentage */
    *MTU2_PWM_MATCH[obj->pwm_type] = wk_set_value;
}

float pwmout_rst_sync_read(pwmout_rst_sync_t* obj) {
    uint32_t wk_cycle;
    float value;
    uint32_t wk_pulse;

    wk_cycle = MTU2TGRA_3 & 0xffff;
    if (wk_cycle != 0) {
        wk_pulse = *MTU2_PWM_MATCH[obj->pwm_type];
        value = ((float)wk_pulse / (float)wk_cycle);
    } else {
        value = 0.0f;
    }

    return (value > 1.0f) ? (1.0f) : (value);
}

void pwmout_rst_sync_period(float seconds) {
    pwmout_rst_sync_period_us(seconds * 1000000.0f);
}

void pwmout_rst_sync_period_ms(int ms) {
    pwmout_rst_sync_period_us(ms * 1000);
}

static void set_mtu2_duty_again(__IO uint16_t *p_pwmpbfr, uint16_t last_cycle, uint16_t new_cycle){
    uint16_t wk_pwmpbfr;
    float    value;

    if (last_cycle != 0) {
        wk_pwmpbfr = *p_pwmpbfr;
        value      = ((float)(wk_pwmpbfr & 0xffff) / (float)last_cycle);
        *p_pwmpbfr = (uint16_t)((float)new_cycle * value);
    } else {
        *p_pwmpbfr = 0;
    }
}

/* Set the PWM period, keeping the duty cycle the same. */
void pwmout_rst_sync_period_us(int us) {
    uint64_t wk_cycle_mtu2;
    uint32_t pclk_base;
    uint32_t wk_cycle;
    uint32_t wk_cks = 0;
    uint16_t wk_last_cycle;
    int      max_us = 0;

    max_us = 2000000;
    if (us > max_us) {
        us = max_us;
    } else if (us < 1) {
        us = 1;
    } else {
        /* Do Nothing */
    }

    if (RZ_A1_IsClockMode0() == false) {
        pclk_base = (uint32_t)CM1_RENESAS_RZ_A1_P0_CLK;
    } else {
        pclk_base = (uint32_t)CM0_RENESAS_RZ_A1_P0_CLK;
    }

    wk_cycle_mtu2 = (uint64_t)pclk_base * us;
    while (wk_cycle_mtu2 >= 65535000000) {
        wk_cycle_mtu2 >>= 2;
        wk_cks++;
    }
    wk_cycle = (uint32_t)(wk_cycle_mtu2 / 1000000);

    /* Counter Stop */
    MTU2TSTR     &= 0xBF;
    wk_last_cycle = MTU2TGRA_3;
    MTU2TCR_3     = 0x20 + wk_cks;        /* TCNT Clear(TGRA), P0φ/1  */
    MTU2TOCR1     = 0x04;                 /*                          */
    MTU2TOCR2     = 0x40;                 /* N L>H  P H>L             */
    MTU2TMDR_3    = 0x38;                 /* Buff:ON Reset-Synchronized PWM mode */
    MTU2TMDR_4    = 0x30;                 /* Buff:ON                  */
    MTU2TOER      = mtu2_toer;            /* enabled output           */
    MTU2TCNT_3    = MTU2TCNT_4 = 0;       /* TCNT3,TCNT4 Set 0        */
    MTU2TGRA_3    = MTU2TGRC_3 = (uint16_t)wk_cycle;

    set_mtu2_duty_again(MTU2_PWM_MATCH[0], wk_last_cycle, wk_cycle);
    set_mtu2_duty_again(MTU2_PWM_MATCH[1], wk_last_cycle, wk_cycle);
    set_mtu2_duty_again(MTU2_PWM_MATCH[2], wk_last_cycle, wk_cycle);

    MTU2TSTR     |= 0x40;                 /* TCNT_4 Start             */

    /* Save for future use */
    mtu2_period = us;
}

void pwmout_rst_sync_pulsewidth(pwmout_rst_sync_t* obj, float seconds) {
    pwmout_rst_sync_pulsewidth_us(obj, seconds * 1000000.0f);
}

void pwmout_rst_sync_pulsewidth_ms(pwmout_rst_sync_t* obj, int ms) {
    pwmout_rst_sync_pulsewidth_us(obj, ms * 1000);
}

void pwmout_rst_sync_pulsewidth_us(pwmout_rst_sync_t* obj, int us) {
    float value = 0;

    if (mtu2_period != 0) {
        value = (float)us / (float)mtu2_period;
    }
    pwmout_rst_sync_write(obj, value);
}
