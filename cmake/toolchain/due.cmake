####
# due.cmake:
#
# Arduino DUE support.
####
set(CMAKE_DEBUG_OUTPUT ON)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm")
set(CMAKE_CROSSCOMPILING 1)
set(FPRIME_PLATFORM "ArduinoFw")
set(FPRIME_USE_BAREMETAL_SCHEDULER ON)
set(ARDUINO_BUILD_PROPERTIES)
# Change FQBN to Arduino DUE
set(ARDUINO_FQBN "arduino:sam:arduino_due_x")
# Update board-specific defines
add_compile_options(-D_BOARD_DUE -DUSE_BASIC_TIMER)
include("${CMAKE_CURRENT_LIST_DIR}/support/arduino-support.cmake")
