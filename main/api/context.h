#ifndef THERMAL_PRINTER_CONTEXT_H
#define THERMAL_PRINTER_CONTEXT_H

#include <cstdint>

#include "drivers/gpo.h"
#include "drivers/timer.h"

#include "printer/printer_driver.h"

/**
 * Contains the context for the thermal printer API
 */
class Context {
public:
    /**
     * Constructor
     */
    Context();

    /**
     * Prints according to the binary data provided in the data buffer
     * @param data Data buffer
     * @param data_len Length of the data buffer
     */
    void Print(const uint8_t* data, size_t data_len);

    /**
     * Powers up the printer
     */
    void PowerPrinter();

private:
    /**
     * RTOS task handler for checking whether the power should still be on
     * @param data Data to pass to the task
     */
    static void CheckPowerTaskHandler(void* data);

    /**
     * Contains the possible commands that can be sent to the API
     */
    enum class Command : uint8_t {
        PrintText = 0x00,
        SetTextStyle = 0x01,
        PrintImage = 0x10
    };

    /**
     * Contains the current text style of the API
     */
    struct TextStyle {
        bool emphasized: 1;
        bool bold: 1;
        bool inverse: 1;
    };

    /**
     * Union for working with text style as a
     */
    union TextStyleOrByte {
        TextStyle style;
        uint8_t byte;
    };

    /**
     * Contains the possible power states of the printer
     */
    enum class PowerState {
        Off,
        Boot,
        Active,
    };

    void Perform(Command cmd, const uint8_t* data, size_t data_len);

    void CheckPower();

    TextStyleOrByte m_prev_style;
    PrinterDriver m_printer_driver;
    GPO m_relay;
    Timer m_relay_timer;
    PowerState m_power_state;
};

#endif //THERMAL_PRINTER_CONTEXT_H
