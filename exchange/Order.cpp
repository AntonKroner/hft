module;
#include <array>
// #include <sstream>
export module Order;
import common;
export struct Order {
    common::id::Ticker ticker_id_ = common::id::INVALID<common::id::Ticker>;
    common::id::Client client_id_ = common::id::INVALID<common::id::Client>;
    common::id::Order client_order_id_ = common::id::INVALID<common::id::Order>;
    common::id::Order market_order_id_ = common::id::INVALID<common::id::Order>;
    common::Side side_ = common::Side::INVALID;
    common::Price price_ = common::PRICE_INVALID;
    common::Quantity quantity_ = common::QUANTITY_INVALID;
    common::Priority priority_ = common::PRIORITY_INVALID;
    Order* prev_order_ = nullptr;
    Order* next_order_ = nullptr;
    // only needed for use with MemPool.
    Order() = default;
    Order(
      common::id::Ticker ticker_id,
      common::id::Client client_id,
      common::id::Order client_order_id,
      common::id::Order market_order_id,
      common::Side side,
      common::Price price,
      common::Quantity quantity,
      common::Priority priority,
      Order* prev_order,
      Order* next_order) noexcept
      : ticker_id_(ticker_id)
      , client_id_(client_id)
      , client_order_id_(client_order_id)
      , market_order_id_(market_order_id)
      , side_(side)
      , price_(price)
      , quantity_(quantity)
      , priority_(priority)
      , prev_order_(prev_order)
      , next_order_(next_order) {
    }
    // auto toString() const -> std::string {
    //   std::stringstream ss;
    //   ss << "MEOrder" << "["
    //      << "ticker:" << tickerIdToString(ticker_id_) << " "
    //      << "cid:" << clientIdToString(client_id_) << " "
    //      << "oid:" << orderIdToString(client_order_id_) << " "
    //      << "moid:" << orderIdToString(market_order_id_) << " "
    //      << "side:" << sideToString(side_) << " "
    //      << "price:" << priceToString(price_) << " "
    //      << "qty:" << qtyToString(quantity_) << " "
    //      << "prio:" << priorityToString(priority_) << " "
    //      << "prev:"
    //      << orderIdToString(prev_order_ ? prev_order_->market_order_id_ : OrderId_INVALID)
    //      << " "
    //      << "next:"
    //      << orderIdToString(next_order_ ? next_order_->market_order_id_ : OrderId_INVALID)
    //      << "]";
    //   return ss.str();
    // }
};
typedef std::array<Order*, common::ME_MAX_ORDER_IDS> OrderHashMap;
typedef std::array<OrderHashMap, common::ME_MAX_NUM_CLIENTS> ClientOrderHashMap;
struct OrdersAtPrice {
    common::Side side_ = common::Side::INVALID;
    common::Price price_ = common::PRICE_INVALID;
    Order* first_me_order_ = nullptr;
    OrdersAtPrice* prev_entry_ = nullptr;
    OrdersAtPrice* next_entry_ = nullptr;
    OrdersAtPrice() = default;
    OrdersAtPrice(
      common::Side side,
      common::Price price,
      Order* first_me_order,
      OrdersAtPrice* prev_entry,
      OrdersAtPrice* next_entry)
      : side_(side)
      , price_(price)
      , first_me_order_(first_me_order)
      , prev_entry_(prev_entry)
      , next_entry_(next_entry) {
    }
    // auto toString() const {
    //   std::stringstream ss;
    //   ss << "OrdersAtPrice["
    //      << "side:" << sideToString(side_) << " "
    //      << "price:" << priceToString(price_) << " "
    //      << "first_me_order:" << (first_me_order_ ? first_me_order_->toString() : "null")
    //      << " "
    //      << "prev:" << priceToString(prev_entry_ ? prev_entry_->price_ : Price_INVALID)
    //      << " "
    //      << "next:" << priceToString(next_entry_ ? next_entry_->price_ : Price_INVALID)
    //      << "]";
    //   return ss.str();
    // }
};
typedef std::array<OrdersAtPrice*, common::ME_MAX_PRICE_LEVELS> OrdersAtPriceHashMap;
