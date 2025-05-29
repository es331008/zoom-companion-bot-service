find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(azure AzureSpeechManager.cpp)

target_include_directories(azure PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(azure PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
)