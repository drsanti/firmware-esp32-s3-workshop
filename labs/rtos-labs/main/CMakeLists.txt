# rtos-labs — one app; exercise sources live in ../ex*/.
# Workshop root is labs/rtos-labs/main → ../../..
set(WS_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(FW_INC "${WS_ROOT}/include/firmware-esp32-s3-dev")
set(FW_LIB "${WS_ROOT}/lib/librobotos_fw.a")
set(LAB_COMMON "${WS_ROOT}/labs/common")
set(RTOS_LABS_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
include("${WS_ROOT}/cmake/board.cmake")

if(NOT EXISTS "${FW_LIB}")
    message(FATAL_ERROR "missing ${FW_LIB} — install a robotos-fw release (see ${WS_ROOT}/lib/README.md)")
endif()
if(NOT IS_DIRECTORY "${FW_INC}")
    message(FATAL_ERROR "missing ${FW_INC} — unpack include/ from the same release")
endif()

file(GLOB BOARD_SRCS "${BOARD_DIR}/*.c")
file(GLOB LAB_COMMON_SRCS "${LAB_COMMON}/*.c")
file(GLOB RTOS_COMMON_SRCS "${RTOS_LABS_COMMON}/*.c")
file(GLOB MAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
# New exNN-* folders (or new .c files inside them) need `idf.py reconfigure`
# (IDF runs this file in script mode, so CONFIGURE_DEPENDS is not allowed here).
file(GLOB EX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../ex*/*.c")
file(GLOB EX_DIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/../ex*")
set(PORT_SRCS
    "${WS_ROOT}/ports/esp_idf.c"
    "${WS_ROOT}/ports/zpeer_null.c"
)

set(EX_INC_DIRS "")
foreach(_ex_dir ${EX_DIRS})
    if(IS_DIRECTORY "${_ex_dir}")
        list(APPEND EX_INC_DIRS "${_ex_dir}")
    endif()
endforeach()

idf_component_register(
    SRCS ${MAIN_SRCS} ${EX_SRCS} ${RTOS_COMMON_SRCS} ${LAB_COMMON_SRCS}
         ${PORT_SRCS} ${BOARD_SRCS}
    INCLUDE_DIRS
        "."
        ${EX_INC_DIRS}
        "${RTOS_LABS_COMMON}"
        "${LAB_COMMON}"
        "${FW_INC}"
        "${FW_INC}/middleware"
        "${FW_INC}/serial_proto"
        "${FW_INC}/robotos_proto"
        "${FW_INC}/vendor/nanopb"
        "${FW_INC}/ui"
        "${WS_ROOT}/ui"
        "${WS_ROOT}/ports"
        "${BOARD_DIR}"
    REQUIRES esp_driver_gpio esp_driver_rmt esp_timer nvs_flash
             esp_driver_uart esp_driver_usb_serial_jtag
             esp_lcd esp_driver_spi esp_driver_ledc
             esp_driver_i2c esp_driver_i2s esp_adc
)

add_library(robotos_fw STATIC IMPORTED GLOBAL)
set_target_properties(robotos_fw PROPERTIES IMPORTED_LOCATION "${FW_LIB}")
target_link_libraries(${COMPONENT_LIB} INTERFACE robotos_fw m)

target_compile_definitions(${COMPONENT_LIB} PRIVATE RSP_LINK_USB_JTAG)
