#ifndef THERMAL_PRINTER_ACCESS_POINT_H
#define THERMAL_PRINTER_ACCESS_POINT_H

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
 * Creates a WiFi access point
 */
    class AccessPoint {
    public:
        /**
         * Constructor
         * @param ssid Network SSID
         * @param password Network password
         * @param max_conns Maximum connections
         */
        AccessPoint(const char* ssid, const char* password, int max_conns);

    private:
        /**
         * WiFi event handler
         */
        static void OnWifiEvent(
                void* arg,
                esp_event_base_t event_base,
                int32_t event_id,
                void* event_data);

        EventGroupHandle_t m_wifi_event_group;
        static constexpr const char* Tag = "AccessPoint";
        static constexpr EventBits_t WifiConnectedBit = BIT0;
    };
}

#endif //THERMAL_PRINTER_ACCESS_POINT_H
