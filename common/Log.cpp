module;
#include <string>
#include <fstream>
#include <cstdio>
#include "./macro.h"
export module Log;
import Queue;
export class Log {
    static constexpr size_t QUEUE_SIZE = 8 * 1024 * 1024;
    enum class Type : int8_t {
      CHAR = 0,
      INTEGER = 1,
      LONG_INTEGER = 2,
      LONG_LONG_INTEGER = 3,
      UNSIGNED_INTEGER = 4,
      UNSIGNED_LONG_INTEGER = 5,
      UNSIGNED_LONG_LONG_INTEGER = 6,
      FLOAT = 7,
      DOUBLE = 8
    };
    struct Element {
        Type type_ = Type::CHAR;
        union {
            char c;
            int i;
            long l;
            long long ll;
            unsigned u;
            unsigned long ul;
            unsigned long long ull;
            float f;
            double d;
        } u_;
    };
    COMMON_MACRO_DELETE_CONSTRUCTOR(Log)
  public:
    auto flushQueue() noexcept {
      while (running_) {
        for (auto next = queue_.getNextToRead(); queue_.size() && next;
             next = queue_.getNextToRead()) {
          switch (next->type_) {
            case Type::CHAR:
              file_ << next->u_.c;
              break;
            case Type::INTEGER:
              file_ << next->u_.i;
              break;
            case Type::LONG_INTEGER:
              file_ << next->u_.l;
              break;
            case Type::LONG_LONG_INTEGER:
              file_ << next->u_.ll;
              break;
            case Type::UNSIGNED_INTEGER:
              file_ << next->u_.u;
              break;
            case Type::UNSIGNED_LONG_INTEGER:
              file_ << next->u_.ul;
              break;
            case Type::UNSIGNED_LONG_LONG_INTEGER:
              file_ << next->u_.ull;
              break;
            case Type::FLOAT:
              file_ << next->u_.f;
              break;
            case Type::DOUBLE:
              file_ << next->u_.d;
              break;
          }
          queue_.updateReadIndex();
        }
        file_.flush();
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(10ms);
      }
    }
    explicit Log(const std::string& file_name)
      : file_name_(file_name)
      , queue_(LOG_QUEUE_SIZE) {
      file_.open(file_name);
      ASSERT(file_.is_open(), "Could not open log file:" + file_name);
      logger_thread_ = createAndStartThread(-1, "Common/Logger " + file_name_, [this]() {
        flushQueue();
      });
      ASSERT(logger_thread_ != nullptr, "Failed to start Logger thread.");
    }
    ~Log() {
      std::string time_str;
      std::cerr << Common::getCurrentTimeStr(&time_str)
                << " Flushing and closing Logger for " << file_name_ << std::endl;
      while (queue_.size()) {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
      }
      running_ = false;
      logger_thread_->join();
      file_.close();
      std::cerr << Common::getCurrentTimeStr(&time_str) << " Logger for " << file_name_
                << " exiting." << std::endl;
    }
    auto pushValue(const Element& log_element) noexcept {
      *(queue_.getNextToWriteTo()) = log_element;
      queue_.updateWriteIndex();
    }
    auto pushValue(const char value) noexcept {
      pushValue(Element{ Type::CHAR, { .c = value } });
    }
    auto pushValue(const int value) noexcept {
      pushValue(Element{ Type::INTEGER, { .i = value } });
    }
    auto pushValue(const long value) noexcept {
      pushValue(Element{ Type::LONG_INTEGER, { .l = value } });
    }
    auto pushValue(const long long value) noexcept {
      pushValue(Element{ Type::LONG_LONG_INTEGER, { .ll = value } });
    }
    auto pushValue(const unsigned value) noexcept {
      pushValue(Element{ Type::UNSIGNED_INTEGER, { .u = value } });
    }
    auto pushValue(const unsigned long value) noexcept {
      pushValue(Element{ Type::UNSIGNED_LONG_INTEGER, { .ul = value } });
    }
    auto pushValue(const unsigned long long value) noexcept {
      pushValue(Element{ Type::UNSIGNED_LONG_LONG_INTEGER, { .ull = value } });
    }
    auto pushValue(const float value) noexcept {
      pushValue(Element{ Type::FLOAT, { .f = value } });
    }
    auto pushValue(const double value) noexcept {
      pushValue(Element{ Type::DOUBLE, { .d = value } });
    }
    auto pushValue(const char* value) noexcept {
      while (*value) {
        pushValue(*value);
        ++value;
      }
    }
    auto pushValue(const std::string& value) noexcept {
      pushValue(value.c_str());
    }
    template<typename T, typename... A>
    auto log(const char* s, const T& value, A... args) noexcept {
      while (*s) {
        if (*s == '%') {
          if (UNLIKELY(*(s + 1) == '%')) { // to allow %% -> % escape character.
            ++s;
          }
          else {
            pushValue(value); // substitute % with the value specified in the arguments.
            log(s + 1, args...); // pop an argument and call self recursively.
            return;
          }
        }
        pushValue(*s++);
      }
      FATAL("extra arguments provided to log()");
    }
    // note that this is overloading not specialization. gcc does not allow inline specializations.
    auto log(const char* s) noexcept {
      while (*s) {
        if (*s == '%') {
          if (UNLIKELY(*(s + 1) == '%')) { // to allow %% -> % escape character.
            ++s;
          }
          else {
            FATAL("missing arguments to log()");
          }
        }
        pushValue(*s++);
      }
    }
  private:
    const std::string file_name_;
    std::ofstream file_;
    Queue<Element> queue_;
    std::atomic<bool> running_ = { true };
    std::thread* logger_thread_ = nullptr;
};
