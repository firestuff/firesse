#include <poll.h>
#include <sys/eventfd.h>

#include "keepalive.h"

namespace firesse {

KeepAlive::KeepAlive(const std::chrono::nanoseconds& max_stale, Index* index)
    : max_stale_(max_stale), index_(index), shutdown_fd_(eventfd(0, 0)) {
  PCHECK(shutdown_fd_ >= 0) << "eventfd()";
}

KeepAlive::~KeepAlive() { PCHECK(close(shutdown_fd_) == 0); }

void KeepAlive::Start() {
  thread_ = std::thread([this]() {
    int timeout = 0;
    constexpr auto num_fds = 1;
    pollfd fds[num_fds] = {
        {
            .fd = shutdown_fd_,
            .events = POLLIN,
        },
    };
    while (running_ && (timeout == 0 || poll(fds, num_fds, timeout) <= 0)) {
      auto sleep = index_->WithStalest(
          [](Stream* stream) { stream->WriteRaw(":\n"); }, max_stale_);
      timeout =
          std::chrono::duration_cast<std::chrono::milliseconds>(sleep).count();
    }
  });
}

void KeepAlive::Stop() {
  CHECK(thread_.joinable());

  running_ = false;
  uint64_t shutdown = 1;
  PCHECK(write(shutdown_fd_, &shutdown, sizeof(shutdown)) == sizeof(shutdown));
  thread_.join();
}

}  // namespace firesse
