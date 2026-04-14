#pragma once

#include <SFML/Graphics.hpp>

#include "Renderers.hpp"

enum class PinType : uint8_t { Input, Output };

enum class PinState : uint8_t { High, Low, Floating, Error };

class Pin {
 protected:
  PinType type;

 public:
  PinState state{PinState::Floating};

  Pin(PinType pinType);
  PinType getType() const;
};

class PinRenderer : public CircleRenderer {
 public:
  sf::Vector2f offset;
  PinRenderer(float radius);
};
