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

	std::chrono::steady_clock::time_point LastMessageTime();
	void Close();

  private:
	firecgi::Request* request_;

	std::function<void()> on_close_;

	std::mutex mu_;
	std::chrono::steady_clock::time_point last_message_time_;
};

class IsFresherStream {
  public:
	bool operator() (Stream* a, Stream* b) const;
};

} // namespace firesse
