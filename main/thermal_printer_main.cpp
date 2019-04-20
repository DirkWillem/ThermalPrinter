#include "wifi/station.h"
#include "wifi/access_point.h"

#include "api/thermal_printer_api.h"
#include "drivers/timer.h"

#include "drivers/gpo.h"
#include "constants.h"

#include <esp_event.h>
#include <nvs_flash.h>

static constexpr const char A[] = "Hi";


extern "C" void app_main() {
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    tcpip_adapter_init();

    // Set up access point & start HTTP server
    static wifi::AccessPoint ap{"Printer 4.0", "SteinbuchP100D", 4};
    static APIServer api_server{};
}
