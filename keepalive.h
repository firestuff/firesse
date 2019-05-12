#pragma once

#include <thread>

#include "index.h"

namespace firesse {

class KeepAlive {
  public:
	KeepAlive(const std::chrono::nanoseconds& max_stale, Index* index);
	void Start();
	void Stop();

  private:
	const std::chrono::nanoseconds max_stale_;
	Index* index_;
	int shutdown_;

	std::thread thread_;
};

} // namespace firesse
