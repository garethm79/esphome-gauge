from esphome.components.font import Font
from esphome.components import sensor, color
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_COLOR,
    CONF_ID,
    CONF_SEGMENTS,
    CONF_VALUE_FONT,
    CONF_NAME_FONT,
    CONF_WIDTH,
    CONF_SENSOR,
    CONF_HEIGHT,
    CONF_MIN_VALUE,
    CONF_MAX_VALUE,
    CONF_LINE_THICKNESS,
    CONF_BORDER,
    CONF_ROTATION,
    CONF_SIZE,
)

CODEOWNERS = ["@synco"]

DEPENDENCIES = ["display", "sensor"]
MULTI_CONF = True

gauge_ns = cg.esphome_ns.namespace("gauge")
Gauge_ = gauge_ns.class_("Gauge", cg.Component)

DirectionType = gauge_ns.enum("DirectionType")
DIRECTION_TYPE = {
    "AUTO": DirectionType.DIRECTION_TYPE_AUTO,
    "HORIZONTAL": DirectionType.DIRECTION_TYPE_HORIZONTAL,
    "VERTICAL": DirectionType.DIRECTION_TYPE_VERTICAL,
}

ValuePositionType = gauge_ns.enum("ValuePositionType")
VALUE_POSITION_TYPE = {
    "NONE": ValuePositionType.VALUE_POSITION_TYPE_NONE,
    "AUTO": ValuePositionType.VALUE_POSITION_TYPE_AUTO,
    "BESIDE": ValuePositionType.VALUE_POSITION_TYPE_BESIDE,
    "BELOW": ValuePositionType.VALUE_POSITION_TYPE_BELOW,
}

GAUGE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(Gauge_),
        cv.Required(CONF_WIDTH): cv.positive_not_null_int,
        cv.Required(CONF_HEIGHT): cv.positive_not_null_int,
        cv.Optional(CONF_SEGMENTS): cv.positive_int,
        cv.Optional(CONF_ROTATION): cv.positive_int,
        cv.Optional(CONF_SIZE): cv.positive_int,        
        cv.Optional(CONF_BORDER): cv.boolean,
        # Single trace options in base
        cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_LINE_THICKNESS): cv.positive_int,
        cv.Optional(CONF_COLOR): cv.use_id(color.ColorStruct),
        # Axis specific options (Future feature may be to add second Y-axis)
        cv.Optional(CONF_MIN_VALUE): cv.float_,
        cv.Optional(CONF_MAX_VALUE): cv.float_,
        cv.Optional(CONF_VALUE_FONT): cv.use_id(Font),
        cv.Optional(CONF_NAME_FONT): cv.use_id(Font),
    }
)

CONFIG_SCHEMA = cv.All(
    GAUGE_SCHEMA,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    await cg.register_component(var, config)

    # Gauge options

    if CONF_BORDER in config:
        cg.add(var.set_border(config[CONF_BORDER]))
    # Axis related options
    if CONF_MIN_VALUE in config:
        cg.add(var.set_min_value(config[CONF_MIN_VALUE]))
    if CONF_MAX_VALUE in config:
        cg.add(var.set_max_value(config[CONF_MAX_VALUE]))
    if CONF_SEGMENTS in config:
        cg.add(var.set_segments(config[CONF_SEGMENTS]))
    if CONF_SIZE in config:
        cg.add(var.set_size(config[CONF_SIZE]))
    if CONF_ROTATION in config:
        cg.add(var.set_rotation(config[CONF_ROTATION]))
    if CONF_VALUE_FONT in config:
        font = await cg.get_variable(config[CONF_VALUE_FONT])
        cg.add(var.set_value_font(font))
    if CONF_NAME_FONT in config:
        font = await cg.get_variable(config[CONF_NAME_FONT])
        cg.add(var.set_name_font(font))

    sens = await cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_sensor(sens))

    cg.add_define("USE_GAUGE")
