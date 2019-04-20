#include "station.h"

#include <cstring>

wifi::Station::Station(const char* ssid, const char* password, int max_retries)
    : m_max_retries{max_retries}, m_retry_count{0}, m_wifi_event_group{} {
    // Set up event group & adapter
    m_wifi_event_group = xEventGroupCreate();

    // Initialize WiFi
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    // Set up event handler
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Station::OnWifiEvent, this));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Station::OnWifiEvent, this));

    // Configure WiFi
    wifi_config_t cfg{};
    strcpy(reinterpret_cast<char*>(cfg.sta.ssid), ssid);
    strcpy(reinterpret_cast<char*>(cfg.sta.password), password);

    // Set WiFi parameters
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(Tag, "wifi init finished");
}

void wifi::Station::OnWifiEvent(
        void* arg,
        esp_event_base_t event_base,
        int32_t event_id,
        void* event_data) {
    auto self = reinterpret_cast<Station*>(arg);

    if (event_base == WIFI_EVENT) {
        switch(event_id)
            case WIFI_EVENT_STA_START: {
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                // Check whether we still want a retry
                if (self->m_retry_count < self->m_max_retries) {
                    esp_wifi_connect();
                    self->m_retry_count++;
                    xEventGroupClearBits(self->m_wifi_event_group, Station::WifiConnectedBit);
                    ESP_LOGI(Tag, "Connect to AP failed, retry %d/%d", self->m_retry_count, self->m_max_retries);
                } else {
                    ESP_LOGI(Tag, "Connect to AP failed, not retrying");
                }
                break;
            default: break;
        }
    } else if (event_base == IP_EVENT) {
        switch(event_id) {
            case IP_EVENT_STA_GOT_IP: {
                auto event = reinterpret_cast<ip_event_got_ip_t*>(event_data);
                ESP_LOGI(Tag, "Got IP: %s", ip4addr_ntoa(&event->ip_info.ip));
                self->m_retry_count = 0;
                xEventGroupSetBits(self->m_wifi_event_group, Station::WifiConnectedBit);
                break;
            }
            default: break;
        }
    }
}