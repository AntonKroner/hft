module;
#include <utility>
#include <cstdint>
#include <cstddef>
#include <cstring>
export module order_book;
export class order_book {
  public:
    struct price_level {
        std::int64_t price = 0;
        std::int32_t size = 0;
    };

    // ----------------------------------------------------------------
    // SideBook – Depth is the maximum number of price levels.
    // Ascending == true  → prices stored low → high  (asks)
    // Ascending == false → prices stored high → low (bids)
    // ----------------------------------------------------------------
    template<std::size_t Depth, bool Ascending> class SideBook {
      public:
        constexpr SideBook() = default;

        // Insert or replace a price level.
        // Returns true if the level already existed, false if it was added.
        bool upsert(const price_level& lvl) noexcept {
          for (std::size_t i = 0; i < count_; ++i) {
            if (levels_[i].price == lvl.price) {
              levels_[i].size = lvl.size;
              return true; // existing level updated
            }
          }
          // New level – insert while keeping the array sorted.
          if (count_ < Depth) {
            levels_[count_] = lvl;
            ++count_;
            // Insertion‑sort step (Depth is tiny → O(Depth) is fine)
            for (std::size_t i = count_ - 1; i > 0; --i) {
              bool out_of_order =
                Ascending ? (levels_[i].price < levels_[i - 1].price)
                          : // ascend → smaller should be earlier
                  (levels_[i].price
                   > levels_[i - 1].price); // descend → larger should be earlier
              if (out_of_order) {
                std::swap(levels_[i], levels_[i - 1]);
              }
              else {
                break;
              }
            }
            return false; // new level inserted
          }
          // Book full – in a prototype we just drop the worst level.
          return false;
        }

        // Remove a level (size == 0)
        bool erase(std::int64_t price) noexcept {
          for (std::size_t i = 0; i < count_; ++i) {
            if (levels_[i].price == price) {
              std::memmove(
                &levels_[i],
                &levels_[i + 1],
                (count_ - i - 1) * sizeof(price_level));
              --count_;
              return true;
            }
          }
          return false;
        }

        // Best price (first element if we have any levels)
        const price_level* best() const noexcept {
          return count_ ? &levels_[0] : nullptr;
        }

        // Find an exact price level (used only for debugging)
        const price_level* find(std::int64_t price) const noexcept {
          for (std::size_t i = 0; i < count_; ++i) {
            if (levels_[i].price == price) {
              return &levels_[i];
            }
          }
          return nullptr;
        }
      private:
        price_level levels_[Depth];
        std::size_t count_ = 0;
    };

    // ----------------------------------------------------------------
    // Public book: bids are descending, asks are ascending
    // ----------------------------------------------------------------
    using bid_book = SideBook<64, false>; // descending order
    using ask_book = SideBook<64, true>; // ascending order

    constexpr order_book() = default;

    // ------------------------------------------------------------
    // Convert big‑endian → host order (little‑endian on x86)
    // ------------------------------------------------------------
    static std::int64_t be64toh(const void* p) noexcept {
      const unsigned char* b = static_cast<const unsigned char*>(p);
      return (static_cast<std::int64_t>(b[0]) << 56)
             | (static_cast<std::int64_t>(b[1]) << 48)
             | (static_cast<std::int64_t>(b[2]) << 40)
             | (static_cast<std::int64_t>(b[3]) << 32)
             | (static_cast<std::int64_t>(b[4]) << 24)
             | (static_cast<std::int64_t>(b[5]) << 16)
             | (static_cast<std::int64_t>(b[6]) << 8) | (static_cast<std::int64_t>(b[7]));
    }
    static std::int32_t be32toh(const void* p) noexcept {
      const unsigned char* b = static_cast<const unsigned char*>(p);
      return (static_cast<std::int32_t>(b[0]) << 24)
             | (static_cast<std::int32_t>(b[1]) << 16)
             | (static_cast<std::int32_t>(b[2]) << 8) | (static_cast<std::int32_t>(b[3]));
    }

    // ------------------------------------------------------------
    // Called for every incoming binary market‑data packet
    // ------------------------------------------------------------
    void on_market_data(const void* msg, std::size_t len) noexcept {
      // Expected layout: 1‑byte side, 8‑byte price (BE), 4‑byte size (BE)
      if (len < 1 + 8 + 4) {
        return;
      }
      const unsigned char* p = static_cast<const unsigned char*>(msg);
      uint8_t side = *p++; // 0 = bid, 1 = ask
      std::int64_t price = be64toh(p);
      p += 8; // convert BE → host
      std::int32_t size = be32toh(p); // convert BE → host
      price_level lvl{ price, size };

      if (side == 0) {
        if (size == 0) {
          bids_.erase(price);
        }
        else {
          bids_.upsert(lvl);
        }
      }
      else {
        if (size == 0) {
          asks_.erase(price);
        }
        else {
          asks_.upsert(lvl);
        }
      }
    }

    const bid_book& bids() const noexcept {
      return bids_;
    }
    const ask_book& asks() const noexcept {
      return asks_;
    }
  private:
    bid_book bids_;
    ask_book asks_;
};
