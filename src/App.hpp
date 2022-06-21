#pragma once

#include "ExampleShaders.hpp"
#include "ShaderManager.hpp"

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

    // Load the provided example shader and set it as the
    // active shader
    void loadExampleShader(ExampleShaders exampleShader);

    // Load a texture into the provided input channel
    void loadInputChannelTexture(std::size_t channelIndex, std::string_view path);

    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    ShaderManager m_shaderMgr;
    std::string m_shaderSource;
    std::array<std::string, 4> m_textureInputPaths;
    std::string m_errorString;
    
    bool m_failedToMakeRenderTexture { false };
    bool m_useShaderToyNames { false };
    sf::Int32 m_frames { 0 };
};
