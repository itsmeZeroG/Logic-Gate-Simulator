#include <SFML/Graphics.hpp>

class Gate {
 public:
  sf::RectangleShape body;
  bool inputA = false;
  bool inputB = false;
  bool output = false;

  Gate() { body.setSize({100, 50}); }

  virtual void update() = 0;

  void render(sf::RenderWindow& window) { window.draw(body); }
};

class AndGate : public Gate {
  void update() override {
    output = inputA && inputB;
    body.setFillColor(output ? sf::Color::Green : sf::Color::Red);
  }
};

class GatesManager {
 public:
  std::vector<std::unique_ptr<Gate>> gates;

  void createAndGate(sf::Vector2f position) {
    auto gate = std::make_unique<AndGate>();
    gate->body.setPosition(position);
    gates.push_back(std::move(gate));
  }

  void update() {
    for (auto& g : gates) {
      g->update();
    }
  }

  void render(sf::RenderWindow& window) {
    for (auto& g : gates) {
      g->render(window);
    }
  }
};

class Simulation {
 public:
  GatesManager gateManager;
  sf::RenderWindow window;

  Simulation(sf::Vector2u windowSize)
      : window(sf::VideoMode(windowSize), "Logic Gates Simulation") {
    window.setFramerateLimit(30);

    sf::Vector2f center{windowSize.x / 2.0f, windowSize.y / 2.0f};
    center.x -= 100.0f;
    center.y -= 50.0f;

    gateManager.createAndGate(center);
  }

  void start() {
    while (window.isOpen()) {
      handleEvents();
      update();
      render();
    }
  }

  void exit() { window.close(); }

  void handleEvents() {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        exit();
      }
    }
  }

  void update() { gateManager.update(); }

  void render() {
    window.clear();

    gateManager.render(window);

    window.display();
  }
};

int main() {
  Simulation sim({800, 400});
  sim.start();

  return 0;
}
