module;
#include <iostream>
#include <vector>
#include <atomic>
#include "./macro.h"
export module Queue;
namespace common {
  export template<typename T> class Queue {
      COMMON_MACRO_DELETE_CONSTRUCTOR(Queue)
    private:
      std::vector<T> store_;
      struct {
          std::atomic<size_t> write = { 0 };
          std::atomic<size_t> read = { 0 };
      } index_;
      std::atomic<size_t> length = { 0 };
    public:
      Queue(std::size_t length)
        : store_(length, T()) {
      }
      auto getNextToWriteTo() noexcept {
        return &this->store_[this->index_.write];
      }
      auto updateWriteIndex() noexcept {
        this->index_.write = (this->index_.write + 1) % this->store_.size();
        this->length++;
      }
      auto getNextToRead() const noexcept -> const T* {
        return (this->size() ? &this->store_[this->index_.read] : nullptr);
      }
      auto updateReadIndex() noexcept {
        this->index_.read = (this->index_.read + 1) % this->store_.size();
        ASSERT(
          this->length != 0,
          "Read an invalid element in:" + std::to_string(pthread_self()));
        this->length--;
      }
      auto size() const noexcept {
        return this->length.load();
      }
  };
} // namespace common
