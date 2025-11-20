module;
#include <print>
export module main;
import common;
extern "C++" int main() {
  common::id::Order orderId = {};
  orderId.id = 123456789;
	common::Price price = {};
	price.id = 100500;
  std::println("Order ID: {}", common::id::Order::stringify(orderId));
  std::println("hello world");
  return 0;
}
