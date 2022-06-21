#include "ShaderManager.hpp"

#include <SFML/System/Err.hpp>
#include <spdlog/fmt/fmt.h>
#include <sstream>

ShaderManager::ShaderManager() { }

void ShaderManager::update(bool useShadertoy)
{ // We failed to compile, so no point trying to pass uniforms
    if (m_didFailLastCompile)
        return;

    if (!useShadertoy) {
        m_shader.setUniform("u_deltaTime", m_uniforms.deltaTime.asSeconds());
        m_shader.setUniform("u_elapsedTime", m_uniforms.elapsedTime.asSeconds());
        m_shader.setUniform("u_resolution", m_uniforms.resolution);
        m_shader.setUniform("u_mouse", m_uniforms.mousePos);
        m_shader.setUniform("u_frames", m_uniforms.frames);

        for (std::size_t i = 0; i < m_uniforms.loadResults.size(); ++i) {
            if (m_uniforms.loadResults[i]) {
                auto var = fmt::format("u_texture{}", i);
                m_shader.setUniform(var, m_uniforms.textures[i]);
            }
        }
    } else {
        m_shader.setUniform("iTimeDelta", m_uniforms.deltaTime.asSeconds());
        m_shader.setUniform("iTime", m_uniforms.elapsedTime.asSeconds());
        m_shader.setUniform("iResolution", m_uniforms.resolution);
        m_shader.setUniform("iMouse", m_uniforms.mousePos);
        m_shader.setUniform("iFrame", m_uniforms.frames);

        for (std::size_t i = 0; i < m_uniforms.loadResults.size(); ++i) {
            if (m_uniforms.loadResults[i]) {
                auto var = fmt::format("iChannel{}", i);
                m_shader.setUniform(var, m_uniforms.textures[i]);
            }
        }
    }
}

std::optional<std::string> ShaderManager::loadAndCompile(std::string_view source, bool useShadertoy)
{
    std::optional<std::string> result;
    bool foundChar = false;
    for (auto& c : source) {
        if (c == '\0') {
            continue;
        }
        foundChar = true;
        break;
    }

    if (!foundChar) {
        // m_errorString.clear();
        m_didFailLastCompile = false;
        return result;
    }

    // Redirect the error stream
    // so we can log the shader errors
    // to an imgui window
    auto defaultStr = sf::err().rdbuf();
    std::stringstream errStream;
    sf::err().rdbuf(errStream.rdbuf());

    // We need to append on the uniforms as
    // string depending on whether or not
    // we're using the shadertoy form or not
    std::string combined;
    if (!useShadertoy) {
        combined = m_defaultUniformNames + source.data();
    } else {
        combined = m_shaderToyUniformNames + m_shaderToyMainFunction + source.data();
    }

    // Let's compile the shader, if it failed
    // then we should mark a flag saying it failed
    if (!m_shader.loadFromMemory(combined, sf::Shader::Type::Fragment)) {
        result.emplace(errStream.str());
        m_didFailLastCompile = true;
    } else
        m_didFailLastCompile = false;

    // Redirect the error stream
    // back to its default state!
    sf::err().rdbuf(defaultStr);
    return result;
}
