#include <SFML/Graphics.hpp>

class Grid {
 private:
  sf::VertexArray vertices;

 public:
  Grid(float gridCellSize, sf::Vector2u windowSize) {
    vertices.setPrimitiveType(sf::PrimitiveType::Lines);
    vertices.clear();

    sf::Color gridColor{50, 50, 50};

    for (float x = 0; x < windowSize.x; x += gridCellSize) {
      vertices.append(sf::Vertex({x, 0}, gridColor));
      vertices.append(sf::Vertex({x, windowSize.y}, gridColor));
    }

    for (float y = 0; y < windowSize.y; y += gridCellSize) {
      vertices.append(sf::Vertex({0, y}, gridColor));
      vertices.append(sf::Vertex({windowSize.x, y}, gridColor));
    }
  }

  void render(sf::RenderWindow& window) { window.draw(vertices); }
};

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
  Grid grid;

  Simulation(sf::Vector2u windowSize)
      : window{sf::VideoMode(windowSize), "Logic Gates Simulation"},
        grid{20, windowSize} {
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
    window.clear(sf::Color{18, 18, 18});

    grid.render(window);
    gateManager.render(window);

    window.display();
  }
};

int main() {
  Simulation sim(sf::Vector2u{800, 400});
  sim.start();

  return 0;
}
