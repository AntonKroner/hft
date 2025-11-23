module;
#include <cstdint>
#include <string>
export module Side;
namespace common {
  export class Side {
    public:
      enum class Value : int8_t { SELL = -1, INVALID = 0, BUY = 1 };
      Value value = Value::INVALID;
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
          case Value::SELL:
            return "SELL";
          case Value::BUY:
            return "BUY";
          case Value::INVALID:
            return "INVALID";
        }
      }
  };
} // namespace common
