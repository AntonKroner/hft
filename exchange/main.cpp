module;
#include <csignal>
#include <thread>
#include "../common/time_utils.hpp"
export module main;
import common;
import MatchingEngine;
import Request;
import Response;
import MarketUpdate;
common::Log* logger = nullptr;
MatchingEngine* matching_engine = nullptr;
void signal_handler(int) {
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(10s);
  delete logger;
  logger = nullptr;
  delete matching_engine;
  matching_engine = nullptr;
  std::this_thread::sleep_for(10s);
  exit(EXIT_SUCCESS);
}
export extern "C++" int main(int, char**) {
  logger = new common::Log("logs/exchange_main.log");
  std::signal(SIGINT, signal_handler);
  const int sleep_time = 100 * 1000;
  common::Queue<Request> requests(common::ME_MAX_CLIENT_UPDATES);
  common::Queue<Response> responses(common::ME_MAX_CLIENT_UPDATES);
  common::Queue<MarketUpdate> updates(common::ME_MAX_MARKET_UPDATES);
  std::string time_str;
  logger->log(
    "%:% %() % Starting Matching Engine...\n",
    __FILE__,
    __LINE__,
    __FUNCTION__,
    Common::getCurrentTimeStr(&time_str));
  matching_engine = new MatchingEngine(&requests, &responses, &updates);
  matching_engine->start();
  while (true) {
    logger->log(
      "%:% %() % Sleeping for a few milliseconds..\n",
      __FILE__,
      __LINE__,
      __FUNCTION__,
      Common::getCurrentTimeStr(&time_str));
    usleep(sleep_time * 1000);
  }
}
