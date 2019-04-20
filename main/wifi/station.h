#ifndef THERMAL_PRINTER_STATION_H
#define THERMAL_PRINTER_STATION_H

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>


#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>

#include <lwip/err.h>
#include <lwip/sys.h>
}

namespace wifi {
    /**
 * Sets up the ESP32 as a WiFi station using WPA2
 */
    class Station {
    public:
        /**
         * Constructor
         * @param ssid SSID of the network to connect to
         * @param password Password of the network to connect to
         * @param max_retries Maximum number of retries allowed for connecting to the network
         */
        Station(const char* ssid, const char* password, int max_retries = 5);

    private:
        /**
         * WiFi event handler
         */
        static void OnWifiEvent(
                void* arg,
                esp_event_base_t event_base,
                int32_t event_id,
                void* event_data);

        // Retries
        const int m_max_retries;
        int m_retry_count;

        EventGroupHandle_t m_wifi_event_group;
        static constexpr const char* Tag = "Station";
        static constexpr EventBits_t WifiConnectedBit = BIT0;
    };
}


#endif //THERMAL_PRINTER_STATION_H
