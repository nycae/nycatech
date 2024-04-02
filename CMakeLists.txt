cmake_minimum_required(VERSION 3.27)
project(
  NycaTech
    VERSION
      0.2
    LANGUAGES
      CXX
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(protos)
add_subdirectory(core)
add_subdirectory(shaders)

add_executable(
  NycaTech
    main.cc
)

target_link_libraries(
  NycaTech
    PUBLIC
      Networking
      Core
)

target_include_directories(
  NycaTech
    PUBLIC
      core
)