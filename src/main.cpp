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

  unsigned char opacity = 255;

  Gate() {
    body.setSize({Grid::CELL_SIZE * 4, Grid::CELL_SIZE * 3});
    body.setFillColor(sf::Color::Blue);
  }

  virtual void update() = 0;

  void render(sf::RenderWindow& window) {
    sf::Color bodyColor = body.getFillColor();
    bodyColor.a = opacity;

    body.setFillColor(bodyColor);

    window.draw(sf::RectangleShape(body));
  }
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

  Gate* getGateAt(sf::Vector2f pos) {
    for (const auto& g : gates) {
      if (g->body.getGlobalBounds().contains(pos)) {
        return g.get();
      }
    }

    return nullptr;
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

  bool rmbPressed = false;
  Gate* selectedGate = nullptr;
  sf::Vector2f dragOffset{0.0f, 0.0f};

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

  void handleGateMove() {
    rmbPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    if (!rmbPressed) {
      if (selectedGate == nullptr) return;

      // Just released a gate
      selectedGate->body.setPosition(
          Grid::snapPoint(selectedGate->body.getPosition()));

      selectedGate->opacity = 255;
      dragOffset = sf::Vector2f{0.0f, 0.0f};
      selectedGate = nullptr;

      return;
    };

    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (selectedGate == nullptr) {
      selectedGate = gateManager.getGateAt(mousePos);
      if (selectedGate == nullptr) return;

      // Just selected a gate
      selectedGate->opacity = 128;
      dragOffset = mousePos - selectedGate->body.getPosition();
    }

    selectedGate->body.setPosition(mousePos - dragOffset);
  }

  void handleEvents() {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        exit();
      }
    }
  }

  void update() {
    handleGateMove();
    gateManager.update();
  }

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
