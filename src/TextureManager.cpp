#include "TextureManager.hpp"

#include <cassert>
#include <spdlog/spdlog.h>

void TextureManager::setPathAndLoad(std::size_t textureIndex, std::string_view path)
{
    assert(textureIndex < m_textureUniforms.size());
    if (!std::filesystem::exists(path)) {
        // TODO: somehow display an error about this...?
        spdlog::debug("Unable to load {}", path.data());
        m_textureUniforms[textureIndex].loaded = false;
        return;
    }

    if (!m_textureUniforms[textureIndex].texture.loadFromFile(path.data())) {
        // TODO: somehow display an error about this...?
        m_textureUniforms[textureIndex].loaded = false;
    }

    m_textureUniforms[textureIndex].loaded = true;
}

sf::Texture* TextureManager::getTexture(std::size_t textureIndex)
{
    assert(textureIndex < m_textureUniforms.size());

    if (!m_textureUniforms[textureIndex].loaded) {
        return nullptr;
    }
    return &m_textureUniforms[textureIndex].texture;
}
