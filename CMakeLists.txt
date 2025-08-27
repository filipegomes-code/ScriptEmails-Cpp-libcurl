cmake_minimum_required(VERSION 3.27)
project(script_mails)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED) # diz ao Cmake para procurar libcurl no sistema

add_executable(script_mails
        Script_emails.cpp
        header.h
)

target_link_libraries(script_mails CURL::libcurl) # liga a minha aplicação à biblioteca libcurl
