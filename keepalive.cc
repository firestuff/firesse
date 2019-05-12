#include <poll.h>
#include <sys/eventfd.h>

#include "keepalive.h"

namespace firesse {

KeepAlive::KeepAlive(const std::chrono::nanoseconds& max_stale, Index* index)
		: max_stale_(max_stale),
		  index_(index),
		  shutdown_(eventfd(0, 0)) {
	PCHECK(shutdown_ >= 0) << "eventfd()";
}

void KeepAlive::Start() {
	thread_ = std::thread([this]() {
		int timeout = 0;	
		constexpr auto num_fds = 1;
		pollfd fds[num_fds] = {
			{
				.fd = shutdown_,
				.events = POLLIN,
			},
		};
		while (poll(fds, num_fds, timeout) <= 0) {
			auto sleep = index_->WithStalest([](Stream* stream) {
				stream->WriteRaw(":\n");
			}, max_stale_);
			timeout = std::chrono::duration_cast<std::chrono::milliseconds>(sleep).count();
		}
	});
}

void KeepAlive::Stop() {
	CHECK(thread_.joinable());

	uint64_t shutdown = 1;
	PCHECK(write(shutdown_, &shutdown, sizeof(shutdown)) == sizeof(shutdown));
	thread_.join();
}

} // namespace firesse
