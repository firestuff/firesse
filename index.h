#pragma once

#include "stream.h"

namespace firesse {

// Track live streams
class Index {
  public:
	void Add(Stream* stream);
	void Remove(Stream* stream);

  private:
	std::mutex mu_;
	Stream* freshest_ = nullptr;
	Stream* stalest_ = nullptr;
};

} // namespace firesse
