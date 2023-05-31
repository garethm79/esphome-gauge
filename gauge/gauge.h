#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/color.h"
#include "esphome/core/component.h"

namespace esphome {

// forward declare DisplayBuffer
namespace display {
class DisplayBuffer;
class Font;
}  // namespace display

namespace gauge {

class Gauge;

const Color COLOR_ON(255, 255, 255, 255);

enum DirectionType {
  DIRECTION_TYPE_AUTO,
  DIRECTION_TYPE_HORIZONTAL,
  DIRECTION_TYPE_VERTICAL,
};

enum ValuePositionType {
  VALUE_POSITION_TYPE_NONE,
  VALUE_POSITION_TYPE_AUTO,
  VALUE_POSITION_TYPE_BESIDE,
  VALUE_POSITION_TYPE_BELOW
};

class Gauge : public Component {
 public:
  //void draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color);
  void draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color start, Color mid, Color end, Color legend);

  void setup() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  void dump_config() override;
  std::string get_name() { return name_; }

  void set_sensor(sensor::Sensor *sensor);
  void set_width(uint32_t width) { width_ = width; }
  void set_height(uint32_t height) { height_ = height; }
  void set_segments(uint32_t segments) { segments_ = segments; }
  void set_rotation(uint32_t rotation) { rotation_ = rotation; }
  void set_size(uint32_t size) { size_ = size; }
  void set_min_value(float val) { this->min_value_ = val; }
  void set_max_value(float val) { this->max_value_ = val; }
  void set_border(bool val) { this->border_ = val; }
  void set_value_font(display::Font *font) { this->font_value_ = font; }
  void set_name_font(display::Font *font) { this->font_name_ = font; }

  uint32_t get_width() { return width_; }
  uint32_t get_height() { return height_; }

 protected:
  uint32_t width_;     /// in pixels
  uint32_t height_;    /// in pixels
  uint32_t segments_;    /// 
  uint32_t rotation_;    /// in degrees
  uint32_t size_;    /// in degrees
  float min_value_{NAN};
  float max_value_{NAN};
  float min_range_{1.0};
  float max_range_{NAN};
  float gridspacing_y_{NAN};
  display::Font *font_value_{nullptr};
  display::Font *font_name_{nullptr};
  
  Color interpolate_color(Color color1, Color color2, float position);
  Color value_to_rgb(Color start, Color mid, Color end, float position);
  
  bool border_{true};

  std::string name_{""};
  sensor::Sensor *sensor_{nullptr};
  float dataval;

};

}  // namespace Gauge
}  // namespace esphome
