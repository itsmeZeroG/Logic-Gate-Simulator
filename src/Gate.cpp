#include "Gate.hpp"

#include <numbers>

#include "Errors.hpp"

sf::ConvexShape GateShapeGenerator::ANDGate(sf::Vector2f size,
                                            int arcPointsCount,
                                            int spinePointsCount) {
  sf::ConvexShape shape;
  shape.setPointCount(arcPointsCount + spinePointsCount);

  shape.setPoint(0, sf::Vector2f{0.0f, 0.0f});

  float arcRadius = size.x / 2.0f;
  sf::Vector2f center = size / 2.0f;

  for (int i = 0; i <= arcPointsCount; i++) {
    float angleRad =
        (-90.0f + (180.0f * i / arcPointsCount)) * std::numbers::pi / 180.0f;

    sf::Vector2f point{center};
    point.x += arcRadius * std::cos(angleRad);
    point.y += arcRadius * std::sin(angleRad);

    shape.setPoint(i + 1, point);
  }

  shape.setPoint(arcPointsCount + 1, sf::Vector2f{0, size.y});

  shape.setFillColor(sf::Color{40u, 40u, 40u});

  shape.setOutlineThickness(2.0f);
  shape.setOutlineColor(sf::Color::White);
  shape.setOrigin(size / 2.0f);

  return shape;
}

GateRenderer::GateRenderer(sf::ConvexShape shape) { body = shape; }

void GateRenderer::setPosition(float x, float y) {
  body.setPosition(sf::Vector2f{x, y});
}

sf::Vector2f GateRenderer::getPosition() const { return body.getPosition(); }

void GateRenderer::update(GateLogic* logic) {
  switch (logic->outputPins[0].state) {
    case PinState::High:
      body.setFillColor(sf::Color::Green);
      break;

    case PinState::Low:
      body.setFillColor(sf::Color::Red);
      break;

    case PinState::Floating:
      body.setFillColor(sf::Color{128u, 128u, 128u});
      break;

    case PinState::Error:
      body.setFillColor(sf::Color::Magenta);
      break;
  }
}

GateLogic::GateLogic(size_t inputCount, size_t outputCount) {
  inputPins.reserve(inputCount);
  outputPins.reserve(outputCount);

  for (size_t i = 0z; i < inputCount; i++)
    inputPins.emplace_back(PinType::Input);
  for (size_t i = 0z; i < outputCount; i++)
    outputPins.emplace_back(PinType::Output);
}
