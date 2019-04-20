#include "context.h"

#include "constants.h"

#include <cstring>

#include <esp_log.h>

constexpr uint8_t EmphasizedStyle = 1 << 0;
constexpr uint8_t BoldStyle = 1 << 1;
constexpr uint8_t InverseStyle = 1 << 2;

Context::Context()
    : m_prev_style{},
      m_printer_driver{constants::PrinterTx, constants::PrinterRx, constants::PrinterUART},
      m_relay{constants::Relay},
      m_relay_timer{TIMER_GROUP_0, TIMER_0}, m_power_state{PowerState::Off} {
    m_prev_style.byte = 0;
    m_relay = false;

    xTaskCreate(&Context::CheckPowerTaskHandler, "power_check_task", 1024, this, 5, nullptr);
}

void Context::Print(const uint8_t* data, size_t data_len) {
    // Ensure the printer is powered on
    switch(m_power_state) {
        case PowerState::Off:
            PowerPrinter();
            vTaskDelay(constants::PrinterBootMs / portTICK_PERIOD_MS);
            break;
        case PowerState::Boot:
            vTaskDelay((constants::PrinterBootMs - m_relay_timer.Millis()) / portTICK_PERIOD_MS);
            break;
        default: break;
    }

    m_relay_timer.Reset();

    // Iterate over all data
    size_t ptr{0};

    while (ptr < data_len) {
        // Retrieve length of the current command
        uint16_t cur_cmd_len = *reinterpret_cast<const uint16_t*>(data+ptr);
        ptr += sizeof(uint16_t);

        // Read the command
        auto cmd = static_cast<Command>(data[ptr]);
        ptr += sizeof(Command);

        // Perform the command
        size_t data_len = cur_cmd_len -  (sizeof(uint16_t) + sizeof(Command));
        Perform(cmd, &data[ptr], data_len);

        // Increment data pointer
        ptr += data_len;
    }
}

void Context::Perform(Command cmd, const uint8_t* data, size_t data_len) {
    switch(cmd) {
        case Command::PrintText: {
            char buf[data_len + 1];
            memcpy(buf, data, data_len);
            buf[data_len] = 0;
            ESP_LOGI("Printer", "Printing: %s", buf);

            m_printer_driver.WriteText(reinterpret_cast<const char*>(data), data_len);
            ESP_LOGI("Printer", "Done printing");
            break;
        }
        case Command::SetTextStyle: {
            // Retrieve text style
            TextStyleOrByte style{};
            style.byte = data[0];

            ESP_LOGI("Printer", "Setting style: 0x%02x", style.byte);

            // Check whether the style has changed. If so, update it
            if (m_prev_style.byte != style.byte) {
                if (style.style.bold != m_prev_style.style.bold)
                    m_printer_driver.SetBold(style.style.bold);
                if (style.style.emphasized != m_prev_style.style.emphasized)
                    m_printer_driver.SetEmphasized(style.style.emphasized);
                if (style.style.inverse != m_prev_style.style.inverse)
                    m_printer_driver.SetInverse(style.style.inverse);
            }

            // Update previous text style
            m_prev_style = style;
            break;
        }
        case Command::PrintImage: {

            // Set custom line spacing
            m_printer_driver.SetLineSpacing(24);

            // Retrieve number of horizontal dots
            uint16_t h_dots = reinterpret_cast<const uint16_t*>(data)[0];

            ESP_LOGI("Printer", "Printing image with %d horizontal dots", h_dots);

            m_printer_driver.PrintImage(data+sizeof(uint16_t), h_dots, data_len-sizeof(uint16_t));

            // Restore line spacing
            m_printer_driver.SetLineSpacing();
        }
        default: break;
    }
}

void Context::PowerPrinter() {
    // If we're in Off, power on and reset the relay timer
    if (m_power_state == PowerState::Off) {
        m_power_state = PowerState::Boot;
        m_relay = true;
        m_relay_timer.Reset();
    } else if (m_power_state == PowerState::Active) {
        m_relay_timer.Reset();
    }
}

void Context::CheckPowerTaskHandler(void* data) {
    auto self = reinterpret_cast<Context*>(data);

    while(true) {
        self->CheckPower();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void Context::CheckPower() {
    switch(m_power_state) {
        case PowerState::Off: break;
        case PowerState::Boot:
            if (m_relay_timer.Millis() >= constants::PrinterBootMs) {
                m_relay_timer.Reset();
                m_power_state = PowerState::Active;
            }
        case PowerState::Active:
            if (m_relay_timer.Millis() >= constants::PrinterPowerTimeoutMs) {
                m_relay = false;
                m_power_state = PowerState::Off;
            }
    }
}