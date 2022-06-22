#pragma once

#include "Constants.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <array>
#include <optional>
#include <string>

class TextureManager {
public:
    struct TextureEntry {
        sf::Texture texture;
        std::string path;
        bool loaded { false };
    };

    // Set the path of the texture & then attempt to load it
    [[nodiscard]] std::optional<std::string> setPathAndLoad(std::size_t textureIndex, std::string_view path);

    [[nodiscard]] sf::Texture* getTexture(std::size_t textureIndex);

    [[nodiscard]] std::string getTexturePath(std::size_t textureIndex) const;

private:
    std::array<TextureEntry, constants::TEXTURE_CHANNELS_COUNT> m_textureUniforms;
};