#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <vector>

class Grid {
 private:
  sf::VertexArray vertices;

 public:
  static constexpr float CELL_SIZE{20.0f};
  static sf::Vector2f snapPoint(sf::Vector2f pos) {
    return sf::Vector2f{std::round(pos.x / CELL_SIZE) * CELL_SIZE,
                        std::round(pos.y / CELL_SIZE) * CELL_SIZE};
  }

  Grid(sf::Vector2f windowSize) {
    vertices.setPrimitiveType(sf::PrimitiveType::Lines);
    vertices.clear();

    sf::Color gridColor{36, 36, 36};

    for (float x = 0; x < windowSize.x; x += CELL_SIZE) {
      vertices.append(sf::Vertex{sf::Vector2f{x, 0}, gridColor});
      vertices.append(sf::Vertex{sf::Vector2f{x, windowSize.y}, gridColor});
    }

    for (float y = 0; y < windowSize.y; y += CELL_SIZE) {
      vertices.append(sf::Vertex{sf::Vector2f{0, y}, gridColor});
      vertices.append(sf::Vertex{sf::Vector2f{windowSize.x, y}, gridColor});
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

  Gate() { body.setSize({Grid::CELL_SIZE * 4, Grid::CELL_SIZE * 3}); }

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
    gate->body.setPosition(Grid::snapPoint(position));
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
        grid{static_cast<sf::Vector2f>(windowSize)} {
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
  Simulation sim({800, 400});
  sim.start();

  return 0;
}
