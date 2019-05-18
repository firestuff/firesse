#pragma once

#include <atomic>
#include <thread>

#include "index.h"

namespace firesse {

class KeepAlive {
 public:
  KeepAlive(const std::chrono::nanoseconds& max_stale, Index* index);
  ~KeepAlive();

  void Start();
  void Stop();

 private:
  const std::chrono::nanoseconds max_stale_;
  Index* index_;

  // Two shutdown mechanisms, one for if we're in a tight no-syscall loop,
  // and one for if we're sleeping in poll()
  int shutdown_fd_;
  std::atomic<bool> running_ = true;

  std::thread thread_;
};

}  // namespace firesse
