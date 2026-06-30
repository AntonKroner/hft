module;
#include <cstdint>
#include <vector>
#include "./macro.hpp"
export module Pool;
namespace common {
  export template<typename T> class Pool final {
    private:
      struct ObjectBlock {
          T object_;
          bool isFree_ = true;
      };
      // TODO: compare performance when using static/stack/heap allocated store
      std::vector<ObjectBlock> store_;
      size_t next = 0;
    public:
      COMMON_MACRO_DELETE_CONSTRUCTOR(Pool)
      explicit Pool(std::size_t size)
        : store_(size, { T(), true }) {
        ASSERT(
          reinterpret_cast<const ObjectBlock*>(&(store_[0].object_)) == &(store_[0]),
          "T object should be first member of ObjectBlock.");
      }

      template<typename... Args> T* allocate(Args... args) noexcept {
        auto obj_block = &(store_[next]);
        ASSERT(
          obj_block->isFree_,
          "Expected free ObjectBlock at index:" + std::to_string(next));
        T* ret = &(obj_block->object_);
        ret = new (ret) T(args...); // placement new.
        obj_block->isFree_ = false;
        updateNextFreeIndex();
        return ret;
      }
      auto deallocate(const T* elem) noexcept {
        const auto elem_index = (reinterpret_cast<const ObjectBlock*>(elem) - &store_[0]);
        ASSERT(
          elem_index >= 0 && static_cast<size_t>(elem_index) < store_.size(),
          "Element being deallocated does not belong to this Memory pool.");
        ASSERT(
          !store_[elem_index].isFree_,
          "Expected in-use ObjectBlock at index:" + std::to_string(elem_index));
        store_[elem_index].isFree_ = true;
      }
      auto updateNextFreeIndex() noexcept {
        const auto initial_free_index = next;
        while (!store_[next].isFree_) {
          ++next;
          // hardware branch predictor should almost always predict this to be false any ways.
          if (UNLIKELY(next == store_.size())) {
            next = 0;
          }
          if (UNLIKELY(initial_free_index == next)) {
            ASSERT(initial_free_index != next, "Memory Pool out of space.");
          }
        }
      }
  };
} // namespace common
