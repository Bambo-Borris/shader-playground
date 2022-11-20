#pragma once

#include "Constants.hpp"

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <array>
#include <optional>
#include <string>

class TextureManager; 

class ShaderManager {
public:
    struct ShaderUniforms {
        sf::Vector2f resolution;
        sf::Vector2f mousePos;
        sf::Time elapsedTime;
        sf::Time deltaTime;
        std::int32_t frames { 0 };
    };

    ShaderManager();
    void update(bool useShadertoy, TextureManager& textureMgr);
    [[nodiscard]] std::optional<std::string> loadAndCompile(std::string_view source, bool useShadertoy);

    [[nodiscard]] auto getUniforms() -> ShaderUniforms& { return m_uniforms; }
    [[nodiscard]] auto getShader() -> sf::Shader& { return m_shader; }
    [[nodiscard]] auto didFailLastCompilation() const -> bool { return m_didFailLastCompile; }

private:
    const std::string m_defaultUniformNames = R"str(
            uniform vec2 u_resolution; 
            uniform vec2 u_mouse;
            uniform float u_elapsedTime;
            uniform float u_deltaTime;
            uniform int u_frames;
            uniform sampler2D u_texture0;
            uniform sampler2D u_texture1;
            uniform sampler2D u_texture2;
            uniform sampler2D u_texture3;
            )str";

    const std::string m_shaderToyUniformNames = R"str(
            uniform vec2 iResolution; 
            uniform vec2 iMouse;
            uniform float iTime;
            uniform float iTimeDelta;
            uniform int iFrame;
            uniform sampler2D iChannel0;
            uniform sampler2D iChannel1;
            uniform sampler2D iChannel2;
            uniform sampler2D iChannel3;
            )str";

    const std::string m_shaderToyMainFunction = R"str(
        void mainImage(out vec4, in vec2);
        void main() {
            mainImage(gl_FragColor, gl_FragCoord);
        }
    )str";

    sf::Shader m_shader;
    ShaderUniforms m_uniforms;
    bool m_didFailLastCompile { false };
};
