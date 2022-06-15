#pragma once

#include <SFML/Graphics.hpp>

class App {
public:
    App();
    ~App();

    void run();

private:
    void logFPS(const sf::Time& dt);
    void updateUI(const sf::Time& dt);
    void setupShaderUniforms(const sf::Time& dt, const sf::Time& elapsed);
    void loadAndCompileShader();

    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    sf::Shader m_shader;
    std::string m_shaderSource;
    const std::string m_defaultUniformNames = R"str(
            uniform vec2 u_resolution; 
            uniform vec2 u_mouse;
            uniform float u_elapsedTime;
            uniform float u_deltaTime;
            uniform int u_frames;
            )str";

    const std::string m_shaderToyUniformNames = R"str(
            uniform vec2 iResolution; 
            uniform vec2 iMouse;
            uniform float iTime;
            uniform float iTimeDelta;
            uniform int iFrame;
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
