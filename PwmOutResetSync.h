
#ifndef PWMOUT_RESET_SYNC_H
#define PWMOUT_RESET_SYNC_H

// #define TARGET_RZ_A1H

// #if !defined(TARGET_RZ_A1H)
// #error The PwmOutResetSync library is not supported on this target
// #endif

#include "platform.h"
#include "pwmout_rst_sync_api.h"

/** A pulse-width modulation digital output
 *
 * Example
 * @code
 * // Moter Control.
 * #include "mbed.h"
 * #include "PwmOutResetSync.h"
 *
 * PwmOutResetSync pwm_l(P4_4);
 * PwmOutResetSync pwm_r(P4_5);
 *
 * int main() {
 *     while(1) {
 *         pwm_l = 0.5;
 *         pwm_r = 0.5;
 *         wait(1);
 *         pwm_l = 0.0;
 *         pwm_r = 0.8;
 *         wait(1);
 *     }
 * }
 * @endcode
 *
 * @note
 *  The PWMs all share the same period - if you change the period for
 *  one, you change it for all.
 *  Routines that change the period maintain the duty cycle for its PWM.
 */
class PwmOutResetSync {

public:

    /** Create a PwmOutResetSync connected to the specified pin
     *
     *  @param pin PwmOutResetSync pin to connect to
     *
     *  @attention
     *   Output Pins for Reset-Synchronized PWM Mode.
     *   [TIOC3B : P8_11, (P7_9), (P3_5)],
     *   [TIOC4A : P3_8, P4_4, (P7_12), (P11_0)],
     *   [TIOC4B : P3_9, P4_5, (P7_13), (P11_1)]
     *   ():On the GR-PEACH, it can not be used.
     *
     */
    PwmOutResetSync(PinName pin) {
        pwmout_rst_sync_init(&_pwm, pin);
    }

    /** Destructor
     *
     */
    virtual ~PwmOutResetSync() {
        pwmout_rst_sync_free(&_pwm);
    }

    /** Set the ouput duty-cycle, specified as a percentage (float)
     *
     *  @param value A floating-point value representing the output duty-cycle,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *    Values outside this range will be saturated to 0.0f or 1.0f.
     */
    void write(float value) {
        pwmout_rst_sync_write(&_pwm, value);
    }

    /** Return the current output duty-cycle setting, measured as a percentage (float)
     *
     *  @returns
     *    A floating-point value representing the current duty-cycle being output on the pin,
     *    measured as a percentage. The returned value will lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *
     *  @note
     *  This value may not match exactly the value set by a previous <write>.
     */
    float read() {
        return pwmout_rst_sync_read(&_pwm);
    }

    /** Set the PWM period, specified in seconds (float), keeping the duty cycle the same.
     *
     *  @param seconds Specified in seconds (float). The maximum period is 2s. When you set
     *   a number greater than 2s, 2s is set.
     *
     *  @note
     *   The resolution is currently in microseconds; periods smaller than this
     *   will be set to zero.
     */
    static void period(float seconds) {
        pwmout_rst_sync_period(seconds);
    }

    /** Set the PWM period, specified in milli-seconds (int), keeping the duty cycle the same.
     *
     *  @param seconds Specified in milli-seconds (int). The maximum period is 2s. When you set
     *   a number greater than 2s, 2s is set.
     */
    static void period_ms(int ms) {
        pwmout_rst_sync_period_ms(ms);
    }

    /** Set the PWM period, specified in micro-seconds (int), keeping the duty cycle the same.
     *
     *  @param seconds Specified in micro-seconds (int). The maximum period is 2s. When you set
     *   a number greater than 2s, 2s is set.
     */
    static void period_us(int us) {
        pwmout_rst_sync_period_us(us);
    }

    /** Set the PWM pulsewidth, specified in seconds (float), keeping the period the same.
     */
    void pulsewidth(float seconds) {
        pwmout_rst_sync_pulsewidth(&_pwm, seconds);
    }

    /** Set the PWM pulsewidth, specified in milli-seconds (int), keeping the period the same.
     */
    void pulsewidth_ms(int ms) {
        pwmout_rst_sync_pulsewidth_ms(&_pwm, ms);
    }

    /** Set the PWM pulsewidth, specified in micro-seconds (int), keeping the period the same.
     */
    void pulsewidth_us(int us) {
        pwmout_rst_sync_pulsewidth_us(&_pwm, us);
    }

    /** A operator shorthand for write()
     */
    PwmOutResetSync& operator= (float value) {
        write(value);
        return *this;
    }

    PwmOutResetSync& operator= (PwmOutResetSync& rhs) {
        write(rhs.read());
        return *this;
    }

    /** An operator shorthand for read()
     */
    operator float() {
        return read();
    }

protected:
    pwmout_rst_sync_t _pwm;
};

#endif
