//
// Created by rplaz on 2023-12-12.
//

#include <grpc++/grpc++.h>

#include "benchmark.grpc.pb.h"
#include "types.h"

using namespace grpc;
using namespace NycaTech;

int main()
{
  const auto channel = CreateChannel("localhost:8080", InsecureChannelCredentials());
  auto server = Benchmark::NewStub(channel);
  Thread unary_caller([&server]() {
    for (;;) {
      ClientContext context;
      BenchmarkStreamRequest request;
      BenchmarkStreamResponse response;
      String str_response;

      request.mutable_player()->set_id(1);
      request.mutable_target()->set_id(2);
      request.mutable_command()->add_ids(1);

      response.SerializeToString(&str_response);
      std::cout << str_response << std::endl;
      auto frame_timestamp = steady_clock::now();
      if (!server->Call(&context, request, &response).ok()) {
        std::cerr << "error in call" << std::endl;
        break;
      }
      response.SerializeToString(&str_response);
      std::cout << str_response << std::endl;
      sleep_until(frame_timestamp + milliseconds(16));
    }
  });

  Thread streamer([&server]() {
    ClientContext context;
    auto stream = server->Stream(&context);
    Thread writter([&stream] () {
      String                  str_request;
      BenchmarkStreamRequest  request;
      request.mutable_player()->set_id(1);
      request.mutable_target()->set_id(2);
      request.mutable_command()->add_ids(1);

      while (stream->Write(request)) {
        auto frame_timestamp = steady_clock::now();
        std::cout << "request sent: " << request.SerializeAsString() << std::endl;
        sleep_until(frame_timestamp + milliseconds(50));
      }
    });
    Thread reader([&stream]() {
      BenchmarkStreamResponse response;
      while (stream->Read(&response)) {
        auto frame_timestamp = steady_clock::now();
        std::cout << "response received" << response.SerializeAsString() << std::endl;
        sleep_until(frame_timestamp + milliseconds(50));
      }
    });

    sleep_for(seconds(600));
    writter.join();
    reader.join();
    stream->WritesDone();
  });

  sleep_for(seconds(800));
  unary_caller.detach();
  streamer.detach();
  return EXIT_SUCCESS;
}