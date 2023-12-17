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
  lib/vector.h
  lib/pair.h
)

target_link_libraries(
  Client
    Networking
)

set(USE_VULKAN ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_library(
  Core
    STATIC
      types.h
      world.cc
      world.h
      entity.cc
      entity.h
      component.cc
      component.h
      system.cc
      system.h
      renderer/obj_model.cc
      renderer/obj_model.h
      renderer/vulkan_renderer.cc
      renderer/vulkan_renderer.h
  renderer/shader.cc
      renderer/shader.h
  renderer/render_pipeline.cc
  renderer/render_pipeline.h
  renderer/physical_device.cc
  renderer/physical_device.h
  lib/assert.h
  renderer/vulkan_instance.cc
  renderer/vulkan_instance.h
)

target_link_libraries(
  Core
    PUBLIC
      Vulkan::Vulkan
      ${Protobuf_LIBRARIES}
      SDL2::SDL2
      GLEW::GLEW
      opengl32
    INTERFACE
      tinyobjloader::tinyobjloader
)

target_include_directories(
  Core
    PUBLIC
      ${GLEW_LIBRARIES}
      ${OPENGL_LIBRARIES}
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