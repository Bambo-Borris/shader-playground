#include "App.hpp"
#include "Constants.hpp"

#include <array>
#include <filesystem>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <string>

App::App()
{
    sf::ContextSettings ctxt;
    ctxt.antialiasingLevel = 16;
    m_window.create(sf::VideoMode({ 1024, 720 }), constants::WINDOW_TITLE, sf::Style::Default, ctxt);
    spdlog::set_level(spdlog::level::debug);
    m_window.setFramerateLimit(60);

    sf::Image icon;
    if (!icon.loadFromFile("bin/appicon.png"))
        throw std::runtime_error("Unable to load application icon");

    m_window.setIcon(icon.getSize(), icon.getPixelsPtr());

    if (!ImGui::SFML::Init(m_window))
        throw std::runtime_error("Unable to initialise ImGui SFML");

    if (!m_renderTexture.create({ 600, 600 }))
        throw std::runtime_error("Unable to create RenderTexture");

    if (!sf::Shader::isAvailable())
        throw std::runtime_error("Shaders are not available");

    m_shaderSource.resize(constants::SOURCE_STRING_CHAR_COUNT);
    m_errorQueue.resize(static_cast<std::size_t>(ErrorMessageType::MAX));
}

App::~App() { ImGui::SFML::Shutdown(m_window); }

void App::run()
{
    sf::Clock loopClock;
    sf::Clock elapsedClock;
    while (m_window.isOpen()) {
        // spdlog::debug("Has focus? {}", m_window.hasFocus());
        auto dt = loopClock.restart();
        if (dt > sf::seconds(0.25f)) {
            dt = sf::seconds(0.25f);
        }

        sf::Event event;
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_window, event);
            if (event.type == sf::Event::Closed)
                m_window.close();

            if (event.type == sf::Event::Resized) {
                const sf::View v { sf::Vector2f { static_cast<float>(event.size.width) / 2.0f,
                                                  static_cast<float>(event.size.height) / 2.0f },
                                   sf::Vector2f { static_cast<float>(event.size.width),
                                                  static_cast<float>(event.size.height) } };
                m_window.setView(v);
            };
        }

        logFPS(dt);
        updateUI(dt);

        auto& uniforms = m_shaderMgr.getUniforms();
        const auto renderTextureSize = sf::Vector2f { m_renderTexture.getSize() };

        uniforms.elapsedTime = elapsedClock.getElapsedTime();
        uniforms.deltaTime = dt;
        uniforms.resolution = renderTextureSize;

        uniforms.mousePos = sf::Vector2f { sf::Mouse::getPosition(m_window) };
        const auto subtractAmount
            = sf::Vector2f { m_window.getView().getCenter().x - static_cast<float>(renderTextureSize.x) / 2.f,
                             m_window.getView().getCenter().y - static_cast<float>(renderTextureSize.y) / 2.f };
        uniforms.mousePos -= subtractAmount;
        uniforms.mousePos.x = std::clamp(uniforms.mousePos.x, 0.0f, renderTextureSize.x);
        uniforms.mousePos.y = std::clamp(uniforms.mousePos.y, 0.0f, renderTextureSize.y);
        uniforms.mousePos.y = renderTextureSize.y - uniforms.mousePos.y;
        uniforms.frames = m_frames;

        m_shaderMgr.update(m_useShaderToyNames, m_textureMgr);

        m_renderTexture.clear();
        sf::RectangleShape shape(sf::Vector2f { m_renderTexture.getSize() });
        shape.setTextureRect({ { 0, 0 }, sf::Vector2i { shape.getSize() } });
        m_renderTexture.draw(shape, &m_shaderMgr.getShader());
        m_renderTexture.display();

        m_window.clear(sf::Color(75, 75, 75));
        sf::Sprite spr(m_renderTexture.getTexture());
        spr.setPosition((sf::Vector2f(m_window.getSize()) * 0.5f) - (spr.getGlobalBounds().getSize() * 0.5f));
        m_window.draw(spr);
        ImGui::SFML::Render(m_window);
        m_window.display();
        ++m_frames;
    }
}

void App::logFPS(const sf::Time& dt)
{
    static int counter = 0;
    static auto sum = sf::Time::Zero;

    if (counter < 10) {
        sum += dt;
        ++counter;
    } else {
        auto fps = 1.0f / (sum.asSeconds() / static_cast<float>(counter));
        const auto newTitle = fmt::format("{} - FPS {}", constants::WINDOW_TITLE, static_cast<sf::Uint32>(fps));
        m_window.setTitle(newTitle);
        sum = sf::Time::Zero;
        counter = 0;
    }
}

void App::updateUI(const sf::Time& dt)
{
    constexpr auto SIDE_PANEL_WINDOW_WIDTH_PERCENT { 0.25f };
    constexpr auto BOTTOM_PANEL_WINDOW_WIDTH_PERCENT { 1.f - (1.f * SIDE_PANEL_WINDOW_WIDTH_PERCENT) };
    constexpr auto BOTTOM_PANEL_WINDOW_HEIGHT_PERCENT { 0.15f };

    ImGui::SFML::Update(m_window, dt);
    const auto renderWindowSize = sf::Vector2f { m_window.getSize() };
    const auto sidePanelSize
        = sf::Vector2f { renderWindowSize.x * SIDE_PANEL_WINDOW_WIDTH_PERCENT, renderWindowSize.y };

    /*
    Options Window
    */
    ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(sidePanelSize);
    ImGui::SetWindowPos({ 0, 0 });

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::Text("Fragment Shader Source");
    if (ImGui::InputTextMultiline("##source",
                                  m_shaderSource.data(),
                                  m_shaderSource.size(),
                                  { 0, 0.4f * sidePanelSize.y },
                                  ImGuiInputTextFlags_AllowTabInput)) {
        const auto result = m_shaderMgr.loadAndCompile(m_shaderSource, m_useShaderToyNames);
        if (result) {
            m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)] = result.value();
        } else {
            m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)].clear();
        }
    }
    ImGui::Separator();

    if (ImGui::Checkbox("Use Shadertoy Setup", &m_useShaderToyNames)) {
        const auto result = m_shaderMgr.loadAndCompile(m_shaderSource, m_useShaderToyNames);
        if (result) {
            m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)] = result.value();
        } else {
            m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)].clear();
        }
    }

    ImGui::Text("In built variables");
    if (!m_useShaderToyNames) {
        ImGui::Text("u_deltaTime = Delta Time");
        ImGui::Text("u_elapsedTime = Elapsed Time");
        ImGui::Text("u_resolution = Surface Resolution");
        ImGui::Text("u_mouse = Mouse Screen Position");
        ImGui::Text("u_frames = Number of frames elapsed");
    } else {
        ImGui::Text("iTimeDelta = Delta Time");
        ImGui::Text("iTime = Elapsed Time");
        ImGui::Text("iResolution = Surface Resolution");
        ImGui::Text("iMouse = Mouse Screen Position");
        ImGui::Text("iFrame = Number of frames elapsed");
    }
    ImGui::Separator();

    std::array<sf::Int32, 2> dimensions
        = { static_cast<signed>(m_renderTexture.getSize().x), static_cast<signed>(m_renderTexture.getSize().y) };
    ImGui::Text("Resolution");
    if (ImGui::InputInt2("##", dimensions.data())) {
        if (dimensions[0] > 0 && dimensions[1] > 0) {
            if (!m_renderTexture.create(
                    { static_cast<unsigned>(dimensions[0]), static_cast<unsigned>(dimensions[1]) })) {
                m_failedToMakeRenderTexture = true;
            } else {
                m_failedToMakeRenderTexture = false;
            }
        }
    }
    ImGui::Separator();

    /*
    Texture channels for shader
    */
    ImGui::Text("Textures");

    for (std::size_t i = 0; i < constants::TEXTURE_CHANNELS_COUNT; ++i) {
        const std::string varName = m_useShaderToyNames ? "iChannel" : "u_texture";
        std::string texturePath = m_textureMgr.getTexturePath(i);
        texturePath.resize(300);
        ImGui::Text("%s%zu", varName.data(), i);
        if (ImGui::InputText(fmt::format("##texture{}", i).data(), texturePath.data(), texturePath.size())) {
            if (texturePath[0] == '\0')
                texturePath.clear();

            const auto result = m_textureMgr.setPathAndLoad(i, texturePath);

            // If we got an error we'll set the queue error string
            // if not we'll just clear the error string just in case
            // it still contains an error
            const std::size_t errorQueueIndex = static_cast<std::size_t>(ErrorMessageType::Texture0) + i;
            if (result) {
                m_errorQueue[errorQueueIndex] = result.value();
            } else {
                m_errorQueue[errorQueueIndex].clear();
            }
        }

        if (m_textureMgr.getTexture(i)) {
            auto spr = sf::Sprite(*m_textureMgr.getTexture(i));
            const auto width = (ImGui::GetWindowWidth() * 0.95f) * 0.75f;
            const auto scaleFactor = width / spr.getGlobalBounds().width;
            spr.setScale({ scaleFactor, scaleFactor });
            ImGui::Image(spr);
        }
    }
    ImGui::Separator();

    /*
    Example Shaders
    */
    ImGui::Text("Default Shaders");
    if (ImGui::Button("Basic"))
        loadExampleShader(ExampleShaders::Basic);

    if (ImGui::Button("Generic Noise"))
        loadExampleShader(ExampleShaders::Generic_Noise);

    if (ImGui::Button("Simplex Noise"))
        loadExampleShader(ExampleShaders::Simplex_Noise);

    if (ImGui::Button("Texture Background"))
        loadExampleShader(ExampleShaders::TextureBackground);

    ImGui::PopItemWidth();
    ImGui::End();

    /*
    Errors Window
    */
    const auto errorsPanelSize = sf::Vector2f { BOTTOM_PANEL_WINDOW_WIDTH_PERCENT * renderWindowSize.x,
                                                renderWindowSize.y * BOTTOM_PANEL_WINDOW_HEIGHT_PERCENT };
    ImGui::Begin("Errors", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(errorsPanelSize);
    ImGui::SetWindowPos({ sidePanelSize.x, renderWindowSize.y - errorsPanelSize.y });
    for (std::size_t i = 0; i < static_cast<std::size_t>(ErrorMessageType::MAX); ++i) {
        if (m_errorQueue[i].empty())
            continue;
        const auto asEnum = static_cast<ErrorMessageType>(i);
        switch (asEnum) {
        case ErrorMessageType::Shader:
            ImGui::TextColored(ImVec4(sf::Color::Red), "Shader compile error!");
            break;
        case ErrorMessageType::Texture0:
            ImGui::TextColored(ImVec4(sf::Color::Red), "Texture slot 0 load error!");
            break;
        case ErrorMessageType::Texture1:
            ImGui::TextColored(ImVec4(sf::Color::Red), "Texture slot 1 load error!");
            break;
        case ErrorMessageType::Texture2:
            ImGui::TextColored(ImVec4(sf::Color::Red), "Texture slot 2 load error!");
            break;
        case ErrorMessageType::Texture3:
            ImGui::TextColored(ImVec4(sf::Color::Red), "Texture slot 3 load error!");
            break;
        default:
            assert(false);
            break;
        }
        ImGui::Text("%s", m_errorQueue[i].data());
    }

    if (m_failedToMakeRenderTexture) {
        ImGui::TextColored(ImVec4(sf::Color::Red), "Unable to create render texture");
    }
    ImGui::End();

    /*
    Export Window
    (Still deciding on preferred layout..)
    */

    // ImGui::Begin("Export", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    // ImGui::SetWindowSize(sidePanelSize);
    // ImGui::SetWindowPos({ renderWindowSize.x - sidePanelSize.x, 0 });
    // ImGui::End();
}

void App::loadExampleShader(ExampleShaders exampleShader)
{
    m_shaderSource.clear();

    switch (exampleShader) {
    case ExampleShaders::Basic:
        m_shaderSource = BASIC_SHADER_SOURCE;
        break;
    case ExampleShaders::Generic_Noise:
        m_shaderSource = GENERIC_NOISE_SOURCE;
        break;
    case ExampleShaders::Simplex_Noise:
        m_shaderSource = SIMPLEX_SHADER_SOURCE;
        break;
    case ExampleShaders::TextureBackground:
        m_shaderSource = TEXTURE_BACKGROUND_SOURCE;
        break;
    default:
        assert(false);
    }

    m_shaderSource.resize(constants::SOURCE_STRING_CHAR_COUNT);
    m_useShaderToyNames = false;
    const auto result = m_shaderMgr.loadAndCompile(m_shaderSource, m_useShaderToyNames);
    if (result) {
        m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)] = result.value();
    } else {
        m_errorQueue[static_cast<std::size_t>(ErrorMessageType::Shader)].clear();
    }
}
