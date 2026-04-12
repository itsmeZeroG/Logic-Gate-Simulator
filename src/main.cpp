#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

class Grid {
 private:
  sf::VertexArray vertices;

 public:
  static constexpr float CELL_SIZE{30.0f};
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

enum class PinType : uint8_t { Input, Output };
class Pin {
 public:
  bool state{false};
  PinType type;

  sf::CircleShape body{Grid::CELL_SIZE / 4.0f};

  Pin(PinType pinType) : type{pinType} {}

  void render(sf::RenderWindow& window) {
    body.setFillColor(sf::Color::Blue);

    window.draw(body);
  }
};

class Wire {
 public:
  Pin* startPin{nullptr};
  Pin* endPin{nullptr};
  sf::VertexArray vertices{sf::PrimitiveType::LineStrip, 4};

  static void calcuateRouting(sf::VertexArray& routingVertices,
                              sf::Vector2f startPos, sf::Vector2f endPos) {
    float midX = startPos.x + (endPos.x - startPos.x) / 2.0f;

    routingVertices[0].position = startPos;
    routingVertices[1].position = sf::Vector2f{midX, startPos.y};
    routingVertices[2].position = sf::Vector2f{midX, endPos.y};
    routingVertices[3].position = endPos;
  }

  Wire(Pin* startingPin, Pin* endingPin)
      : startPin{startingPin}, endPin{endingPin} {}

  void update() {
    if (startPin == nullptr || endPin == nullptr) return;

    endPin->state = startPin->state;
  }

  void render(sf::RenderWindow& window) {
    if (startPin != nullptr && endPin != nullptr) {
      calcuateRouting(vertices, startPin->body.getPosition(),
                      endPin->body.getPosition());

      for (size_t i = 0; i < vertices.getVertexCount(); i++)
        vertices[i].color = sf::Color::Yellow;

      window.draw(vertices);
    }
  }
};

class Gate {
 public:
  sf::RectangleShape body;

  std::vector<Pin> inputPins;
  std::vector<Pin> outputPins;

  uint8_t opacity{255u};

  Gate(size_t inputCount = 1z, size_t outputCount = 1z) {
    inputPins.reserve(inputCount);
    outputPins.reserve(outputCount);

    sf::Vector2f bodyPos = body.getPosition();
    sf::Vector2f bodySize = body.getSize();

    for (size_t i = 0z; i < inputCount; i++)
      inputPins.emplace_back(PinType::Input);
    for (size_t i = 0z; i < outputCount; i++)
      outputPins.emplace_back(PinType::Output);

    body.setSize(sf::Vector2f{Grid::CELL_SIZE * 4.0f, Grid::CELL_SIZE * 3.0f});
    body.setFillColor(sf::Color::Blue);
  }

  virtual void updateLogic() = 0;
  void updatePins() {
    sf::Vector2f bodyPos = body.getPosition();
    sf::Vector2f bodySize = body.getSize();

    for (size_t i = 0z; i < inputPins.capacity(); i++) {
      auto& pin = inputPins[i];

      pin.body.setOrigin(pin.body.getGeometricCenter());

      float posY = bodyPos.y;
      float pinRegionSize = bodySize.y / inputPins.capacity();
      posY += i * pinRegionSize;
      posY += pinRegionSize / 2.0f;

      pin.body.setPosition(sf::Vector2f{bodyPos.x, posY});
    }

    for (size_t i = 0z; i < outputPins.capacity(); i++) {
      auto& pin = outputPins[i];

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
  AndGate() : Gate(2z, 1z) {}

  void updateLogic() override {
    outputPins[0].state = inputPins[0].state && inputPins[1].state;
    body.setFillColor(outputPins[0].state ? sf::Color::Green : sf::Color::Red);
  }
};

class WiresManager {
 public:
  std::vector<std::unique_ptr<Wire>> wires;

  bool connectionExists(Pin* startPin, Pin* endPin) const {
    return std::any_of(
        wires.begin(), wires.end(), [&startPin, &endPin](const auto& w) {
          return ((w->startPin == startPin && w->endPin == endPin) ||
                  (w->startPin == endPin && w->endPin == startPin));
        });
  }

  bool canConnect(Pin* startPin, Pin* endPin) const {
    return startPin != endPin && startPin->type != endPin->type;
  }

  void createWire(Pin* startPin, Pin* endPin) {
    auto wire = std::make_unique<Wire>(startPin, endPin);
    wires.push_back(std::move(wire));
  }

  void update() {
    for (auto& w : wires) w->update();
  }

  void render(sf::RenderWindow& window) {
    for (auto& w : wires) w->render(window);
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
    for (const auto& g : gates)
      if (g->body.getGlobalBounds().contains(pos)) return g.get();

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
    for (auto& g : gates) g->update();
  }

  void render(sf::RenderWindow& window) {
    for (auto& g : gates) g->render(window);
  }
};

class Simulation {
 public:
  GatesManager gatesManager;
  WiresManager wiresManager;
  sf::RenderWindow window;
  Grid grid;

  bool rmbPressed{false};
  bool lmbPressed{false};

  Gate* selectedGate{nullptr};
  sf::Vector2f dragOffset{0.0f, 0.0f};

  Pin* selectedPin{nullptr};
  sf::VertexArray previewWireVertices{sf::PrimitiveType::LineStrip, 4};

  Simulation(sf::Vector2u windowSize)
      : window{sf::VideoMode(windowSize), "Logic Gates Simulation"},
        grid{static_cast<sf::Vector2f>(windowSize)} {
    window.setFramerateLimit(30u);

    sf::Vector2f center{windowSize.x / 2.0f, windowSize.y / 2.0f};
    center.x -= 100.0f;
    center.y -= 50.0f;

    sf::Vector2f gate1Pos{center};
    gate1Pos.x -= 100.0f;
    gatesManager.createAndGate(gate1Pos);

    sf::Vector2f gate2Pos{center};
    gate2Pos.x += 100.0f;
    gatesManager.createAndGate(gate2Pos);
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
      selectedGate = gatesManager.getGateAt(mousePos);
      if (selectedGate == nullptr) return;

      // Just selected a gate
      selectedGate->opacity = 128u;
      dragOffset = mousePos - selectedGate->body.getPosition();
    }

    selectedGate->body.setPosition(mousePos - dragOffset);
  }

  void handleWireDragging() {
    lmbPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (!lmbPressed) {
      if (selectedPin == nullptr) return;

      // Just released a pin
      auto endPin = gatesManager.getPinAt(mousePos);
      if (endPin != nullptr && wiresManager.canConnect(selectedPin, endPin) &&
          !wiresManager.connectionExists(selectedPin, endPin))
        wiresManager.createWire(selectedPin, endPin);

      selectedPin = nullptr;

      return;
    };

    if (selectedPin == nullptr) {
      selectedPin = gatesManager.getPinAt(mousePos);
      return;
    }

    Wire::calcuateRouting(previewWireVertices, selectedPin->body.getPosition(),
                          mousePos);

    for (size_t i = 0; i < previewWireVertices.getVertexCount(); i++)
      previewWireVertices[i].color = sf::Color::Yellow;
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
    handleWireDragging();
    wiresManager.update();
    gatesManager.update();
  }

  void render() {
    window.clear(sf::Color{18u, 18u, 18u});

    grid.render(window);
    wiresManager.render(window);
    if (selectedPin != nullptr) window.draw(previewWireVertices);
    gatesManager.render(window);

    window.display();
  }
};

int main() {
  Simulation sim(sf::Vector2u{800u, 400u});
  sim.start();

  return 0;
}
