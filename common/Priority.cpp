module;
#include <cstdint>
export module Priority;
import id;
namespace common {
  export typedef std::uint64_t Priority;
  export constexpr Priority PRIORITY_INVALID = id::INVALID<Priority>;
  export inline auto Priority_stringify(Priority priority) {
    return id::stringify(priority);
  }
} // namespace common
