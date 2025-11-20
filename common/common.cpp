module;
#include <cstdint>
export module common;
import id;
import Price;
import Quantity;
import Priority;
import Side;
export namespace common {
  using id = id;
  using Price = Price;
  using Quantity = Quantity;
  using Priority = Priority;
  using Side = Side;
  constexpr std::size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;
  constexpr std::size_t ME_MAX_TICKERS = 8;
  constexpr std::size_t ME_MAX_CLIENT_UPDATES = 256 * 1024;
  constexpr std::size_t ME_MAX_MARKET_UPDATES = 256 * 1024;
  constexpr std::size_t ME_MAX_NUM_CLIENTS = 256;
  constexpr std::size_t ME_MAX_ORDER_IDS = 1024 * 1024;
  constexpr std::size_t ME_MAX_PRICE_LEVELS = 256;
}; // namespace common
