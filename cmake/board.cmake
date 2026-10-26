# Shared board selection for workshop demos.
# Expects WS_ROOT already set. Sets BOARD and BOARD_DIR.
if(NOT DEFINED BOARD)
    set(BOARD "ws28p" CACHE STRING "target board sku (ws28p|es3c28p)")
endif()
set_property(CACHE BOARD PROPERTY STRINGS ws28p es3c28p)

set(BOARD_DIR "${WS_ROOT}/ports/boards/${BOARD}")
if(NOT IS_DIRECTORY "${BOARD_DIR}")
    message(FATAL_ERROR
        "unknown BOARD='${BOARD}' — expected directory ${BOARD_DIR}\n"
        "Supported: ws28p (Waveshare Touch-LCD-2.8), es3c28p (ternion)")
endif()
message(STATUS "workshop BOARD=${BOARD} (${BOARD_DIR})")
