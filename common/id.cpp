module;
#include <cstdint>
#include <limits>
#include <string>
export module id;
export struct id {
    template<class T> struct Base {
        T id;

        static constexpr auto INVALID = std::numeric_limits<T>::max();
        static std::string stringify(Base base) {
          return (base.id == INVALID) ? "INVALID" : std::to_string(base.id);
        }
    };
    struct Order: public Base<std::uint64_t> { };
    struct Ticker: public Base<std::uint32_t> { };
    struct Client: public Base<std::uint32_t> { };
}; // namespace id
