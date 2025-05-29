cmake_minimum_required(VERSION 3.10)
project(ZoomCompanionBotService)

set(CMAKE_CXX_STANDARD 14)

# Vcpkg Toolchain
set(CMAKE_TOOLCHAIN_FILE "D:/git/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Azure Speech SDK
set(SPEECH_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/Microsoft.CognitiveServices.Speech.1.43.0/build/native")

# External headers (non-vcpkg)
include_directories(
    ${SPEECH_SDK_ROOT}/include/c_api
    ${SPEECH_SDK_ROOT}/include/cxx_api
)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)

# Zoom SDK headers/libs
include_directories(${CMAKE_SOURCE_DIR}/external/zoom-sdk/h)
link_directories(${CMAKE_SOURCE_DIR}/external/zoom-sdk/lib)
link_directories(${SPEECH_SDK_ROOT}/x64/Release)

# Submodules
add_subdirectory(azure)
add_subdirectory(bot)
add_subdirectory(structs)
add_subdirectory(util)
add_subdirectory(virtual-audio)
add_subdirectory(zoom-client)

# Main executable
add_executable(ZoomCompanionBotService Main.cpp)

# Link all required libraries
target_link_libraries(ZoomCompanionBotService PRIVATE
    azure
    bot
    structs
    util
    virtual-audio
    zoom-client
    sdk  # Assuming this is your Zoom SDK .lib
    Microsoft.CognitiveServices.Speech.core
    nlohmann_json::nlohmann_json
    httplib::httplib
)

# Copy Zoom SDK DLLs after build
add_custom_command(TARGET ZoomCompanionBotService POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/external/zoom-sdk/bin
        $<TARGET_FILE_DIR:ZoomCompanionBotService>
)
