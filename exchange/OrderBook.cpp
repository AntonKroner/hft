module;
#include "../common/macro.hpp"
#include "../common/time_utils.hpp"
export module OrderBook;
import common;
import Order;
import MarketUpdate;
import Response;
// TODO: not super impressed with this forward declaration
export class OrderBook final {
  public:
    class MatchingEngine {
      public:
        virtual ~MatchingEngine() = default;
        virtual void sendClientResponse(const Response* response) noexcept = 0;
        virtual void sendMarketUpdate(const MarketUpdate* update) noexcept = 0;
    };
    explicit OrderBook(
      common::id::Ticker ticker_id,
      common::Log* logger,
      MatchingEngine* matching_engine);
    COMMON_MACRO_DELETE_CONSTRUCTOR(OrderBook)
    ~OrderBook();
    auto add(
      common::id::Client client_id,
      common::id::Order client_order_id,
      common::id::Ticker ticker_id,
      common::Side side,
      common::Price price,
      common::Quantity qty) noexcept -> void;
    auto cancel(
      common::id::Client client_id,
      common::id::Order order_id,
      common::id::Ticker ticker_id) noexcept -> void;
    auto toString(bool detailed, bool validity_check) const -> std::string;
  private:
    common::id::Ticker ticker_id_ = common::id::INVALID<common::id::Ticker>;
    MatchingEngine* matching_engine_ = nullptr;
    Order::ClientOrderHashMap cid_oid_to_order_;
    common::Pool<Order::OrdersAtPrice> orders_at_price_pool_;
    Order::OrdersAtPrice* bids_by_price_ = nullptr;
    Order::OrdersAtPrice* asks_by_price_ = nullptr;
    Order::OrdersAtPriceHashMap price_orders_at_price_;
    common::Pool<Order> order_pool_;
    Response client_response_;
    MarketUpdate market_update_;
    common::id::Order next_market_order_id_ = 1;
    std::string time_str_;
    common::Log* logger_ = nullptr;
  private:
    auto generateNewMarketOrderId() noexcept -> common::id::Order {
      return next_market_order_id_++;
    }
    auto priceToIndex(common::Price price) const noexcept {
      return (price % common::ME_MAX_PRICE_LEVELS);
    }
    auto getOrdersAtPrice(common::Price price) const noexcept -> Order::OrdersAtPrice* {
      return price_orders_at_price_.at(priceToIndex(price));
    }
    auto addOrdersAtPrice(Order::OrdersAtPrice* new_orders_at_price) noexcept {
      price_orders_at_price_.at(priceToIndex(new_orders_at_price->price_)) =
        new_orders_at_price;
      const auto best_orders_by_price =
        (new_orders_at_price->side_ == common::Side::BUY ? bids_by_price_
                                                         : asks_by_price_);
      if (UNLIKELY(!best_orders_by_price)) {
        (new_orders_at_price->side_ == common::Side::BUY
           ? bids_by_price_
           : asks_by_price_) = new_orders_at_price;
        new_orders_at_price->prev_entry_ = new_orders_at_price->next_entry_ =
          new_orders_at_price;
      }
      else {
        auto target = best_orders_by_price;
        bool add_after = ((new_orders_at_price->side_ == common::Side::SELL && new_orders_at_price->price_ > target->price_) ||
                          (new_orders_at_price->side_ == common::Side::BUY && new_orders_at_price->price_ < target->price_));
        if (add_after) {
          target = target->next_entry_;
          add_after = ((new_orders_at_price->side_ == common::Side::SELL && new_orders_at_price->price_ > target->price_) ||
                       (new_orders_at_price->side_ == common::Side::BUY && new_orders_at_price->price_ < target->price_));
        }
        while (add_after && target != best_orders_by_price) {
          add_after = ((new_orders_at_price->side_ == common::Side::SELL && new_orders_at_price->price_ > target->price_) ||
                       (new_orders_at_price->side_ == common::Side::BUY && new_orders_at_price->price_ < target->price_));
          if (add_after) {
            target = target->next_entry_;
          }
        }
        if (add_after) { // add new_orders_at_price after target.
          if (target == best_orders_by_price) {
            target = best_orders_by_price->prev_entry_;
          }
          new_orders_at_price->prev_entry_ = target;
          target->next_entry_->prev_entry_ = new_orders_at_price;
          new_orders_at_price->next_entry_ = target->next_entry_;
          target->next_entry_ = new_orders_at_price;
        }
        else { // add new_orders_at_price before target.
          new_orders_at_price->prev_entry_ = target->prev_entry_;
          new_orders_at_price->next_entry_ = target;
          target->prev_entry_->next_entry_ = new_orders_at_price;
          target->prev_entry_ = new_orders_at_price;
          if ((new_orders_at_price->side_ == common::Side::BUY && new_orders_at_price->price_ > best_orders_by_price->price_) ||
              (new_orders_at_price->side_ == common::Side::SELL && new_orders_at_price->price_ < best_orders_by_price->price_)) {
            target->next_entry_ =
              (target->next_entry_ == best_orders_by_price ? new_orders_at_price
                                                           : target->next_entry_);
            (new_orders_at_price->side_ == common::Side::BUY
               ? bids_by_price_
               : asks_by_price_) = new_orders_at_price;
          }
        }
      }
    }
    auto removeOrdersAtPrice(common::Side side, common::Price price) noexcept {
      const auto best_orders_by_price =
        (side == common::Side::BUY ? bids_by_price_ : asks_by_price_);
      auto orders_at_price = this->getOrdersAtPrice(price);
      if (UNLIKELY(orders_at_price->next_entry_ == orders_at_price)) { // empty side of book.
        (side == common::Side::BUY ? bids_by_price_ : asks_by_price_) = nullptr;
      }
      else {
        orders_at_price->prev_entry_->next_entry_ = orders_at_price->next_entry_;
        orders_at_price->next_entry_->prev_entry_ = orders_at_price->prev_entry_;
        if (orders_at_price == best_orders_by_price) {
          (side == common::Side::BUY ? bids_by_price_ : asks_by_price_) =
            orders_at_price->next_entry_;
        }
        orders_at_price->prev_entry_ = orders_at_price->next_entry_ = nullptr;
      }
      price_orders_at_price_.at(priceToIndex(price)) = nullptr;
      orders_at_price_pool_.deallocate(orders_at_price);
    }
    auto getNextPriority(common::Price price) noexcept {
      const auto orders_at_price = this->getOrdersAtPrice(price);
      if (!orders_at_price) {
        return 1lu;
      }
      return orders_at_price->first_me_order_->prev_order_->priority_ + 1;
    }
    auto match(
      common::id::Ticker ticker_id,
      common::id::Client client_id,
      common::Side side,
      common::id::Order client_order_id,
      common::id::Order new_market_order_id,
      Order* bid_itr,
      common::Quantity* leaves_qty) noexcept;
    auto checkForMatch(
      common::id::Client client_id,
      common::id::Order client_order_id,
      common::id::Ticker ticker_id,
      common::Side side,
      common::Price price,
      common::Quantity qty,
      common::Quantity new_market_order_id) noexcept;
    auto removeOrder(Order* order) noexcept {
      auto orders_at_price = this->getOrdersAtPrice(order->price_);
      if (order->prev_order_ == order) { // only one element.
        removeOrdersAtPrice(order->side_, order->price_);
      }
      else { // remove the link.
        const auto order_before = order->prev_order_;
        const auto order_after = order->next_order_;
        order_before->next_order_ = order_after;
        order_after->prev_order_ = order_before;
        if (orders_at_price->first_me_order_ == order) {
          orders_at_price->first_me_order_ = order_after;
        }
        order->prev_order_ = order->next_order_ = nullptr;
      }
      cid_oid_to_order_.at(order->client_id_).at(order->client_order_id_) = nullptr;
      order_pool_.deallocate(order);
    }
    auto addOrder(Order* order) noexcept {
      const auto orders_at_price = this->getOrdersAtPrice(order->price_);
      if (!orders_at_price) {
        order->next_order_ = order->prev_order_ = order;
        auto new_orders_at_price =
          orders_at_price_pool_
            .allocate(order->side_, order->price_, order, nullptr, nullptr);
        addOrdersAtPrice(new_orders_at_price);
      }
      else {
        auto first_order = (orders_at_price ? orders_at_price->first_me_order_ : nullptr);
        first_order->prev_order_->next_order_ = order;
        order->prev_order_ = first_order->prev_order_;
        order->next_order_ = first_order;
        first_order->prev_order_ = order;
      }
      cid_oid_to_order_.at(order->client_id_).at(order->client_order_id_) = order;
    }
  public:
    typedef std::array<OrderBook*, common::ME_MAX_TICKERS> OrderBookHashMap;
};

OrderBook::OrderBook(
  common::id::Ticker ticker_id,
  common::Log* logger,
  MatchingEngine* matching_engine)
  : ticker_id_(ticker_id)
  , matching_engine_(matching_engine)
  , orders_at_price_pool_(common::ME_MAX_PRICE_LEVELS)
  , order_pool_(common::ME_MAX_ORDER_IDS)
  , logger_(logger) {
}
OrderBook::~OrderBook() {
  logger_->log(
    "%:% %() % OrderBook\n%\n",
    __FILE__,
    __LINE__,
    __FUNCTION__,
    Common::getCurrentTimeStr(&time_str_),
    toString(false, true));
  matching_engine_ = nullptr;
  bids_by_price_ = asks_by_price_ = nullptr;
  for (auto& itr: cid_oid_to_order_) {
    itr.fill(nullptr);
  }
}
auto OrderBook::match(
  common::id::Ticker ticker_id,
  common::id::Client client_id,
  common::Side side,
  common::id::Order client_order_id,
  common::id::Order new_market_order_id,
  Order* itr,
  common::Quantity* leaves_qty) noexcept {
  const auto order = itr;
  const auto order_qty = order->quantity_;
  const auto fill_qty = std::min(*leaves_qty, order_qty);
  *leaves_qty -= fill_qty;
  order->quantity_ -= fill_qty;
  client_response_ = {
    Response::Type::FILLED,
    client_id,
    ticker_id,
    client_order_id,
    new_market_order_id,
    side,
    itr->price_,
    fill_qty,
    *leaves_qty
  };
  // matching_engine_->sendClientResponse(&client_response_);
  client_response_ = {
    Response::Type::FILLED,  order->client_id_, ticker_id,   order->client_order_id_,
    order->market_order_id_, order->side_,      itr->price_, fill_qty,
    order->quantity_
  };
  // matching_engine_->sendClientResponse(&client_response_);
  market_update_ = { MarketUpdate::Type::TRADE,
                     common::id::INVALID<common::id::Order>,
                     ticker_id,
                     side,
                     itr->price_,
                     fill_qty,
                     common::PRIORITY_INVALID };
  // matching_engine_->sendMarketUpdate(&market_update_);
  if (!order->quantity_) {
    market_update_ = { MarketUpdate::Type::CANCEL,
                       order->market_order_id_,
                       ticker_id,
                       order->side_,
                       order->price_,
                       order_qty,
                       common::PRIORITY_INVALID };
    // matching_engine_->sendMarketUpdate(&market_update_);
    removeOrder(order);
  }
  else {
    market_update_ = { MarketUpdate::Type::MODIFY,
                       order->market_order_id_,
                       ticker_id,
                       order->side_,
                       order->price_,
                       order->quantity_,
                       order->priority_ };
    // matching_engine_->sendMarketUpdate(&market_update_);
  }
}
auto OrderBook::checkForMatch(
  common::id::Client client_id,
  common::id::Order client_order_id,
  common::id::Ticker ticker_id,
  common::Side side,
  common::Price price,
  common::Quantity qty,
  common::Quantity new_market_order_id) noexcept {
  auto leaves_qty = qty;
  if (side == common::Side::BUY) {
    while (leaves_qty && asks_by_price_) {
      const auto ask_itr = asks_by_price_->first_me_order_;
      if (LIKELY(price < ask_itr->price_)) {
        break;
      }
      match(
        ticker_id,
        client_id,
        side,
        client_order_id,
        new_market_order_id,
        ask_itr,
        &leaves_qty);
    }
  }
  if (side == common::Side::SELL) {
    while (leaves_qty && bids_by_price_) {
      const auto bid_itr = bids_by_price_->first_me_order_;
      if (LIKELY(price > bid_itr->price_)) {
        break;
      }
      match(
        ticker_id,
        client_id,
        side,
        client_order_id,
        new_market_order_id,
        bid_itr,
        &leaves_qty);
    }
  }
  return leaves_qty;
}
auto OrderBook::add(
  common::id::Client client_id,
  common::id::Order client_order_id,
  common::id::Ticker ticker_id,
  common::Side side,
  common::Price price,
  common::Quantity qty) noexcept -> void {
  const auto new_market_order_id = generateNewMarketOrderId();
  client_response_ = {
    Response::Type::ACCEPTED,
    client_id,
    ticker_id,
    client_order_id,
    new_market_order_id,
    side,
    price,
    0,
    qty
  };
  // matching_engine_->sendClientResponse(&client_response_);
  const auto leaves_qty = checkForMatch(
    client_id,
    client_order_id,
    ticker_id,
    side,
    price,
    qty,
    new_market_order_id);
  if (LIKELY(leaves_qty)) {
    const auto priority = getNextPriority(price);
    auto order = order_pool_.allocate(
      ticker_id,
      client_id,
      client_order_id,
      new_market_order_id,
      side,
      price,
      leaves_qty,
      priority,
      nullptr,
      nullptr);
    addOrder(order);
    market_update_ = { MarketUpdate::Type::ADD,
                       new_market_order_id,
                       ticker_id,
                       side,
                       price,
                       leaves_qty,
                       priority };
    // matching_engine_->sendMarketUpdate(&market_update_);
  }
}
auto OrderBook::cancel(
  common::id::Client client_id,
  common::id::Order order_id,
  common::id::Ticker ticker_id) noexcept -> void {
  auto is_cancelable = (client_id < cid_oid_to_order_.size());
  Order* exchange_order = nullptr;
  if (LIKELY(is_cancelable)) {
    auto& co_itr = cid_oid_to_order_.at(client_id);
    exchange_order = co_itr.at(order_id);
    is_cancelable = (exchange_order != nullptr);
  }
  if (UNLIKELY(!is_cancelable)) {
    client_response_ = {
      Response::Type::CANCEL_REJECTED,
      client_id,
      ticker_id,
      order_id,
      common::id::INVALID<common::id::Order>,
      common::Side::INVALID,
      common::PRICE_INVALID,
      common::QUANTITY_INVALID,
      common::QUANTITY_INVALID
    };
  }
  else {
    client_response_ = {
      Response::Type::CANCELED,
      client_id,
      ticker_id,
      order_id,
      exchange_order->market_order_id_,
      exchange_order->side_,
      exchange_order->price_,
      common::QUANTITY_INVALID,
      exchange_order->quantity_
    };
    market_update_ = {
      MarketUpdate::Type::CANCEL, exchange_order->market_order_id_, ticker_id,
      exchange_order->side_,      exchange_order->price_,           0,
      exchange_order->priority_
    };
    removeOrder(exchange_order);
    // matching_engine_->sendMarketUpdate(&market_update_);
  }
  // matching_engine_->sendClientResponse(&client_response_);
}
auto OrderBook::toString(bool detailed, bool validity_check) const -> std::string {
  std::stringstream ss;
  std::string time_str;
  auto printer = [&](
                   std::stringstream& ss,
                   Order::OrdersAtPrice* itr,
                   common::Side side,
                   common::Price& last_price,
                   bool sanity_check) {
    char buf[4096];
    common::Quantity qty = 0;
    size_t num_orders = 0;
    for (auto o_itr = itr->first_me_order_;; o_itr = o_itr->next_order_) {
      qty += o_itr->quantity_;
      ++num_orders;
      if (o_itr->next_order_ == itr->first_me_order_) {
        break;
      }
    }
    sprintf(
      buf,
      " <px:%3s p:%3s n:%3s> %-3s @ %-5s(%-4s)",
      common::Price_stringify(itr->price_).c_str(),
      common::Price_stringify(itr->prev_entry_->price_).c_str(),
      common::Price_stringify(itr->next_entry_->price_).c_str(),
      common::Price_stringify(itr->price_).c_str(),
      common::Quantity_stringify(qty).c_str(),
      std::to_string(num_orders).c_str());
    ss << buf;
    for (auto o_itr = itr->first_me_order_;; o_itr = o_itr->next_order_) {
      if (detailed) {
        sprintf(
          buf,
          "[oid:%s q:%s p:%s n:%s] ",
          common::id::stringify(o_itr->market_order_id_).c_str(),
          common::Quantity_stringify(o_itr->quantity_).c_str(),
          common::id::stringify(
            o_itr->prev_order_ ? o_itr->prev_order_->market_order_id_
                               : common::id::INVALID<common::id::Order>)
            .c_str(),
          common::id::stringify(
            o_itr->next_order_ ? o_itr->next_order_->market_order_id_
                               : common::id::INVALID<common::id::Order>)
            .c_str());
        ss << buf;
      }
      if (o_itr->next_order_ == itr->first_me_order_) {
        break;
      }
    }
    ss << std::endl;
    if (sanity_check) {
      if (
        (side == common::Side::SELL && last_price >= itr->price_)
        || (side == common::Side::BUY && last_price <= itr->price_)) {
        FATAL(
          "Bids/Asks not sorted by ascending/descending prices last:"
          + common::Price_stringify(last_price) + " itr:" + itr->toString());
      }
      last_price = itr->price_;
    }
  };
  ss << "Ticker:" << common::id::stringify(ticker_id_) << std::endl;
  {
    auto ask_itr = asks_by_price_;
    auto last_ask_price = std::numeric_limits<common::Price>::min();
    for (size_t count = 0; ask_itr; ++count) {
      ss << "ASKS L:" << count << " => ";
      auto next_ask_itr =
        (ask_itr->next_entry_ == asks_by_price_ ? nullptr : ask_itr->next_entry_);
      printer(ss, ask_itr, common::Side::SELL, last_ask_price, validity_check);
      ask_itr = next_ask_itr;
    }
  }
  ss << std::endl << "                          X" << std::endl << std::endl;
  {
    auto bid_itr = bids_by_price_;
    auto last_bid_price = std::numeric_limits<common::Price>::max();
    for (size_t count = 0; bid_itr; ++count) {
      ss << "BIDS L:" << count << " => ";
      auto next_bid_itr =
        (bid_itr->next_entry_ == bids_by_price_ ? nullptr : bid_itr->next_entry_);
      printer(ss, bid_itr, common::Side::BUY, last_bid_price, validity_check);
      bid_itr = next_bid_itr;
    }
  }
  return ss.str();
}
