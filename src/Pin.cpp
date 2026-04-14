#include "Pin.hpp"

Pin::Pin(PinType pinType) : type{pinType} {}

PinType Pin::getType() const { return type; }

PinRenderer::PinRenderer(float radius) { body.setRadius(radius); }
