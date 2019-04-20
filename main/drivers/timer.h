#ifndef THERMAL_PRINTER_TIMER_H
#define THERMAL_PRINTER_TIMER_H

#include "driver/timer.h"


class Timer {
public:
    Timer(timer_group_t group, timer_idx_t idx);

    template<typename T>
    /**
     * Attaches a method as an interrupt handler to the timer
     * @tparam T Class to call the method on
     * @param interval_ms Interval of the timer in milliseconds
     * @param ptr Pointer to the instance to call the handler on
     * @param handler Method pointer to the method to call
     */
    void AttachInterrupt(unsigned int interval_ms, T* ptr, void (T::*handler)()) {
        m_mptr1 = ptr;
        m_mptr2 = handler;

        AttachInterrupt(interval_ms, [](void* arg) {
            auto self = reinterpret_cast<Timer*>(arg);
            auto c_ptr = reinterpret_cast<T*>(self->m_mptr1);
            auto m_ptr = reinterpret_cast<void (T::*)()>(self->m_mptr2);
            c_ptr->*m_ptr();
        }, this);
    }

    /**
     * Attaches an interrupt handler to the timer
     * @param interval_ms Interval of the timer in milliseconds
     * @param handler Handler to attach
     * @param data Data to pass to the ISR
     */
    void AttachInterrupt(unsigned int interval_ms, void (*handler)(void*), void* data);

    /**
     * Resets the timer
     */
    void Reset();

    /**
     * Returns the number of milliseconds the timer has been running
     * @return Number of milliseconds
     */
    unsigned int Millis();
private:
    void* m_dptr;
    void (*m_fptr)(void*);
    void* m_mptr1;
    void* m_mptr2;

    timer_group_t m_group;
    timer_idx_t m_idx;

    static constexpr auto Divider = 16;
    static constexpr auto Scale = TIMER_BASE_CLK / Divider;
};

#endif //THERMAL_PRINTER_TIMER_H
