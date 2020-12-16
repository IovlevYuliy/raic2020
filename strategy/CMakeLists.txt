cmake_minimum_required(VERSION 3.5)
project(aicup2020)

# OS and compiler checks.
if(WIN32)
    add_definitions(-DWIN32)
    SET(PROJECT_LIBS Ws2_32.lib)
endif()

set(CMAKE_CXX_STANDARD 17)

file(GLOB HEADERS "*.hpp" "model/*.hpp" "stream/*.hpp" "include/*.hpp")
file(GLOB SRC "*.cpp" "model/*.cpp" "stream/*.cpp" "source/*.cpp")

SET_SOURCE_FILES_PROPERTIES(${HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
add_executable(aicup2020 ${HEADERS} ${SRC})

include_directories(aicup2020 PUBLIC include model)
TARGET_LINK_LIBRARIES(aicup2020 ${PROJECT_LIBS})