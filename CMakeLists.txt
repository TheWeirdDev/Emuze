cmake_minimum_required(VERSION 3.16)
project(Emuze)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options("-Wall")
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

add_subdirectory("libs/nativefiledialog")

add_executable(Emuze src/main.cpp src/emuze.cpp src/emuze.h src/components/EmulatorView.h
        src/emulators/chip8/Disasm.cpp
        src/emulators/chip8/Disasm.h src/emulators/chip8/Chip8.cpp
        src/emulators/chip8/Chip8.h src/menu.cpp src/menu.h)
find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(spdlog)
find_package(fmt)
target_link_libraries(Emuze sfml-graphics sfml-audio sfml-window sfml-system)
target_link_libraries(Emuze fmt)
target_link_libraries(Emuze spdlog)
target_link_libraries(Emuze pthread)
target_link_libraries(Emuze nativefiledialog)

add_compile_definitions(SPDLOG_FMT_EXTERNAL)

