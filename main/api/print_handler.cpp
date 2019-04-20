#include "print_handler.h"

void PrintHandler::Handle(Context* ctx, httpd_req_t* req) {
    char buf[req->content_len];
    httpd_req_recv(req, buf, req->content_len);

    ctx->Print(reinterpret_cast<const uint8_t*>(buf), req->content_len);

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "", 0);
}