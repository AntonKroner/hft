module;
#include <cstdint>
#include <string>
// #include <sstream>
export module Response;
import common;
#pragma pack(push, 1)
export struct Response {
    enum class Type : std::uint8_t {
      INVALID = 0,
      ACCEPTED = 1,
      CANCELED = 2,
      FILLED = 3,
      CANCEL_REJECTED = 4
    };
    static inline std::string stringifyType(Type type) {
      switch (type) {
        case Type::ACCEPTED:
          return "ACCEPTED";
        case Type::CANCELED:
          return "CANCELED";
        case Type::FILLED:
          return "FILLED";
        case Type::CANCEL_REJECTED:
          return "CANCEL_REJECTED";
        case Type::INVALID:
          return "INVALID";
      }
      return "UNKNOWN";
    }
    Type type_ = Type::INVALID;
    common::id::Client client_id_ = common::id::INVALID<common::id::Client>;
    common::id::Ticker ticker_id_ = common::id::INVALID<common::id::Ticker>;
    common::id::Order client_order_id_ = common::id::INVALID<common::id::Order>;
    common::id::Order market_order_id_ = common::id::INVALID<common::id::Order>;
    common::Side side_ = common::Side::INVALID;
    common::Price price_ = common::PRICE_INVALID;
    common::Quantity exec_qty_ = common::QUANTITY_INVALID;
    common::Quantity leaves_qty_ = common::QUANTITY_INVALID;
    auto toString() const {
      std::stringstream ss;
      ss << "response"
         << " ["
         << "type:" << stringifyType(this->type_)
         << " client:" << common::id::stringify(this->client_id_)
         << " ticker:" << common::id::stringify(this->ticker_id_)
         << " coid:" << common::id::stringify(this->client_order_id_)
         << " moid:" << common::id::stringify(this->market_order_id_)
         << " side:" << common::Side::stringify(this->side_.value)
         << " exec_qty:" << common::Quantity_stringify(this->exec_qty_)
         << " leaves_qty:" << common::Quantity_stringify(this->leaves_qty_)
         << " price:" << common::Price_stringify(this->price_) << "]";
      return ss.str();
    }
};
#pragma pack(pop)
