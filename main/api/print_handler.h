#ifndef THERMAL_PRINTER_PRINT_HANDLER_H
#define THERMAL_PRINTER_PRINT_HANDLER_H

#include "http_server/server.h"
#include "context.h"

/**
 * Handler for a print through the HTTP API
 */
struct PrintHandler {
    static void Handle(Context* ctx, httpd_req_t* req);

    static constexpr const char* URI = "/print";
    static constexpr const httpd_method_t Method = HTTP_POST;
};

#endif //THERMAL_PRINTER_PRINT_HANDLER_H
