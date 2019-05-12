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

std::chrono::nanoseconds Index::WithStalest(std::function<void(Stream*)> callback, const std::chrono::nanoseconds& min_stale) {
	Stream* stalest = nullptr;
	std::chrono::nanoseconds ret;
	const auto now = std::chrono::steady_clock::now();
	const auto latest = now - min_stale;

	{
		std::lock_guard l(mu_);
		if (!stalest_) {
			return min_stale;
		}
		if (stalest_->last_message_time_ > latest) {
			return stalest_->last_message_time_ - latest;
		}

		// stalest_ is valid for callback
		stalest = stalest_;

		if (stalest->fresher_) {
			if (stalest->fresher_->last_message_time_ > latest) {
				ret = stalest_->fresher_->last_message_time_ - latest;
			}
			// Otherwise ret is 0 for immediate cycle
		} else {
			ret = min_stale;
		}

		if (!stalest->mu_.try_lock()) {
			// We're acquiring mutexes in the wrong order here; normally it's
			// (Stream, Index), but we're doing (Index, Stream). That means we
			// may deadlock. We take the lower-priority path and fail in case
			// of deadlock.
			return {};
		}

		Freshen(stalest);
	}

	callback(stalest);
	stalest->mu_.unlock();

	return ret;
}

} // namespace firesse
