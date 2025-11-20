module;
#include <cstdint>
export module Quantity;
import id;
export struct Quantity: public id::Base<std::uint32_t> { };
