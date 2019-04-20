#ifndef THERMAL_PRINTER_THERMAL_PRINTER_API_H
#define THERMAL_PRINTER_THERMAL_PRINTER_API_H

#include "http_server/embedded_file_handler.h"
#include "http_server/server.h"

#include "context.h"
#include "print_handler.h"
#include "power_handler.h"

// Embedded files
EMBEDDED_FILE(index_html, index_html)
EMBEDDED_FILE(index_css, index_css)
EMBEDDED_FILE(index_js, index_js)

// Embedded files routes
static constexpr const char IndexHtml[] = "/";
static constexpr const char IndexCss[] = "/index.css";
static constexpr const char IndexJs[] = "/index.js";

using APIServer = http::Server<Context,
    PrintHandler,
    PowerHandler,
    http::EmbeddedFileHandler<Context, IndexHtml, http::mimetype::HTML, index_html_start, index_html_end>,
    http::EmbeddedFileHandler<Context, IndexJs, http::mimetype::Js, index_js_start, index_js_end>,
    http::EmbeddedFileHandler<Context, IndexCss, http::mimetype::CSS, index_css_start, index_css_end>>;

#endif //THERMAL_PRINTER_THERMAL_PRINTER_API_H
