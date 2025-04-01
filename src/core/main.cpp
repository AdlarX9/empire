#define WIDTH 2400
#define HEIGHT 1500

#include "../three/main.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "../maths/utils.hpp"

using namespace std;

int main() {
	// Create the main window
	sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Empire");
	sf::Clock        clock;
	unsigned int     nbrFrame = 0;

	sf::RenderTexture   texture(sf::Vector2u(WIDTH, HEIGHT));
	sf::Vector3<double> cameraPosition(5, 5, 5);
	sf::Vector3<double> cameraDirection(-1, -1, -1);
	Camera              camera(cameraPosition, cameraDirection);
	Scene               scene;

	Renderer renderer(texture, camera, scene);

	// Start the game loop
	while (window.isOpen()) {
		// Process events
		while (const std::optional event = window.pollEvent()) {
			// Close window: exit
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}

		// Clear screen
		window.clear();

		renderer.render();
		sf::Sprite sprite(texture.getTexture());
		window.draw(sprite);

		// Update the window
		window.display();
		nbrFrame++;
	}

	cout << "Average FPS: " << (double)nbrFrame / clock.getElapsedTime().asSeconds() << endl;

	return 0;
}
