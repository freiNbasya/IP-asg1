#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <sstream>
#include <iomanip> 

class Player {
private:
	sf::RectangleShape playerShape;
	const float speed = 5.0f;

public:
	Player(float x, float y) {
		playerShape.setSize(sf::Vector2f(50.0f, 30.0f));
		playerShape.setFillColor(sf::Color::Red);
		playerShape.setOrigin(playerShape.getSize() / 2.0f);
		playerShape.setPosition(x, y);
	}

	void handleInput(const sf::Keyboard& keyboard, const sf::RenderWindow& window) {
		if (keyboard.isKeyPressed(sf::Keyboard::Up)) {
			playerShape.move(0, -speed);
		}
		if (keyboard.isKeyPressed(sf::Keyboard::Down)) {
			playerShape.move(0, speed);
		}
		if (keyboard.isKeyPressed(sf::Keyboard::Left)) {
			playerShape.move(-speed, 0);
		}
		if (keyboard.isKeyPressed(sf::Keyboard::Right)) {
			playerShape.move(speed, 0);
		}

		sf::Vector2f playerPos = playerShape.getPosition();
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);

		float dx = worldMousePos.x - playerPos.x;
		float dy = worldMousePos.y - playerPos.y;

		float angle = std::atan2(dy, dx) * 180.0f / 3.14159f;
		playerShape.setRotation(angle + 90.0f);
	}

	void checkBounds(const sf::RenderWindow& window) {
		sf::Vector2f position = playerShape.getPosition();
		sf::Vector2f size = playerShape.getSize();

		if (position.x - size.x / 2 < 0) playerShape.setPosition(size.x / 2, position.y);
		if (position.x + size.x / 2 > window.getSize().x) playerShape.setPosition(window.getSize().x - size.x / 2, position.y);
		if (position.y - size.y / 2 < 0) playerShape.setPosition(position.x, size.y / 2);
		if (position.y + size.y / 2 > window.getSize().y) playerShape.setPosition(position.x, window.getSize().y - size.y / 2);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(playerShape);
	}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(1200, 900), "SFML Player");
	window.setFramerateLimit(60);

	Player player(600.0f, 450.0f);

	sf::Clock clock;
	float timer = 0.0f;

	sf::Font font;
	if (!font.loadFromFile("C://Labs_Kse//OOP//sfmlTest//arial.ttf")) {
		return -1;
	}

	sf::Text timerText;
	timerText.setFont(font);
	timerText.setCharacterSize(24);
	timerText.setFillColor(sf::Color::White);
	timerText.setPosition(550, 10);

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


					clock.restart();

				}

				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
		}

		if (!paused) {
			timer += clock.restart().asSeconds();

			player.handleInput(sf::Keyboard(), window);
			player.checkBounds(window);

			timerText.setString(std::format("{:f}", timer));
		}

		window.clear(sf::Color::Blue);
		player.draw(window);
		window.draw(timerText);
		window.display();
	}

	return 0;
}
