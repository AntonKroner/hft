module;
#include <cstdint>
export module Quantity;
import id;
namespace common {
  export typedef std::uint32_t Quantity;
  export constexpr Quantity QUANTITY_INVALID = id::INVALID<Quantity>;
  export inline auto Quantity_stringify(Quantity quantity) {
    return id::stringify(quantity);
  }
} // namespace common
