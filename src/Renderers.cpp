#include "Renderers.hpp"

template <typename S>
void Renderer<S>::draw(sf::RenderWindow& window) {
  window.draw(body);
}
