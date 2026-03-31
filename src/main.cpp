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

    sf::Color gridColor{36u, 36u, 36u};

    for (auto x = 0.0f; x < windowSize.x; x += CELL_SIZE) {
      vertices.append(sf::Vertex{sf::Vector2f{x, 0.0f}, gridColor});
      vertices.append(sf::Vertex{sf::Vector2f{x, windowSize.y}, gridColor});
    }

    for (auto y = 0.0f; y < windowSize.y; y += CELL_SIZE) {
      vertices.append(sf::Vertex{sf::Vector2f{0.0f, y}, gridColor});
      vertices.append(sf::Vertex{sf::Vector2f{windowSize.x, y}, gridColor});
    }
  }

  void render(sf::RenderWindow& window) { window.draw(vertices); }
};

class Gate;
class Pin {
 public:
  Gate* parentGate{nullptr};
  bool state{false};

  sf::CircleShape body{5.0f};  // TODO Try different point count

  void render(sf::RenderWindow& window) {
    body.setFillColor(sf::Color::Blue);

    window.draw(body);
  }
};

class Gate {
 public:
  sf::RectangleShape body;

  std::vector<Pin> inputPins;
  std::vector<Pin> outputPins;

  unsigned char opacity{255u};

  Gate(unsigned int inputCount = 1u, unsigned int outputCount = 1u) {
    inputPins.reserve(inputCount);
    outputPins.reserve(outputCount);

    sf::Vector2f bodyPos = body.getPosition();
    sf::Vector2f bodySize = body.getSize();

    for (auto i = 0u; i < inputCount; i++) inputPins.emplace_back(this);
    for (auto i = 0u; i < outputCount; i++) outputPins.emplace_back(this);

    body.setSize(sf::Vector2f{Grid::CELL_SIZE * 4.0f, Grid::CELL_SIZE * 3.0f});
    body.setFillColor(sf::Color::Blue);
  }

  virtual void updateLogic() = 0;
  void updatePins() {
    sf::Vector2f bodyPos = body.getPosition();
    sf::Vector2f bodySize = body.getSize();

    for (size_t i = 0u; i < inputPins.capacity(); i++) {
      auto& pin = inputPins[i];

      pin.body.setOrigin(pin.body.getGeometricCenter());

      float posY = bodyPos.y;
      float pinRegionSize = bodySize.y / inputPins.capacity();
      posY += i * pinRegionSize;
      posY += pinRegionSize / 2.0f;

      pin.body.setPosition(sf::Vector2f{bodyPos.x, posY});
    }

    for (size_t i = 0u; i < outputPins.capacity(); i++) {
      auto& pin{outputPins[i]};

      pin.body.setOrigin(pin.body.getGeometricCenter());

      float posY = bodyPos.y;
      float pinRegionSize = bodySize.y / outputPins.capacity();
      posY += i * pinRegionSize;
      posY += pinRegionSize / 2.0f;

      pin.body.setPosition(sf::Vector2f{bodyPos.x + bodySize.x, posY});
    }
  }
  virtual void update() {
    updatePins();
    updateLogic();
  };

  void render(sf::RenderWindow& window) {
    sf::Color bodyColor = body.getFillColor();
    bodyColor.a = opacity;

    body.setFillColor(bodyColor);

    window.draw(body);
    for (auto& pin : inputPins) pin.render(window);
    for (auto& pin : outputPins) pin.render(window);
  }
};

class AndGate : public Gate {
 public:
  AndGate() : Gate(2u, 1u) {}

  void updateLogic() override {
    outputPins[0].state = inputPins[0].state && inputPins[1].state;
    body.setFillColor(outputPins[0].state ? sf::Color::Green : sf::Color::Red);
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

  Pin* getPinAt(sf::Vector2f pos) {
    for (const auto& g : gates) {
      for (auto& pin : g->inputPins)
        if (pin.body.getGlobalBounds().contains(pos)) return &pin;

      for (auto& pin : g->outputPins)
        if (pin.body.getGlobalBounds().contains(pos)) return &pin;
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

  bool rmbPressed{false};
  Gate* selectedGate{nullptr};
  sf::Vector2f dragOffset{0.0f, 0.0f};

  Simulation(sf::Vector2u windowSize)
      : window{sf::VideoMode(windowSize), "Logic Gates Simulation"},
        grid{static_cast<sf::Vector2f>(windowSize)} {
    window.setFramerateLimit(30u);

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

      selectedGate->opacity = 255u;
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
      selectedGate->opacity = 128u;
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
    window.clear(sf::Color{18u, 18u, 18u});

    grid.render(window);
    gateManager.render(window);

    window.display();
  }
};

int main() {
  Simulation sim(sf::Vector2u{800u, 400u});
  sim.start();

  return 0;
}
