#include "printer_driver.h"

#include <cstring>

PrinterDriver::PrinterDriver(gpio_num_t tx, gpio_num_t rx, uart_port_t port)
        : m_print_mode{}, m_uart{tx, rx, port} { }

void PrinterDriver::WriteText(const char* text) {
    m_uart.Write(text, strlen(text));
}

void PrinterDriver::WriteText(const char* text, size_t len) {
    m_uart.Write(text, len);
}

void PrinterDriver::SetEmphasized(bool emph) {
    m_print_mode.emphasized = emph;
    WriteCommand(Command::SelectPrintMode, m_print_mode);
}

void PrinterDriver::SetBold(bool bold) {
    WriteCommand(Command::SetBold, bold ? 1 : 0);
}

void PrinterDriver::SetInverse(bool inverse) {
    WriteCommand(Command::SetInverse, inverse ? 1 : 0, 29);
}

void PrinterDriver::SetLineSpacing() {
    WriteCommand(Command::SetDefaultLineSpacing);
}

void PrinterDriver::SetLineSpacing(uint8_t spacing) {
    WriteCommand(Command::SetCustomLineSpacing, spacing);
}

void PrinterDriver::PrintImage(const uint8_t* data, uint16_t n_h_dots, size_t data_len) {
    std::array<uint8_t, 5> header { 27, '*', 33, static_cast<uint8_t>(n_h_dots % 256), static_cast<uint8_t>(n_h_dots / 256) };
    m_uart.Write(header);
    m_uart.Write(reinterpret_cast<const char*>(data), data_len);
    m_uart.Write("\n");
}

void PrinterDriver::WriteCommand(PrinterDriver::Command cmd) {
    // Form command buffer
    std::array<uint8_t, 2> buf;
    buf[0] = 27;
    buf[1] = static_cast<uint8_t>(cmd);

    // Write buffer over UART
    m_uart.Write(buf);
}