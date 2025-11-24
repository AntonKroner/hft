module;
#include <cstdint>
#include <string>
export module Side;
namespace common {
  export class Side {
    public:
      enum Value : int8_t { SELL = -1, INVALID = 0, BUY = 1 };
      Value value = INVALID;
      Side() = default;
      constexpr Side(Value side)
        : value(side) {
      }
      // #if Enable switch (fruit) use case:
      //     // Allow switch and comparisons.
      //     constexpr operator Value() const {
      //       return value;
      //     }
      //     // Prevent usage: if(fruit)
      //     explicit operator bool() const = delete;
      // #else
      constexpr bool operator==(Side a) const {
        return value == a.value;
      }
      constexpr bool operator!=(Side a) const {
        return value != a.value;
      }
      static std::string stringify(Value side) {
        switch (side) {
          case SELL:
            return "SELL";
          case BUY:
            return "BUY";
          case INVALID:
            return "INVALID";
        }
        return "UNKNOWN";
      }
  };
} // namespace common
