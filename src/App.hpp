#pragma once

#include "ExampleShaders.hpp"

#include <SFML/Graphics.hpp>
#include <array>

class App {
public:
    App();
    ~App();

    void run();

private:
    // Tracks average FPS and sets the Window title to the FPS
    // value
    void logFPS(const sf::Time& dt);

    // Handles imgui UI objects
    void updateUI(const sf::Time& dt);

    // Update the uniform variables for active shader
    void setupShaderUniforms(const sf::Time& dt, const sf::Time& elapsed);

    // Setup the shader string and compile the shader
    void loadAndCompileShader();

    // Load the provided example shader and set it as the
    // active shader
    void loadExampleShader(ExampleShaders exampleShader);

    // Load a texture into the provided input channel
    void loadInputChannelTexture(std::size_t channelIndex, std::string_view path);

    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    sf::Shader m_shader;
    std::string m_shaderSource;
    std::array<sf::Texture, 4> m_textureInputs;
    std::array<std::string, 4> m_textureInputPaths;
    std::array<bool, 4> m_textureInputLoadResuls = { false, false, false, false };

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

    std::string m_errorString;
    bool m_didFailLastCompile { false };
    bool m_failedToMakeRenderTexture { false };
    bool m_useShaderToyNames { false };
    sf::Int32 m_frames { 0 };
};
