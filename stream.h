#pragma once

#include <memory>

#include "firecgi/request.h"

namespace firesse {

class Stream {
  public:
	Stream(firecgi::Request* request);

	[[nodiscard]] bool WriteEvent(const std::string& data, uint64_t id=0, const std::string& type="");
	bool End();

  private:
	firecgi::Request* request_;
};

} // namespace firesse
