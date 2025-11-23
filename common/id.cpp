module;
#include <cstdint>
#include <limits>
#include <string>
export module id;
export namespace common::id {
  typedef std::uint64_t Order;
  typedef std::uint32_t Ticker;
  typedef std::uint32_t Client;
  template<class T> constexpr auto INVALID = std::numeric_limits<T>::max();
  template<class T> std::string stringify(T id) {
    return (id == INVALID<T>) ? "INVALID" : std::to_string(id);
  }
}; // namespace common::id
