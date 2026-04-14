#pragma once

#include <SFML/Graphics.hpp>

#include "Renderers.hpp"

enum class GridTypes : uint8_t { Lines, Points };

class GridRenderer : public VertexArrayRenderer {
 public:
  sf::Color color;

  void create(sf::Vector2f size, float spacing, GridTypes type);
};

class Grid {
 public:
  static constexpr float CELL_SIZE{30.0f};

  static sf::Vector2f snap(sf::Vector2f pos);
};
