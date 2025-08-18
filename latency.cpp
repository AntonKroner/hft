module;
#include <cstdint>
#include <chrono>
export module latency;
export class latency {
  public:
    using clock = std::chrono::steady_clock;
    using point = clock::time_point;
    point start() const noexcept {
      return clock::now();
    }
    std::uint64_t ns_elapsed(const point& since) const noexcept {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - since)
        .count();
    }
};
