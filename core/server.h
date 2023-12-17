//
// Created by rplaz on 2023-12-12.
//

#ifndef SERVER_H
#define SERVER_H

#include "benchmark.grpc.pb.h"
#include "benchmark.pb.h"

namespace NycaTech {

using namespace grpc;

class Server final : public Benchmark::Service {
public:
  Status Stream(ServerContext* context, ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream) override;
  Status Call(ServerContext* context, const BenchmarkStreamRequest* request, BenchmarkStreamResponse* response) override;

private:
  static void attend_stream_reader(ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream);
  static void attend_stream_writter(ServerReaderWriter<BenchmarkStreamResponse, BenchmarkStreamRequest>* stream);

};

}  // namespace NycaTech

#endif  // SERVER_H
