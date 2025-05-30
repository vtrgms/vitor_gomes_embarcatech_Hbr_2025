# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(weather_agent C CXX ASM)
pico_sdk_init()

# Cria o executável PRIMEIRO
add_executable(weather_agent 
    src/main.c
)

# Depois gera o header PIO
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/inc)
pico_generate_pio_header(weather_agent ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.pio
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Restante das configurações...
pico_enable_stdio_uart(weather_agent 0)
pico_enable_stdio_usb(weather_agent 1)

target_link_libraries(weather_agent
    pico_stdlib
    hardware_pio
    hardware_dma
    hardware_i2c
    pico_cyw43_arch_lwip_threadsafe_background
)

target_include_directories(weather_agent PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Corrige a dependência do pioasm
add_custom_target(pio_datasheet DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.py
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.pio
    COMMAND pioasm -o python ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.py
    VERBATIM
)
add_dependencies(weather_agent pio_datasheet)

pico_add_extra_outputs(weather_agent)