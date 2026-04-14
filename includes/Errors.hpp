#pragma once

#include <stdexcept>

#include "Pin.hpp"

class PinOutOfBoundsError : public std::out_of_range {
 protected:
  static std::string pinTypeLabel(PinType type);

 public:
  explicit PinOutOfBoundsError(PinType type, size_t index, size_t maxLimit);
};
