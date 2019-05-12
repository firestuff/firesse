#pragma once

#include <memory>

#include "firecgi/request.h"

namespace firesse {

class Stream {
  public:
	Stream(firecgi::Request* request);

	void OnClose(const std::function<void()>& callback);

	bool WriteEvent(const std::string& data, uint64_t id=0, const std::string& type="");
	bool End();

  private:
	firecgi::Request* request_;

	std::function<void()> on_close_;
};

} // namespace firesse
