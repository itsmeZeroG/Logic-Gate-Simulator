#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "Gate.hpp"

class GateController {
 private:
  std::unique_ptr<GateLogic> logic;
  std::unique_ptr<GateRenderer> renderer;

 public:
  void update();
  void draw(sf::RenderWindow& window);
};
