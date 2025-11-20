module;
#include <cstdint>
export module Priority;
import id;
export struct Priority: public id::Base<std::uint64_t> { };
