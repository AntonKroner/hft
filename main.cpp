module;
#include <cstdio>
export module main;
import common;
export extern "C++" int main() {
  common::id::Order orderId = 123456789;
  common::Price price = 100500;
  std::printf("Order ID: %s\n", common::id::stringify(orderId).c_str());
  std::printf("hello world\n");
  return 0;
}
