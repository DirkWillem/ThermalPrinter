#ifndef THERMAL_PRINTER_CONSTANTS_H
#define THERMAL_PRINTER_CONSTANTS_H

#include "driver/gpio.h"
#include "driver/uart.h"

namespace constants {
    static constexpr uart_port_t PrinterUART = UART_NUM_2;
    static constexpr gpio_num_t PrinterTx = GPIO_NUM_16;
    static constexpr gpio_num_t PrinterRx = GPIO_NUM_17;
    static constexpr gpio_num_t Relay = GPIO_NUM_15;

    static constexpr unsigned int PrinterBootMs = 2000;
    static constexpr unsigned int PrinterPowerTimeoutMs = 60 * 1000;
}


#endif //THERMAL_PRINTER_CONSTANTS_H
