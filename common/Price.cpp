module;
#include <cstdint>
export module Price;
import id;
namespace common {
  export typedef std::int64_t Price;
  export constexpr Price PRICE_INVALID = id::INVALID<Price>;
  export inline auto Price_stringify(Price price) {
    return id::stringify(price);
  }
} // namespace common
