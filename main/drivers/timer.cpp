#include "timer.h"

#include <esp_log.h>

Timer::Timer(timer_group_t group, timer_idx_t idx)
    : m_group{group}, m_idx{idx} {
    // Initialize timer
    timer_config_t cfg;
    cfg.divider = Divider;
    cfg.counter_dir = TIMER_COUNT_UP;
    cfg.counter_en = TIMER_PAUSE;
    cfg.alarm_en = TIMER_ALARM_DIS;
    cfg.intr_type = TIMER_INTR_LEVEL;
    ESP_ERROR_CHECK(timer_init(m_group, m_idx, &cfg));

    // Set intitial value & start timer
    ESP_ERROR_CHECK(timer_set_counter_value(m_group, m_idx, 0x00000000ULL));
    ESP_ERROR_CHECK(timer_start(m_group, m_idx));
}

void Timer::AttachInterrupt(unsigned int interval_ms, void (*handler)(void*), void* data) {
    m_fptr = handler;
    m_dptr = data;
    timer_pause(m_group, m_idx);
    ESP_ERROR_CHECK(timer_set_alarm(m_group, m_idx, TIMER_ALARM_EN));
    ESP_ERROR_CHECK(timer_set_auto_reload(m_group, m_idx, TIMER_AUTORELOAD_EN));

    ESP_LOGI("Timer", "Alarm value: %d", interval_ms * Scale / 1000);

    ESP_ERROR_CHECK(timer_set_alarm_value(m_group, m_idx, interval_ms * Scale / 1000));
    ESP_ERROR_CHECK(timer_enable_intr(m_group, m_idx));
    ESP_ERROR_CHECK(timer_isr_register(m_group, m_idx, [](void* arg) {
        auto self = reinterpret_cast<Timer*>(arg);

        decltype(&TIMERG0.int_clr_timers) clr_timers = nullptr;

        // Mark timer as updated
        switch(self->m_group) {
            case TIMER_GROUP_0:
                TIMERG0.hw_timer[self->m_idx].update = 1;
                clr_timers = &TIMERG0.int_clr_timers;
                break;
            case TIMER_GROUP_1:
                TIMERG0.hw_timer[self->m_idx].update = 1;
                clr_timers = &TIMERG1.int_clr_timers;
                break;
            default: break;
        }

        switch(self->m_idx) {
            case TIMER_0: clr_timers->t0 = 1; break;
            case TIMER_1: clr_timers->t1 = 1; break;
            default: break;
        }

        self->m_fptr(self->m_dptr);
    }, this, 0, nullptr));
    timer_start(m_group, m_idx);
}

void Timer::Reset() {
    ESP_ERROR_CHECK(timer_set_counter_value(m_group, m_idx, 0x00000000ULL));
}

unsigned int Timer::Millis() {
    uint64_t val;
    ESP_ERROR_CHECK(timer_get_counter_value(m_group, m_idx, &val));
    return (val*1000) / Scale;
}