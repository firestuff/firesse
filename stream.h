#pragma once

#include <memory>

#include "firecgi/request.h"

namespace firesse {

class Index;

class Stream {
  public:
	Stream(firecgi::Request* request, Index* index);
	~Stream();

	void OnClose(const std::function<void()>& callback);

	bool WriteEvent(const std::string& data, uint64_t id=0, const std::string& type="");
	bool End();
	void Close();

  private:
	firecgi::Request* request_;
	Index* index_;

	std::function<void()> on_close_;

	std::mutex mu_;
	std::chrono::steady_clock::time_point last_message_time_;
	Stream* fresher_;
	Stream* staler_;

	friend class Index;
};

} // namespace firesse
