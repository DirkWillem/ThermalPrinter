#ifndef THERMAL_PRINTER_EMBEDDED_FILE_HANDLER_H
#define THERMAL_PRINTER_EMBEDDED_FILE_HANDLER_H

#include <cstdint>

#include <esp_http_server.h>

#define EMBEDDED_FILE(var_name, f_name) extern const uint8_t var_name##_start[] asm("_binary_"#f_name"_start");\
extern const uint8_t var_name##_end[] asm("_binary_"#f_name"_end");

namespace http {
    /**
     * Namespace containing common MIME types
     */
    namespace mimetype {
        static constexpr const char HTML[] = "text/html";
        static constexpr const char Js[] = "application/javascript";
        static constexpr const char CSS[] = "text/css";
        static constexpr const char JSON[] = "application/json";
        static constexpr const char Plain[] = "text/plain";
    }

    template<typename Ctx, const char* Uri, const char* MimeType, const uint8_t* DataStart, const uint8_t* DataEnd>
    /**
     * http::Server handler for serving static files
     * @tparam Ctx Context type
     * @tparam Uri URI of the type to handle
     * @tparam MimeType Mime type of the file to serve
     * @tparam DataStart Embedded file data start pointer
     * @tparam DataEnd Embedded file data end pointer
     */
    struct EmbeddedFileHandler {
        static constexpr const char* URI = Uri;
        static constexpr httpd_method_t Method = HTTP_GET;

        static void Handle(Ctx*, httpd_req_t* req) {
            httpd_resp_set_type(req, MimeType);
            httpd_resp_send(req, reinterpret_cast<const char*>(DataStart), DataEnd - DataStart - 1);
        }
    };
}

#endif //THERMAL_PRINTER_EMBEDDED_FILE_HANDLER_H
