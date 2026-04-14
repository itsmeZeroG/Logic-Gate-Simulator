#include "Wire.hpp"

void WireRenderer::createPath(sf::Vector2f startPoint, sf::Vector2f endPoint,
                              WirePathingTypes type) {
  body.clear();
  body.setPrimitiveType(sf::PrimitiveType::LineStrip);

  if (type == WirePathingTypes::ManhattanRouting) {
    float midX = startPoint.x + (endPoint.x - startPoint.x) / 2.0f;

    body.append(sf::Vertex{startPoint, color});
    body.append(sf::Vertex{sf::Vector2f{midX, startPoint.y}, color});
    body.append(sf::Vertex{sf::Vector2f{midX, endPoint.y}, color});
    body.append(sf::Vertex{endPoint, color});
  }
}

Wire::Wire(Pin* pinStart, Pin* pinEnd) : startPin{pinStart}, endPin{pinEnd} {}

void Wire::update() {
  if (startPin == nullptr || endPin == nullptr) return;

  endPin->state = startPin->state;
}
