#pragma once

#include "Pin.hpp"
#include "Renderers.hpp"

enum class WirePathingTypes : uint8_t { ManhattanRouting };

class WireRenderer : public VertexArrayRenderer {
 public:
  sf::Color color;

  void createPath(sf::Vector2f startPoint, sf::Vector2f endPoint,
                  WirePathingTypes type);
};

class Wire {
 public:
  Pin* startPin{nullptr};
  Pin* endPin{nullptr};

  Wire(Pin* pinStart, Pin* pinEnd);

  void update();
};
