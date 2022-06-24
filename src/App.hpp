#pragma once

#include "ExampleShaders.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

#include <SFML/Graphics.hpp>
#include <array>

class App {
public:
    App();
    ~App();

    void run();

private:
    enum class ErrorMessageType { Shader, RenderTexture, Texture0, Texture1, Texture2, Texture3, ExportImage, MAX };
    // Tracks average FPS and sets the Window title to the FPS
    // value
    void logFPS(const sf::Time& dt);

    // Handles imgui UI objects
    void updateUI(const sf::Time& dt);

    void updateOptionsTab(const sf::Vector2f& sidePanelSize);

    void updateExampleShadersTab();

    void updateExportTab();

    void updateErrorPanel(const sf::Vector2f& sidePanelSize);

    // Load the provided example shader and set it as the
    // active shader
    void loadExampleShader(ExampleShaders exampleShader);

    bool saveFrameToFile(std::string_view filename) const;

    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    ShaderManager m_shaderMgr;
    TextureManager m_textureMgr;
    std::string m_shaderSource;
    std::vector<std::string> m_errorQueue;

    bool m_useShaderToyNames { false };
    sf::Int32 m_frames { 0 };
};
