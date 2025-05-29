find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(bot BotInstance.cpp BotManager.cpp)

target_include_directories(bot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bot PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)