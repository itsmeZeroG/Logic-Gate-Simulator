#include <algorithm>

#include "Controllers.hpp"
#include "Grid.hpp"

class AndGate : public GateController {
 public:
  AndGate()
      : renderer{GateShapeGenerator::ANDGate(
            sf::Vector2f{Grid::CELL_SIZE * 3, Grid::CELL_SIZE * 3})} {}
};
