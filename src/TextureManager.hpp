#pragma once

#include "Constants.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <array>
#include <string>

class TextureManager {
public:
    struct TextureEntry {
        sf::Texture texture;
        std::string path;
        bool loaded { false };
    };

    // Set the path of the texture & then attempt to load it
    void setPathAndLoad(std::size_t textureIndex, std::string_view path);

    [[nodiscard]] sf::Texture* getTexture(std::size_t textureIndex);

private:
    std::array<TextureEntry, constants::TEXTURE_CHANNELS_COUNT> m_textureUniforms;
};