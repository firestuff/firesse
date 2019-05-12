#include "stream.h"

#include "index.h"

namespace firesse {

Stream::Stream(firecgi::Request* request, Index* index)
		: request_(request),
		  index_(index) {
	index_->Add(this);
}

Stream::~Stream() {
	std::lock_guard l(mu_);
	index_->Remove(this);
}

void Stream::OnClose(const std::function<void()>& callback) {
	on_close_ = callback;
}

bool Stream::WriteEvent(const std::string& data, uint64_t id, const std::string& type) {
	{
		std::lock_guard l(mu_);
		last_message_time_ = std::chrono::steady_clock::now();
	}

	return request_->InTransaction<bool>([=]() {
		if (id) {
			request_->WriteBody("id: ", std::to_string(id), "\n");
		}
		if (!type.empty()) {
			request_->WriteBody("event: ", type, "\n");
		}
		request_->WriteBody("data: ", data, "\n\n");
		return request_->Flush();
	});
}

bool Stream::End() {
	return request_->End();
}

void Stream::Close() {
	if (on_close_) {
		on_close_();
	}
}

} // namespace firesse
