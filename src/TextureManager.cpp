#include "TextureManager.hpp"

#include <SFML/System/Err.hpp>
#include <cassert>
#include <spdlog/spdlog.h>
#include <sstream>

std::optional<std::string> TextureManager::setPathAndLoad(std::size_t textureIndex, std::string_view path)
{
    std::optional<std::string> result;
    if (path.empty())
        return result;

    m_textureUniforms[textureIndex].path = path;
    assert(textureIndex < m_textureUniforms.size());
    if (!std::filesystem::exists(path)) {
        m_textureUniforms[textureIndex].loaded = false;
        result.emplace(fmt::format("Texture {} not found", path.data()));
        return result;
    }

    auto defaultStr = sf::err().rdbuf();
    std::stringstream errStream;
    sf::err().rdbuf(errStream.rdbuf());
    if (!m_textureUniforms[textureIndex].texture.loadFromFile(path.data())) {
        // TODO: somehow display an error about this...?
        m_textureUniforms[textureIndex].loaded = false;
        result.emplace(errStream.str());
    } else {
        m_textureUniforms[textureIndex].loaded = true;
    }

    sf::err().rdbuf(defaultStr);
    return result;
}

sf::Texture* TextureManager::getTexture(std::size_t textureIndex)
{
    assert(textureIndex < m_textureUniforms.size());

    if (!m_textureUniforms[textureIndex].loaded) {
        return nullptr;
    }
    return &m_textureUniforms[textureIndex].texture;
}

std::string TextureManager::getTexturePath(std::size_t textureIndex) const
{
    assert(textureIndex < m_textureUniforms.size());
    return m_textureUniforms[textureIndex].path;
}
