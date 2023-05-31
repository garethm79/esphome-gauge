#include "gauge.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/color.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include <algorithm>
#include <sstream>
#include <iostream>  // std::cout, std::fixed
#include <iomanip>

namespace esphome {

namespace gauge {

using namespace display;

static const char *const TAG = "gauge";

void Gauge::draw(DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color start, Color mid, Color end, Color legend) {
  ESP_LOGD(TAG, "draw");

  Color black = Color(0,0,0);
  Color grey = Color(40,40,40);
  int GaugeDepth = 20;


  float sensor_value = this->sensor_->get_state();

  // TRUE state is defined to be when sensor is >= threshold
  // so when undefined sensor value initialize to FALSE
  if (std::isnan(sensor_value)) {
    ESP_LOGI(TAG, "State on draw: null");
  } else {
    ESP_LOGI(TAG, "State on draw: %f", sensor_value );
    //ESP_LOGI(TAG, "State on draw: not null");
  }

  int gaugeLength = this->size_;
    
  float radius = this-> height_ / 2;
  float valuepc = sensor_value / this->max_value_ ; // value as a % of max
  float needleAngle = ((valuepc ) * gaugeLength) + this->rotation_ ;  // assume value range from 0 to 100
  float thetaNeedle = needleAngle * M_PI / 180;

  // draw arc border
  //buff->fillArc(x_offset + radius, y_offset + radius, radius, 2, 0 + this->rotation_, gaugeLength + this->rotation_, legend);
  buff->fillArc(x_offset + radius, y_offset + radius, radius-GaugeDepth, 2, 0 + this->rotation_, this->size_ + this->rotation_, grey);
  buff->fillArc(x_offset + radius, y_offset + radius, radius, 2, 0 + this->rotation_, this->size_ + this->rotation_, grey);

  // drag the fill for the %
  for (int i = 0; i <= segments_; i++) {
    float segmentAngleStart =  (i * (gaugeLength / segments_)) + this->rotation_ ;
    float segmentAngleEnd =  ((i+1) * (gaugeLength / segments_)) + this->rotation_ ;
    if (needleAngle < segmentAngleEnd)
      segmentAngleEnd = needleAngle;
    float thetaTick = segmentAngleStart * M_PI / 180;    

    if (i < segments_)
    {

      int tickWidth = 3;

      // draw segment
      float color_position = i * (1.0 / segments_);
      Color segmentColour = this->value_to_rgb(start, mid, end, color_position);
      buff->fillArc(x_offset + radius, y_offset + radius, radius, GaugeDepth, segmentAngleStart, segmentAngleEnd, segmentColour);
      
      // draw tick
      buff->fillArc(x_offset + radius, y_offset + radius, radius, GaugeDepth, segmentAngleStart, segmentAngleStart + tickWidth, black);
    
      ESP_LOGW(TAG, "Drawing segment %d color_position %f needleAngle %f segmentAngleStart %f segmentAngleEnd %f ", i, color_position, needleAngle, segmentAngleStart, segmentAngleEnd);
    }

    // print legends  
    int legendinset = 40;
    int x_legend = radius + (radius-legendinset) * cos(thetaTick);
    int y_legend = radius + (radius-legendinset) * sin(thetaTick);
    
    int labelval = i * (this->max_value_ / segments_);    
    // if (labelval >= 1000)
    // {
    //   buff->printf(x_offset + x_legend, y_offset + y_legend, this->font_value_, legend, TextAlign::CENTER, "%dkW", labelval / 1000);
    // }
    // else
    // {
      buff->printf(x_offset + x_legend, y_offset + y_legend, this->font_value_, legend, TextAlign::CENTER, "%d", labelval);
    //}

  }


  // // print unit
  // int unitInset = 70;
  // int unitAngle = 90 + this->rotation_;
  // int x_unit = radius + (radius-unitInset) * cos(unitAngle);
  // int y_unit = radius + (radius-unitInset) * sin(unitAngle);
  // std::string unit = this->sensor_->get_unit_of_measurement();

  // ESP_LOGW(TAG, "unit %s ", unit);

  // buff->printf(x_offset + x_unit, y_offset + y_unit + 35, this->font_name_, color, TextAlign::CENTER, "%s", unit);
  
    
  // // Draw the needle 
  int needleLength = 150;
  int needleStart = radius - needleLength;
  
  int x_end = radius + needleStart * cos(thetaNeedle);
  int y_end = radius + needleStart * sin(thetaNeedle);

  int x_start = radius + (radius ) * cos(thetaNeedle);
  int y_start = radius + (radius ) * sin(thetaNeedle);

  //buff->line(x_offset + x_end, y_offset + y_end, x_offset + x_start, y_offset + y_start, color);  
  //ESP_LOGW(TAG, "Drawing pointer value %f angle %f - %d,%d to %d,%d", valuepc, needleAngle, x_end, y_end, x_start, y_start);



  // // draw pixel at 0,0
  // auto red = Color(255,0,0);
  // buff->draw_pixel_at(0, 0, red);
  
  // // draw pixel at h,w
  // auto green = Color(0,255,0);
  // buff->draw_pixel_at(this->width_, this->height_, green);
  
      
}


Color  Gauge::interpolate_color(Color color1, Color color2, float position) {
    int r = color1.red + (color2.red - color1.red) * position;
    int g = color1.green + (color2.green - color1.green) * position;
    int b = color1.blue + (color2.blue - color1.blue) * position;
    return Color(r, g, b);
}

Color  Gauge::value_to_rgb(Color start, Color mid, Color end, float position) {
    // int min_val = 0;
    // int max_val = 100;

    // // Calculate position of input value in range from 0 to 1
    // float position = (float)(value - min_val) / (float)(max_val - min_val);

    // Determine position on color gradient
    Color color;
    if (position < 0.5) {
        float color_position = position * 2;
        color = interpolate_color(start, mid, color_position);
    }
    else {
        float color_position = (position - 0.5) * 2;
        color = interpolate_color(mid, end, color_position);
    }

    return color;
}

void Gauge::set_sensor(sensor::Sensor *sensor) { this->sensor_ = sensor; }
void Gauge::setup() {
  ESP_LOGI(TAG, "Init setup for sensor %s", this->get_name().c_str());
  //this->data_.init(g->get_width());
  //sensor_->add_on_state_callback([this](float state) {  });
  //sensor_->add_on_state_callback([this](float state) { this->dataval = state; });
  //this->data_.set_update_time_ms(g->get_duration() * 1000 / g->get_width());
}

void Gauge::dump_config() {
  //for (auto *trace : traces_) {
  //  ESP_LOGCONFIG(TAG, "Gauge for sensor %s", trace->get_name().c_str());
  //}
}

}  // namespace gauge
}  // namespace esphome
