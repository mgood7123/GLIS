#include <glis/internal/glis_class.hpp>

const struct GLIS_CLASS::normalized_device_coordinate
GLIS_CLASS::pixel_location_to_normalized_device_coordinate(int x, int y) {
    auto pixels = pixel_location_to_normalized_device_coordinate<int, float>(
        0.0f, x, y, static_cast<int>(width), static_cast<int>(height), true
    );
    return { pixels.x, pixels.y };
}
