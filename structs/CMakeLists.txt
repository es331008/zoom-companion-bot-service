find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(structs ChatMessage.cpp MeetingInfo.cpp)

target_include_directories(structs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(structs PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)