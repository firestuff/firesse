#include "stream.h"

namespace firesse {

Stream::Stream(std::unique_ptr<firecgi::Request> request)
		: request_(std::move(request)) {}

bool Stream::WriteEvent(const std::string& data, uint64_t id, const std::string& type) {
	if (id) {
		request_->WriteBody("id: ");
		request_->WriteBody(std::to_string(id));
		request_->WriteBody("\n");
	}
	if (!type.empty()) {
		request_->WriteBody("event: ");
		request_->WriteBody(type);
		request_->WriteBody("\n");
	}
	request_->WriteBody("data: ");
	request_->WriteBody(data);
	request_->WriteBody("\n\n");

	return request_->Flush();
}

bool Stream::End() {
	return request_->End();
}

} // namespace firesse
