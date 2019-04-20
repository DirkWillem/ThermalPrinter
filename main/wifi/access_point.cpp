#include "access_point.h"

extern "C" {
#include <esp_log.h>
}

#include <cstring>

wifi::AccessPoint::AccessPoint(const char* ssid, const char* password, int max_conns)
        : m_wifi_event_group{} {

    // Set up event group & adapter
    m_wifi_event_group = xEventGroupCreate();

    // Initialize WiFi
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    // Set up event handler
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &AccessPoint::OnWifiEvent, this));

    // Configure WiFi
    wifi_config_t cfg{};
    strcpy(reinterpret_cast<char*>(cfg.ap.ssid), ssid);
    strcpy(reinterpret_cast<char*>(cfg.ap.password), password);
    cfg.ap.ssid_len = strlen(ssid);
    cfg.ap.max_connection = max_conns;
    cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;

    // Set WiFi parameters
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi::AccessPoint::OnWifiEvent(
        void* arg,
        esp_event_base_t event_base,
        int32_t event_id,
        void* event_data) {
    auto self = reinterpret_cast<AccessPoint*>(arg);

    switch(event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP: {
            const auto got_ip = reinterpret_cast<system_event_sta_got_ip_t*>(event_data);
            ESP_LOGI(Tag, "[STA_GOT_IP] got ip:%s",
                     ip4addr_ntoa(&got_ip->ip_info.ip));
            xEventGroupSetBits(self->m_wifi_event_group, BIT0);
            break;
        }
        case SYSTEM_EVENT_AP_STACONNECTED: {
            const auto sta_connected = reinterpret_cast<system_event_ap_staconnected_t*>(event_data);
            ESP_LOGI(Tag, "[AP_STACONNECTED] station:" MACSTR" join, AID=%d",
                     MAC2STR(sta_connected->mac),
                     sta_connected->aid);
            break;
        }
        case SYSTEM_EVENT_AP_STADISCONNECTED: {
            const auto sta_disconnected = reinterpret_cast<system_event_ap_stadisconnected_t*>(event_data);
            ESP_LOGI(Tag, "station:" MACSTR"leave, AID=%d",
                     MAC2STR(sta_disconnected->mac),
                     sta_disconnected->aid);
            break;
        }
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(self->m_wifi_event_group, BIT0);
            break;
        default:
            break;
    }
}