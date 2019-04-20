#ifndef THERMAL_PRINTER_PRINTER_DRIVER_H
#define THERMAL_PRINTER_PRINTER_DRIVER_H

#include <array>
#include <cstdint>

#include "../drivers/uart.h"

/**
 * Class for driving QR701 printers over UART using ESC/Pos
 */
class PrinterDriver {
public:
    /**
     * Constructor
     * @param tx TX pin
     * @param rx RX pin
     * @param port UART port number
     */
    PrinterDriver(gpio_num_t tx, gpio_num_t rx, uart_port_t port = UART_NUM_1);

    /**
     * Writes text to the printer
     * @param text Text to write
     */
    void WriteText(const char* text);

    /**
     * Writes text of a given length to the printer
     * @param text Text to write
     * @param len Length of the text
     */
    void WriteText(const char* text, size_t len);

    /**
     * Enables or disables emphasized mode
     * @param emph Whether emphasized should be enabled
     */
    void SetEmphasized(bool emph);

    /**
     * Sets the default line spacing
     */
    void SetLineSpacing();

    /**
     * Sets a custom line spacing
     * @param spacing Spacing to set
     */
    void SetLineSpacing(uint8_t spacing);

    /**
     * Prints an image
     * @param data Image data
     * @param n_h_dots Number of horizontal dots
     * @param data_len Image data length
     */
    void PrintImage(const uint8_t* data, uint16_t n_h_dots, size_t data_len);

    /**
     * Enables or disables bold mode
     * @param bold Whether bold should be enabled
     */
    void SetBold(bool bold);

    /**
     * Enables or disables inverse printing
     * @param inverse Whether inverse printing should be enabled
     */
    void SetInverse(bool inverse);

private:
    struct PrintMode {
        bool pad_low: 1;
        bool reverse: 1;
        bool up_down: 1;
        bool emphasized: 1;
        bool double_height: 1;
        bool double_width: 1;
        bool delete_line: 1;
        bool pad_high: 1;
    };

    enum class Command : uint8_t {
        SelectPrintMode = 33,
        SetDefaultLineSpacing = 50,
        SetCustomLineSpacing = 51,
        SelectBitImageMode = 42,
        SetInverse = 66,
        SetBold = 69,
    };

    /**
     * Writes a command without data over UART
     * @param cmd Command to write
     */
    void WriteCommand(Command cmd);

    template<typename T>
    /**
     * Writes a command with data over UART
     * @param cmd Command to write
     * @param data Associated data
     * @param cmd_byte Command byte code
     */
    void WriteCommand(Command cmd, const T& data, uint8_t cmd_byte = 27) {
        // Copy data to buffer
        std::array<uint8_t, 2+sizeof(T)> buf;
        buf[0] = cmd_byte;
        buf[1] = static_cast<uint8_t>(cmd);
        *reinterpret_cast<T*>(buf.data() + 2) = data;

        // Write buffer over UART
        m_uart.Write(buf);
    }

    PrintMode m_print_mode;
    UART m_uart;
};

#endif //THERMAL_PRINTER_PRINTER_DRIVER_H
