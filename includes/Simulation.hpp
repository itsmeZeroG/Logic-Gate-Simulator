#pragma once

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

      for (size_t i = 0; i < previewWireVertices.getVertexCount(); i++)
        previewWireVertices[i].position = sf::Vector2f{0.0f, 0.0f};

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
      } else if (const auto* mouseButtonPressed =
                     event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
          auto gate = gatesManager.getGateAt(
              window.mapPixelToCoords(mouseButtonPressed->position));
          if (auto* inputButton = dynamic_cast<InputButton*>(gate)) {
            inputButton->toggle();
          }
        }
      } else if (const auto* keyPressed =
                     event->getIf<sf::Event::KeyPressed>()) {
        sf::Vector2f mousePos =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        switch (keyPressed->scancode) {
          case sf::Keyboard::Scancode::Num1:
            gatesManager.create<InputButton>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num2:
            gatesManager.create<NotGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num3:
            gatesManager.create<AndGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num4:
            gatesManager.create<OrGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num5:
            gatesManager.create<NandGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num6:
            gatesManager.create<NorGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num7:
            gatesManager.create<XorGate>(mousePos);
            break;

          case sf::Keyboard::Scancode::Num8:
            gatesManager.create<XnorGate>(mousePos);
            break;
        }
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
