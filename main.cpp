module;
#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
export module main;
import udp_socket;
import order_book;
import latency;
constexpr unsigned kCoreRx = 2;
constexpr unsigned kCoreTx = 3;
constexpr std::uint16_t kRxPort = 9000;
constexpr std::uint16_t kTxPort = 9100;
constexpr const char* kIfName = "";
constexpr const char* kMcast = nullptr;
udp_socket rx_sock;
udp_socket tx_sock;
order_book ob;
latency timer;
std::atomic<bool> g_run{ true };
void market_thread() {
  rx_sock.open(kIfName, kRxPort);
  rx_sock.pin_to_core(kCoreRx);
  alignas(64) std::uint8_t buf[2048];
  while (g_run.load(std::memory_order_acquire)) {
    std::size_t n = rx_sock.recv(buf, sizeof(buf));
    if (n == 0) {
      continue;
    }
    ob.on_market_data(buf, n);
    const auto* best_bid = ob.bids().best();
    const auto* best_ask = ob.asks().best();
    if (best_bid && best_ask && best_bid->price + 1 < best_ask->price) {
      struct order {
          std::uint8_t side;
          std::int64_t price;
          std::int32_t size;
          std::uint64_t latency_ns;
      };
      static bool flip = false;
      order o{};
      if (flip) {
        o.side = 0;
        o.price = best_bid->price + 1;
      }
      else {
        o.side = 1;
        o.price = best_ask->price - 1;
      }
      flip = !flip;
      o.size = 1;
      o.latency_ns = timer.ns_elapsed(timer.start());
      tx_sock.send(&o, sizeof(o));
    }
  }
  rx_sock.close();
}
void print_loop() {
  while (g_run.load(std::memory_order_acquire)) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const auto* bid = ob.bids().best();
    const auto* ask = ob.asks().best();
    std::cout << "[snapshot] ";
    if (bid) {
      std::cout << "bid " << bid->price << "/" << bid->size << " ";
    }
    else {
      std::cout << "bid N/A ";
    }
    if (ask) {
      std::cout << "ask " << ask->price << "/" << ask->size << "\n";
    }
    else {
      std::cout << "ask N/A\n";
    }
  }
}
int main() {
  std::cout << "=== SteelDragon HFT prototype (C++23 modules) ===\n";
  tx_sock.open(kIfName, kTxPort);
  tx_sock.pin_to_core(kCoreTx);
  std::thread md_thread(market_thread);
  std::thread pr_thread(print_loop);
  std::this_thread::sleep_for(std::chrono::seconds(30));
  g_run.store(false, std::memory_order_release);
  md_thread.join();
  pr_thread.join();
  tx_sock.close();
  std::cout << "Shutdown complete – thank you for flying SteelDragon HFT!\n";
  return 0;
}
