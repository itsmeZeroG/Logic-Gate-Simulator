#include "Errors.hpp"

std::string PinOutOfBoundsError::pinTypeLabel(PinType type) {
  switch (type) {
    case PinType::Input:
      return "Input";
    case PinType::Output:
      return "Output";
    default:
      return "Unknown";
  }
}

PinOutOfBoundsError::PinOutOfBoundsError(PinType type, size_t index,
                                         size_t maxLimit)
    : std::out_of_range(
          pinTypeLabel(type) + " Pin Index " + std::to_string(index) +
          " is out of bounds (Max: " + std::to_string(maxLimit - 1) + ")") {}
