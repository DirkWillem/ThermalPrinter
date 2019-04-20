#ifndef THERMAL_PRINTER_POWER_HANDLER_H
#define THERMAL_PRINTER_POWER_HANDLER_H

#include "http_server/server.h"
#include "context.h"

/**
 * Handler for powering on the printer
 */
struct PowerHandler {
    static void Handle(Context* ctx, httpd_req_t* req);

    static constexpr const char* URI = "/power";
    static constexpr const httpd_method_t Method = HTTP_POST;
};


#endif //THERMAL_PRINTER_POWER_HANDLER_H
