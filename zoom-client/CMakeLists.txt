find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(zoom-client ZoomClient.cpp)

target_include_directories(zoom-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zoom-client PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)