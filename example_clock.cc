#include <atomic>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <sys/time.h>
#include <thread>

#include "server.h"

DEFINE_int32(port, 9000, "TCP port to bind");

int main(int argc, char *argv[]) {
	google::InitGoogleLogging(argv[0]);
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	std::mutex mu;
	std::unordered_set<firesse::Stream*> streams;
	std::atomic<bool> running = true;

	std::thread clock([&streams, &mu, &running]() {
		while (running) {
			sleep(1);

			timeval tv;
			PCHECK(gettimeofday(&tv, nullptr) == 0);
			const uint64_t time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
			const auto time_str = std::to_string(time_ms);

			{
				std::lock_guard l(mu);
				for (auto* stream : streams) {
					stream->WriteEvent(time_str, 0, "time");
				}
			}
		}
	});

	firesse::Server server(FLAGS_port, [&streams, &mu](firesse::Stream* stream) {
		LOG(INFO) << "new stream: " << stream;

		std::lock_guard l(mu);
		streams.insert(stream);
		stream->OnClose([stream, &streams, &mu]() {
			LOG(INFO) << "stream closed: " << stream;

			std::lock_guard l(mu);
			streams.erase(stream);
		});
	});
	server.RegisterSignalHandlers();
	server.Serve();

	running = false;
	clock.join();

	gflags::ShutDownCommandLineFlags();
	google::ShutdownGoogleLogging();
}
