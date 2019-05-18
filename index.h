#pragma once

#include "stream.h"

namespace firesse {

// Track live streams
// O(1) for Add(), Remove(), and Freshen()
// Manages this by only supporting insertion of the freshest element,
// and by requiring an iterator for removal.
// Avoids allocation and folds in iterators by using an intrusive list
// inside Stream.
class Index {
 public:
  void Add(Stream* stream);
  void Remove(Stream* stream);
  void Freshen(Stream* stream);

  // Returns time to sleep until next stalest, or min_stale if none
  // Only calls callback if stalest is at least min_stale
  // Handles all locking and marks Stream as fresh after callback
  std::chrono::nanoseconds WithStalest(
      std::function<void(Stream*)> callback,
      const std::chrono::nanoseconds& min_stale);

 private:
  std::recursive_mutex mu_;
  Stream* freshest_ = nullptr;
  Stream* stalest_ = nullptr;
};

}  // namespace firesse
