module;
#include "../common/thread_utils.hpp"
#include "../common/time_utils.hpp"
#include "../common/macro.hpp"
export module MatchingEngine;
import common;
import Request;
import Response;
import MarketUpdate;
// import OrderBook;
export class MatchingEngine final {
  private:
    // OrderBookHashMap ticker_order_book_;
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
      , logger_("logs/exchange_matching_engine.log") {
      // for (size_t i = 0; i < this->ticker_order_book_.size(); ++i) {
      //   this->ticker_order_book_[i] = new OrderBook(i, &this->logger_, this);
      // }
    }
    COMMON_MACRO_DELETE_CONSTRUCTOR(MatchingEngine)
    ~MatchingEngine() {
      this->run_ = false;
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(1s);
      this->incoming_requests_ = nullptr;
      this->outgoing_ogw_responses_ = nullptr;
      this->outgoing_md_updates_ = nullptr;
      // for (auto& order_book: this->ticker_order_book_) {
      //   delete order_book;
      //   order_book = nullptr;
      // }
    }

    auto processClientRequest(const Request* request) noexcept {
      // auto order_book = this->ticker_order_book_[request->ticker_id_];
      // switch (request->type_) {
      //   case Request::Type::NEW:
      //     order_book->add(
      //       request->client_id_,
      //       request->order_id_,
      //       request->ticker_id_,
      //       request->side_,
      //       request->price_,
      //       request->qty_);
      //     break;
      //   case Request::Type::CANCEL:
      //     order_book->cancel(request->client_id_, request->order_id_, request->ticker_id_);
      //     break;
      //   default:
      //     FATAL(
      //       "Received invalid client-request-type:"
      //       + Request::stringifyType(request->type_));
      //     break;
      // }
    }
    auto sendClientResponse(const Response* response) noexcept {
      this->logger_.log(
        "%:% %() % Sending %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&this->time_str_),
        response->toString());
      auto next_write = this->outgoing_ogw_responses_->getNextToWriteTo();
      *next_write = std::move(*response);
      this->outgoing_ogw_responses_->updateWriteIndex();
    }
    auto sendMarketUpdate(const MarketUpdate* update) noexcept {
      this->logger_.log(
        "%:% %() % Sending %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&this->time_str_),
        update->toString());
      auto next_write = this->outgoing_md_updates_->getNextToWriteTo();
      *next_write = *update;
      this->outgoing_md_updates_->updateWriteIndex();
    }
    auto run() noexcept {
      this->logger_.log(
        "%:% %() %\n",
        __FILE__,
        __LINE__,
        __FUNCTION__,
        Common::getCurrentTimeStr(&this->time_str_));
      while (run_) {
        const auto request = incoming_requests_->getNextToRead();
        if (LIKELY(request)) {
          logger_.log(
            "%:% %() % Processing %\n",
            __FILE__,
            __LINE__,
            __FUNCTION__,
            Common::getCurrentTimeStr(&this->time_str_),
            request->toString());
          this->processClientRequest(request);
          this->incoming_requests_->updateReadIndex();
        }
      }
    }
    auto start() -> void {
      run_ = true;
      ASSERT(
        Common::createAndStartThread(
          -1,
          "Exchange/MatchingEngine",
          [this]() { this->run(); })
          != nullptr,
        "Failed to start MatchingEngine thread.");
    }
    auto stop() -> void {
      run_ = false;
    }
};
