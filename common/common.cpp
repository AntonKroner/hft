module;
#include <cstdint>
export module common;
export import id;
export import Price;
export import Quantity;
export import Priority;
export import Side;
export import Queue;
export import Log;
export namespace common {
  constexpr std::size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;
  constexpr std::size_t ME_MAX_TICKERS = 8;
  constexpr std::size_t ME_MAX_CLIENT_UPDATES = 256 * 1024;
  constexpr std::size_t ME_MAX_MARKET_UPDATES = 256 * 1024;
  constexpr std::size_t ME_MAX_NUM_CLIENTS = 256;
  constexpr std::size_t ME_MAX_ORDER_IDS = 1024 * 1024;
  constexpr std::size_t ME_MAX_PRICE_LEVELS = 256;
}; // namespace common
