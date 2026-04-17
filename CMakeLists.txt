cmake_minimum_required(VERSION 3.23)

project(MiniRDB)

# 1. C++ 표준 설정 (C++17 사용)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 헤더 파일 경로 지정 (코드에서 #include "App.h"가 가능하게 함)
include_directories(include)

# 3. 소스 파일들 등록 (src 폴더 안의 모든 .cpp 파일)
file(GLOB SOURCES "src/*.cpp")
list(APPEND SOURCES "main.cpp")

# 4. 실행 파일 이름 설정
add_executable(MiniRDB_Server ${SOURCES})
target_include_directories(MiniRDB_Server PRIVATE include)
# 5. 윈도우라면 소켓 라이브러리(Winsock) 연결
if(WIN32)
    target_link_libraries(MiniRDB_Server ws2_32)
endif()