module;
#include <cstdint>
#include <string>
// #include <sstream>
#include <sys/types.h>
export module Request;
import common;
#pragma pack(push, 1)
export struct Request {
    enum class Type : std::uint8_t { INVALID = 0, NEW = 1, CANCEL = 2 };
    static inline std::string stringifyType(Type type) {
      switch (type) {
        case Type::NEW:
          return "NEW";
        case Type::CANCEL:
          return "CANCEL";
        case Type::INVALID:
          return "INVALID";
      }
      return "UNKNOWN";
    }
    Type type_ = Type::INVALID;
    common::id::Client client_id_ = common::id::INVALID<common::id::Client>;
    common::id::Ticker ticker_id_ = common::id::INVALID<common::id::Ticker>;
    common::id::Order order_id_ = common::id::INVALID<common::id::Order>;
    common::Side side_ = common::Side::INVALID;
    common::Price price_ = common::PRICE_INVALID;
    common::Quantity qty_ = common::QUANTITY_INVALID;
    // auto toString() const {
    //   std::stringstream ss;
    //   ss << "Request"
    //      << " ["
    //      << "type:" << stringifyType(type_)
    //      << " client:" << common::id::stringify(client_id_)
    //      << " ticker:" << common::id::stringify(ticker_id_)
    //      << " oid:" << common::id::stringify(order_id_)
    //      << " side:" << common::Side::stringify(side_.value)
    //      << " qty:" << common::Quantity_stringify(qty_)
    //      << " price:" << common::Price_stringify(price_) << "]";
    //   return ss.str();
    // }
};
#pragma pack(pop)
