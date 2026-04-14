#include "Grid.hpp"

void GridRenderer::create(sf::Vector2f size, float spacing, GridTypes type) {
  body.clear();

  if (type == GridTypes::Lines) {
    body.setPrimitiveType(sf::PrimitiveType::Lines);

    for (auto x = 0.0f; x <= size.x; x += spacing) {
      body.append(sf::Vertex{sf::Vector2f{x, 0.0f}, color});
      body.append(sf::Vertex{sf::Vector2f{x, size.y}, color});
    }

    for (auto y = 0.0f; y <= size.y; y += spacing) {
      body.append(sf::Vertex{sf::Vector2f{0.0f, y}, color});
      body.append(sf::Vertex{sf::Vector2f{size.x, y}, color});
    }
  } else {
    body.setPrimitiveType(sf::PrimitiveType::Points);

    for (auto x = 0.0f; x < size.x; x += spacing) {
      for (auto y = 0.0f; y <= size.y; y += spacing) {
        body.append(sf::Vertex{sf::Vector2f{x, y}, color});
      }
    }
  }
}

sf::Vector2f Grid::snap(sf::Vector2f pos) {
  return sf::Vector2f{std::round(pos.x / CELL_SIZE) * CELL_SIZE,
                      std::round(pos.y / CELL_SIZE) * CELL_SIZE};
}
