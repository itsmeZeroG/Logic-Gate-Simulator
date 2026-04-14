#include "Controllers.hpp"

void GateController::update() {
  logic->update();
  renderer->update(logic.get());
}

void GateController::draw(sf::RenderWindow& window) { renderer->draw(window); }
