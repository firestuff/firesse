#include "index.h"

namespace firesse {

void Index::Add(Stream* stream) {
	std::lock_guard l(mu_);

	stream->last_message_time_ = std::chrono::steady_clock::now();
	stream->fresher_ = nullptr;
	stream->staler_ = freshest_;
	if (stream->staler_) {
		stream->staler_->fresher_ = stream;
	}
	freshest_ = stream;
	if (!stalest_) {
		stalest_ = stream;
	}
}

void Index::Remove(Stream* stream) {
	std::lock_guard l(mu_);

	if (freshest_ == stream) {
		freshest_ = stream->staler_;
	}
	if (stalest_ == stream) {
		stalest_ = stream->fresher_;
	}
	if (stream->fresher_) {
		stream->fresher_->staler_ = stream->staler_;
		stream->fresher_ = nullptr;
	}
	if (stream->staler_) {
		stream->staler_->fresher_ = stream->fresher_;
		stream->staler_ = nullptr;
	}
}

void Index::Freshen(Stream* stream) {
	std::lock_guard l(mu_);
	if (freshest_ == stream) {
		// Shortcut
		stream->last_message_time_ = std::chrono::steady_clock::now();
		return;
	}
	Remove(stream);
	Add(stream);
}

} // namespace firesse
