#pragma once
#include <string>

namespace constants {
constexpr auto WINDOW_TITLE { "Shader Playground [v0.1.0]" };
constexpr std::size_t TEXTURE_CHANNELS_COUNT { 4 };
constexpr std::size_t SOURCE_STRING_CHAR_COUNT { 1000000 };
constexpr auto SIDE_PANEL_WINDOW_WIDTH_PERCENT { 0.25f };
constexpr auto BOTTOM_PANEL_WINDOW_WIDTH_PERCENT { 1.f - (1.f * SIDE_PANEL_WINDOW_WIDTH_PERCENT) };
constexpr auto BOTTOM_PANEL_WINDOW_HEIGHT_PERCENT { 0.15f };
}