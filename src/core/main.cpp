#include "../three/main.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "../maths/utils.hpp"

using namespace std;

int main() {
	// Create the main window
	sf::RenderWindow window(sf::VideoMode({800, 600}), "Empire");

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

		// Update the window
		window.display();
	}
}
