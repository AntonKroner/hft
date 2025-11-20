#pragma once
#define COMMON_MACRO_DELETE_CONSTRUCTOR(CLASS) \
  CLASS() = delete;                            \
  CLASS(const CLASS&) = delete;                \
  CLASS(const CLASS&&) = delete;               \
  CLASS& operator=(const CLASS&) = delete;     \
  CLASS& operator=(const CLASS&&) = delete;
