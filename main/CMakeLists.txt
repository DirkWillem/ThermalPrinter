set(COMPONENT_SRCS "thermal_printer_main.cpp api/context.cpp api/context.h api/print_handler.cpp api/print_handler.h api/thermal_printer_api.h api/power_handler.cpp api/power_handler.h http_server/embedded_file_handler.h http_server/server.h drivers/uart.h drivers/uart.cpp drivers/gpo.cpp drivers/gpo.h drivers/timer.h drivers/timer.cpp printer/printer_driver.h printer/printer_driver.cpp wifi/access_point.h wifi/access_point.cpp wifi/station.h wifi/station.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_EMBED_TXTFILES web_dist/index.html web_dist/index.js web_dist/index.css)


register_component()