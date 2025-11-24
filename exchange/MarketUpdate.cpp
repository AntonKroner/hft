module;
#include <cstdint>
#include <string>
// #include <sstream>
export module MarketUpdate;
import common;
#pragma pack(push, 1)
export struct MarketUpdate {
    enum class Type : std::uint8_t {
      INVALID = 0,
      ADD = 1,
      MODIFY = 2,
      CANCEL = 3,
      TRADE = 4
    };
    static inline std::string stringifyType(Type type) {
      switch (type) {
        case Type::ADD:
          return "ADD";
        case Type::MODIFY:
          return "MODIFY";
        case Type::CANCEL:
          return "CANCEL";
        case Type::TRADE:
          return "TRADE";
        case Type::INVALID:
          return "INVALID";
      }
      return "UNKNOWN";
    }
    Type type = Type::INVALID;
    common::id::Order orderId = common::id::INVALID<common::id::Order>;
    common::id::Ticker tickerId = common::id::INVALID<common::id::Ticker>;
    common::Side side = common::Side::Value::INVALID;
    common::Price price = common::PRICE_INVALID;
    common::Quantity quantity = common::QUANTITY_INVALID;
    common::Priority priority = common::PRIORITY_INVALID;
    // auto toString() const {
    //   std::stringstream ss;
    //   ss << "MarketUpdate"
    //      << " ["
    //      << " type:" << stringifyType(this->type)
    //      << " ticker:" << common::id::stringify(this->tickerId)
    //      << " oid:" << common::id::stringify(this->orderId)
    //      << " side:" << common::Side::stringify(this->side.value)
    //      << " qty:" << common::Quantity_stringify(this->quantity)
    //      << " price:" << common::Price_stringify(this->price)
    //      << " priority:" << common::Priority_stringify(this->priority) << "]";
    //   return ss.str();
    // }
};
#pragma pack(pop)
