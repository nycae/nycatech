find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(
  Networking STATIC
  benchmark.proto
)

target_link_libraries(
  Networking
    PUBLIC
      ${Protobuf_LIBS}
      gRPC::grpc
      gRPC::grpc++
)
target_include_directories(
  Networking
    PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}
      ${ProtobufIncludePath}
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(
  TARGET
    Networking
  PROTOS
    benchmark.proto
)
protobuf_generate(
  TARGET
    Networking
  LANGUAGE
    grpc
  PROTOS
    benchmark.proto
  GENERATE_EXTENSIONS
    .grpc.pb.h
    .grpc.pb.cc
  PLUGIN
    "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)
