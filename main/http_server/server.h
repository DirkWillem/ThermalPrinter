#ifndef THERMAL_PRINTER_SERVER_H
#define THERMAL_PRINTER_SERVER_H

extern "C" {
#include <esp_log.h>
#include <esp_http_server.h>
};

namespace http {
    template<typename Ctx, typename... Handlers>
/**
 * Represents a HTTP server
 * @tparam Ctx Type of the context that can be accessed by any request handler
 * @tparam Handlers List of request handlers
 */
    class Server {
    public:
        /**
         * Constructor
         */
        Server()
                : m_ctx{}, m_handle{nullptr} {
            // Get default config
            httpd_config_t config = HTTPD_DEFAULT_CONFIG();

            // Start HTTPd server
            auto start_err = httpd_start(&m_handle, &config);
            if (start_err == ESP_OK) {
                ESP_LOGI(Tag, "Starting web server");
                auto i = {RegisterHandler<Handlers>()...};
                return;
            }

            ESP_LOGI(Tag, "Could not start web server");
        }

    private:
        template<typename Handler>
        /**
         * Registers the handler passed through the template parameter
         * @tparam Handler Handler to register
         * @return Dummy value, for use with the comma operator
         */
        uint8_t RegisterHandler() {
            httpd_uri_t uri{};
            uri.handler = [](httpd_req_t* req) -> esp_err_t {
                Handler::Handle(reinterpret_cast<Ctx*>(req->user_ctx), req);
                return ESP_OK;
            };
            uri.method = Handler::Method;
            uri.uri = Handler::URI;
            uri.user_ctx = &m_ctx;

            httpd_register_uri_handler(m_handle, &uri);

            return 0;
        }


        static constexpr const char* Tag = "HTTPServer";

        Ctx m_ctx;
        httpd_handle_t m_handle;
    };
}


#endif //THERMAL_PRINTER_SERVER_H
