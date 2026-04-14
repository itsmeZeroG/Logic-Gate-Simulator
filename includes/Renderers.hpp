#pragma once

#include <SFML/Graphics.hpp>

template <typename S>
class Renderer {
 protected:
  S body;

 public:
  virtual void draw(sf::RenderWindow& window);
};

class CircleRenderer : public Renderer<sf::CircleShape> {};

class ConvexRenderer : public Renderer<sf::ConvexShape> {};

class VertexArrayRenderer : public Renderer<sf::VertexArray> {};
