#pragma once

#include <SFML/Graphics.hpp>

#include "Pin.hpp"
#include "Renderers.hpp"

class GateShapeGenerator {
 public:
  static sf::ConvexShape ANDGate(sf::Vector2f size, int arcPointsCount = 30,
                                 int spinePointsCount = 2);
};

class GateRenderer : public ConvexRenderer {
 public:
  GateRenderer(sf::ConvexShape shape);

  void setPosition(float x, float y);
  sf::Vector2f getPosition() const;

  virtual void update(GateLogic* logic);
};

class GateLogic {
 public:
  std::vector<Pin> inputPins;
  std::vector<Pin> outputPins;

  GateLogic(size_t inputCount = 1z, size_t outputCount = 1z);
  virtual void update() = 0;
};
