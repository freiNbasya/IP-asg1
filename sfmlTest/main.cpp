#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <map>
#include <string>
#include <cmath>

class Player {
private:
    sf::Sprite playerSprite;
    const float speed = 5.0f;

public:
    Player(float x, float y, const sf::Texture& texture) {
        playerSprite.setTexture(texture);
        playerSprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        playerSprite.setPosition(x, y);
    }

    void handleInput(const sf::Keyboard& keyboard, const sf::RenderWindow& window) {
        if (keyboard.isKeyPressed(sf::Keyboard::Up)) {
            playerSprite.move(0, -speed);
        }
        if (keyboard.isKeyPressed(sf::Keyboard::Down)) {
            playerSprite.move(0, speed);
        }
        if (keyboard.isKeyPressed(sf::Keyboard::Left)) {
            playerSprite.move(-speed, 0);
        }
        if (keyboard.isKeyPressed(sf::Keyboard::Right)) {
            playerSprite.move(speed, 0);
        }

        sf::Vector2f playerPos = playerSprite.getPosition();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);

        float dx = worldMousePos.x - playerPos.x;
        float dy = worldMousePos.y - playerPos.y;

        float angle = std::atan2(dy, dx) * 180.0f / 3.14159f;
        playerSprite.setRotation(angle + 90.0f);
    }

    void checkBounds(const sf::RenderWindow& window) {
        sf::Vector2f position = playerSprite.getPosition();
        sf::FloatRect bounds = playerSprite.getGlobalBounds();

        if (position.x - bounds.width / 2 < 0) playerSprite.setPosition(bounds.width / 2, position.y);
        if (position.x + bounds.width / 2 > window.getSize().x) playerSprite.setPosition(window.getSize().x - bounds.width / 2, position.y);
        if (position.y - bounds.height / 2 < 0) playerSprite.setPosition(position.x, bounds.height / 2);
        if (position.y + bounds.height / 2 > window.getSize().y) playerSprite.setPosition(position.x, window.getSize().y - bounds.height / 2);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(playerSprite);
    }

    sf::Vector2f getPosition() const {
        return playerSprite.getPosition();
    }

    float getRotation() const {
        return playerSprite.getRotation();
    }

    void setPosition(const sf::Vector2f& pos) {
        playerSprite.setPosition(pos);
    }

    void setRotation(float rotation) {
        playerSprite.setRotation(rotation);
    }
};

class TextureManager {
private:
    std::map<std::string, sf::Texture> textures;

public:
    bool loadTexture(const std::string& name, const std::string& filePath) {
        sf::Texture texture;
        if (!texture.loadFromFile(filePath)) {
            return false;
        }
        textures[name] = texture;
        return true;
    }

    const sf::Texture& getTexture(const std::string& name) const {
        auto it = textures.find(name);
        if (it == textures.end()) {
            throw std::runtime_error("Texture not found: " + name);
        }
        return it->second;
    }
};

void saveGame(const Player& player, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open save file");
    }

    sf::Vector2f position = player.getPosition();
    float rotation = player.getRotation();

    file.write(reinterpret_cast<const char*>(&position), sizeof(position));
    file.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
}

void loadGame(Player& player, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open load file");
    }

    sf::Vector2f position;
    float rotation;

    file.read(reinterpret_cast<char*>(&position), sizeof(position));
    file.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));

    player.setPosition(position);
    player.setRotation(rotation);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 900), "SFML Player");
    window.setFramerateLimit(60);

    TextureManager textureManager;
    if (!textureManager.loadTexture("player", "assets/player.png")) {
        std::cerr << "Failed to load player texture" << std::endl;
        return -1;
    }

    Player player(600.0f, 450.0f, textureManager.getTexture("player"));

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    sf::Text funniText;
    funniText.setFont(font);
    funniText.setString("Bob's psychic ward");
    funniText.setCharacterSize(36);
    funniText.setFillColor(sf::Color::Black); 
    funniText.setPosition(550, 10);

    bool paused = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    paused = !paused;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Num1) {
                    saveGame(player, "save.dat");
                }
                if (event.key.code == sf::Keyboard::Num2) {
                    loadGame(player, "save.dat");
                }
            }
        }

        if (!paused) {
            player.handleInput(sf::Keyboard(), window);
            player.checkBounds(window);
        }

        window.clear(sf::Color::White);
        player.draw(window);
        window.draw(funniText);
        window.display();
    }

    return 0;
}
