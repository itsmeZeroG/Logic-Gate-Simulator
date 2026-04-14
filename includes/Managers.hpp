#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

template <typename T>
class Manager {
 protected:
  std::vector<std::unique_ptr<T>> elements;

 public:
  void update();
  void draw(sf::RenderWindow& window) {
    for (auto& e : elements) e.draw();
  }
};
