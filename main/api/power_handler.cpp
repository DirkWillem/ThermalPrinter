#include "power_handler.h"

void PowerHandler::Handle(Context* ctx, httpd_req_t* req) {
    ctx->PowerPrinter();

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "", 0);
}