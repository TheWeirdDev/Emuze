cmake_minimum_required(VERSION 3.16)
project(Emuze)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_executable(Emuze src/main.cpp src/emuze.cpp src/emuze.h src/components/EmulatorView.h
 src/components/DisassemblyView.h src/components/RegistersView.h
 src/emulators/chip8/VM.h src/emulators/chip8/VM.cpp
 src/components/LogView.h src/emulators/chip8/Disasm.cpp
 src/emulators/chip8/Disasm.h src/emulators/chip8/Chip8.cpp
 src/emulators/chip8/Chip8.h)
find_package(SFML COMPONENTS graphics audio window system REQUIRED)

target_link_libraries(Emuze sfml-graphics sfml-audio sfml-window sfml-system)
target_link_libraries(Emuze fmt)
target_link_libraries(Emuze spdlog)
target_link_libraries(Emuze pthread)

add_compile_definitions(SPDLOG_FMT_EXTERNAL)

