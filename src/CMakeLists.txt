include_directories("../libs")
set(SERVER_SRC simple_http_server.h simple_http_server.cpp)
add_library(server ${SERVER_SRC})
target_link_libraries(server parser types)
add_executable(simple_server simple_server.cpp)
target_link_libraries(simple_server types server)
