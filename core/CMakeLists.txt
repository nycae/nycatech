add_executable(
  Server
    server.cc
)

target_link_libraries(
  Server
    Networking
)


add_executable(
  Client
    client.cc
)

target_link_libraries(
  Client
    Networking
)

set(USE_VULKAN ON)

find_package(tinyobjloader CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_library(
  Core
    STATIC
      world.cc
      entity.cc
      component.cc
      system.cc
      renderer/obj_model.cc
      renderer/vulkan_renderer.cc
      renderer/shader.cc
)

target_link_libraries(
  Core
    PUBLIC
      Vulkan::Vulkan
      ${Protobuf_LIBRARIES}
      SDL2::SDL2
    INTERFACE
      tinyobjloader::tinyobjloader
)

target_include_directories(
  Core
    PUBLIC
      ${Protobuf_INCLUDE_DIRS}
      ${SDL2_INCLUDE_DIRS}
      Vulkan::Vulkan
      .
)

target_compile_definitions(
  Core
    PRIVATE
      $<$<CONFIG:Debug>:DEBUG>
)