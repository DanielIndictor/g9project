cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_VERBOSE_MAKEFILE 1)

# Pull in SDK (must be before project)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

project(g9project C CXX ASM)

# Initialize the SDK
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
pico_sdk_init()


file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(g9project
    ${SRC_FILES}
)

pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

target_include_directories(g9project PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
)

# # REQUIRED ON DEBIAN TESTING
# target_include_directories(g9project PUBLIC
#     "/usr/include/newlib/c++/8.3.1"
#     "/usr/include/newlib/c++/8.3.1/arm-none-eabi"
# )

target_compile_options(g9project PRIVATE
    -Wall
    -Wextra
    #-Wpedantic                 # Disabled in sdk/test/kitchen-sink
    -Wshadow
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wsign-conversion
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wfloat-equal
    -Wmissing-format-attribute
    -Wdouble-promotion
    -Wconversion
    -Wuninitialized
    -Wformat=2
    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
    $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor; -Woverloaded-virtual; -Wuseless-cast>

    -Wno-uninitialized          # Temp issue when compiling with GCC11

    -Werror=return-type
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-unused-variable
    -Wno-unused-local-typedefs

    -fdata-sections
    -ffunction-sections
    -fuse-linker-plugin
#    -fanalyzer
)

target_link_options(g9project PRIVATE
    -flto
    -Wl,--gc-sections
)

target_link_libraries(g9project
    pico_stdlib
    hardware_pwm
)

pico_enable_stdio_uart(g9project 1)
pico_enable_stdio_usb(g9project 1)

pico_add_extra_outputs(g9project)


add_custom_command(TARGET g9project POST_BUILD
    COMMAND arm-none-eabi-size --format=SysV "${PROJECT_NAME}.elf"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Getting ELF section info..."
)

add_custom_command(TARGET g9project POST_BUILD
    COMMAND picotool info -a "${PROJECT_NAME}.elf"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Getting build info..."
)

add_custom_target(flash
  COMMAND picotool load -vx ${PROJECT_NAME}.uf2
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS g9project
)

add_custom_target(disassemble
  COMMAND arm-none-eabi-objdump -h -S --endian=little --line-numbers --demangle --wide "${PROJECT_NAME}.elf" > "${PROJECT_NAME}.disasm"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS g9project
)
