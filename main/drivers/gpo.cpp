#include "gpo.h"

GPO::GPO(gpio_num_t pin_num)
    : m_pin{pin_num} {
    gpio_pad_select_gpio(pin_num);
    gpio_set_direction(pin_num, GPIO_MODE_OUTPUT);
}

GPO& GPO::operator=(bool val) {
    gpio_set_level(m_pin, val ? 1 : 0);
    return *this;
}