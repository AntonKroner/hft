module;
#include <cstdint>
export module Price;
import id;
export struct Price: public id::Base<std::int64_t> { };
