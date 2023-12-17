//
// Created by rplaz on 2023-12-12.
//

#include "server.h"

#include "types.h"

#include <grpc++/grpc++.h>

namespace NycaTech {

Status Server::Stream(ServerContext* context, ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream)
{
  Thread reader(attend_stream_reader, stream);
  Thread writer(attend_stream_writter, stream);

  reader.join();
  writer.join();

  return Status::OK;
}

Status Server::Call(ServerContext* context, const BenchmarkStreamRequest* request, BenchmarkStreamResponse* response)
{
  if (request->command().ids().at(0) == 1 ) {
    response->mutable_status()->set_health(50);
  }
  response->mutable_status()->mutable_position()->set_pos_x(10);
  response->mutable_status()->mutable_position()->set_pos_y(10);
  response->mutable_status()->mutable_position()->set_pos_z(10);
  return Status::OK;
}

void Server::attend_stream_reader(ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream)
{
  BenchmarkStreamResponse response;
  response.mutable_status()->set_health(10);
  response.mutable_status()->set_mana(10);
  response.mutable_loc()->set_pos_x(1);
  response.mutable_loc()->set_pos_z(1);
  while (stream->Write(response)) {
    auto frame_timestamp = steady_clock::now();
    sleep_until(frame_timestamp + milliseconds(16));
  }
}

void Server::attend_stream_writter(ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream)
{
  BenchmarkStreamRequest request;
  while (stream->Read(&request)) {
    std::cout << "message received: " << request.SerializeAsString() << std::endl;
  }
}

}  // namespace NycaTech

int main()
{
  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:8080", grpc::InsecureServerCredentials());
  builder.RegisterService(new NycaTech::Server());
  auto server = builder.BuildAndStart();
  server->Wait();
  return EXIT_SUCCESS;
}