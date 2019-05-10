#include "stream.h"

namespace firesse {

Stream::Stream(firecgi::Request* request)
		: request_(request) {}

bool Stream::WriteEvent(const std::string& data, uint64_t id, const std::string& type) {
	if (id) {
		request_->WriteBody("id: ", std::to_string(id), "\n");
	}
	if (!type.empty()) {
		request_->WriteBody("event: ", type, "\n");
	}
	request_->WriteBody("data: ", data, "\n");

	return request_->Flush();
}

bool Stream::End() {
	return request_->End();
}

} // namespace firesse
