# Thin lab app — links librobotos_fw.a from the workshop root.
set(WS_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(FW_INC "${WS_ROOT}/include/firmware-esp32-s3-dev")
set(FW_LIB "${WS_ROOT}/lib/librobotos_fw.a")
set(BOARD "ws28p" CACHE STRING "target board sku")
set(BOARD_DIR "${WS_ROOT}/ports/boards/${BOARD}")

if(NOT EXISTS "${FW_LIB}")
    message(FATAL_ERROR "missing ${FW_LIB} — install a robotos-fw release (see lib/README.md)")
endif()
if(NOT IS_DIRECTORY "${FW_INC}")
    message(FATAL_ERROR "missing ${FW_INC} — unpack include/ from the same release")
endif()

file(GLOB BOARD_SRCS "${BOARD_DIR}/*.c")
set(PORT_SRCS
    "${WS_ROOT}/ports/esp_idf.c"
    "${WS_ROOT}/ports/zpeer_null.c"
)

idf_component_register(
    SRCS "main.c" ${PORT_SRCS} ${BOARD_SRCS}
    INCLUDE_DIRS
        "."
        "${FW_INC}"
        "${FW_INC}/middleware"
        "${FW_INC}/serial_proto"
        "${FW_INC}/robotos_proto"
        "${FW_INC}/vendor/nanopb"
        "${WS_ROOT}/ports"
        "${BOARD_DIR}"
    REQUIRES esp_driver_gpio esp_driver_rmt esp_timer nvs_flash
             esp_driver_uart esp_driver_usb_serial_jtag
)

target_link_libraries(${COMPONENT_LIB} INTERFACE "${FW_LIB}" m)
