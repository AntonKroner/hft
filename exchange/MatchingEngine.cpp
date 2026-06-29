module;
#include "../common/thread_utils.hpp"
#include "../common/time_utils.hpp"
#include "../common/macro.hpp"
export module MatchingEngine;
import common;
import Request;
import Response;
import MarketUpdate;
import OrderBook;
export class MatchingEngine final {
  private:
    OrderBookHashMap ticker_order_book_;
    common::Queue<Request>* incoming_requests_ = nullptr;
    common::Queue<Response>* outgoing_ogw_responses_ = nullptr;
    common::Queue<MarketUpdate>* outgoing_md_updates_ = nullptr;
    volatile bool run_ = false;
    std::string time_str_;
    common::Log logger_;
  public:
    MatchingEngine(
      common::Queue<Request>* requests,
      common::Queue<Response>* responses,
      common::Queue<MarketUpdate>* updates)
      : incoming_requests_(requests)
      , outgoing_ogw_responses_(responses)
      , outgoing_md_updates_(updates)
      , logger_("exchange_matching_engine.log") {
      for (size_t i = 0; i < ticker_order_book_.size(); ++i) {
        ticker_order_book_[i] = new MEOrderBook(i, &logger_, this);
      }
    }
    ~MatchingEngine() {
      run_ = false;
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(1s);
      incoming_requests_ = nullptr;
      outgoing_ogw_responses_ = nullptr;
      outgoing_md_updates_ = nullptr;
      for (auto& order_book: ticker_order_book_) {
        delete order_book;
        order_book = nullptr;
      }
    }
    auto start() -> void {
      run_ = true;
      ASSERT(
        Common::createAndStartThread(-1, "Exchange/MatchingEngine", [this]() { run(); })
          != nullptr,
        "Failed to start MatchingEngine thread.");
    }
    auto stop() -> void {
      run_ = false;
    }
    auto processClientRequest(const Request* request) noexcept {
      auto order_book = ticker_order_book_[request->ticker_id_];
      switch (request->type_) {
        case Request::Type::NEW:
          order_book->add(
            request->client_id_,
            request->order_id_,
            request->ticker_id_,
            request->side_,
            request->price_,
            request->qty_);
          break;
        case Request::Type::CANCEL:
          order_book->cancel(request->client_id_, request->order_id_, request->ticker_id_);
          break;
        default:
          FATAL(
            "Received invalid client-request-type:"
            + Request::stringifyType(request->type_));
          break;
      }
    }
    auto sendClientResponse(const Response* client_response) noexcept {
      logger_.log(
        "%:% %() % Sending %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&time_str_),
        client_response->toString());
      auto next_write = outgoing_ogw_responses_->getNextToWriteTo();
      *next_write = std::move(*client_response);
      outgoing_ogw_responses_->updateWriteIndex();
    }
    auto sendMarketUpdate(const MarketUpdate* market_update) noexcept {
      logger_.log(
        "%:% %() % Sending %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&time_str_),
        market_update->toString());
      auto next_write = outgoing_md_updates_->getNextToWriteTo();
      *next_write = *market_update;
      outgoing_md_updates_->updateWriteIndex();
    }
    auto run() noexcept {
      logger_.log(
        "%:% %() %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&time_str_));
      while (run_) {
        const auto me_client_request = incoming_requests_->getNextToRead();
        if (LIKELY(me_client_request)) {
          logger_.log(
            "%:% %() % Processing %\n",
            __FILE__,
            __LINE__,
            __FUNCTION__,
            Common::getCurrentTimeStr(&time_str_),
            me_client_request->toString());
          processClientRequest(me_client_request);
          incoming_requests_->updateReadIndex();
        }
      }
    }
    COMMON_MACRO_DELETE_CONSTRUCTOR(MatchingEngine)
};
// MatchingEngine::MatchingEngine(
//   ClientRequestLFQueue* requests,
//   ClientResponseLFQueue* responses,
//   MEMarketUpdateLFQueue* updates)
//   : incoming_requests_(requests)
//   , outgoing_ogw_responses_(responses)
//   , outgoing_md_updates_(updates)
//   , logger_("exchange_matching_engine.log") {
//   for (size_t i = 0; i < ticker_order_book_.size(); ++i) {
//     ticker_order_book_[i] = new MEOrderBook(i, &logger_, this);
//   }
// }
// MatchingEngine::~MatchingEngine() {
//   run_ = false;
//   using namespace std::literals::chrono_literals;
//   std::this_thread::sleep_for(1s);
//   incoming_requests_ = nullptr;
//   outgoing_ogw_responses_ = nullptr;
//   outgoing_md_updates_ = nullptr;
//   for (auto& order_book: ticker_order_book_) {
//     delete order_book;
//     order_book = nullptr;
//   }
// }
