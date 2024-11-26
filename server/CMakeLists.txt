cmake_minimum_required(VERSION 3.22)
set(project_name ender_label)
project(${project_name} VERSION 0.1.0)
file(GLOB LIBS_PATH_MAIN ./src/*.hpp)

set(CMAKE_CXX_STANDARD 23)
add_library(${project_name}-lib ${LIBS_PATH_MAIN})

target_include_directories(${project_name}-lib PUBLIC src)
## link libs
find_package(oatpp 1.3.0 REQUIRED)
find_package(oatpp-postgresql 1.3.0 REQUIRED)
find_package(oatpp-swagger 1.3.0 REQUIRED)
find_package(oatpp-curl 1.3.0 REQUIRED)
find_package(oatpp-websocket 1.3.0 REQUIRED)

find_package(Boost 1.70.0 COMPONENTS ALL)
find_package(OpenSSL REQUIRED)

target_link_libraries(${project_name}-lib
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-test
        PUBLIC oatpp::oatpp-curl
        PUBLIC oatpp::oatpp-postgresql
        PUBLIC oatpp::oatpp-swagger
        PUBLIC oatpp::oatpp-websocket
        PUBLIC OpenSSL::SSL
        PUBLIC ${Boost_LIBRARIES}
        )

# 读取版本号文件的内容
file(READ version.txt VERSION)

# 去除换行符和空格
string(STRIP ${VERSION} VERSION)

add_definitions(
        # Path to oatpp-swagger res files.
        -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}../bin/oatpp-swagger/res"

        -DVERSION="${VERSION}"

)

add_executable(${project_name}-exe
        src/App.cpp)
target_link_libraries(${project_name}-exe ${project_name}-lib)
add_dependencies(${project_name}-exe ${project_name}-lib)