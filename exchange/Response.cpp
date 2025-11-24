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
    // AUTO TOSTRING() CONST {
    //   STD::STRINGSTREAM SS;
    //   SS << "RESPONSE"
    //      << " ["
    //      << "TYPE:" << STRINGIFYTYPE(THIS->TYPE_)
    //      << " CLIENT:" << COMMON::ID::STRINGIFY(THIS->CLIENT_ID_)
    //      << " TICKER:" << COMMON::ID::STRINGIFY(THIS->TICKER_ID_)
    //      << " COID:" << COMMON::ID::STRINGIFY(THIS->CLIENT_ORDER_ID_)
    //      << " MOID:" << COMMON::ID::STRINGIFY(THIS->MARKET_ORDER_ID_)
    //      << " SIDE:" << COMMON::SIDE::STRINGIFY(THIS->SIDE_.VALUE)
    //      << " EXEC_QTY:" << COMMON::QUANTITY_STRINGIFY(THIS->EXEC_QTY_)
    //      << " LEAVES_QTY:" << COMMON::QUANTITY_STRINGIFY(THIS->LEAVES_QTY_)
    //      << " PRICE:" << COMMON::PRICE_STRINGIFY(THIS->PRICE_) << "]";
    //   RETURN SS.STR();
    // }
};
#pragma pack(pop)
