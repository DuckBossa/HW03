#include "Main.h"

#define FPS	60.0f
#define SPF sf::seconds(1.0f/FPS)

const double PI = std::acos(-1);

namespace Keys {
	const auto UP = 0x11;		/* w */
	const auto DOWN = 0x1f;		/* s */
	const auto LEFT = 0x1e;		/* a */
	const auto RIGHT = 0x20;	/* d */
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");
	sf::Clock clock;
	sf::Time lag = sf::seconds(0);
	
	initialize();
	
	/* run the program as long as the window is open */
	while (window.isOpen()) {
		/* check all the window's events that were triggered since the last iteration of the loop */
		sf::Event event;
		while (window.pollEvent(event)) {
			/* "close requested" event: we close the window */
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		/* handle input */
		em.handleInput();
		
		/* update entities */
		em.update(SPF.asSeconds());
		/* collision detection */
		em.logic();
		window.clear();
		
		/* render */
		em.render(window);
		
		window.display();
		
		const auto time_elapsed = clock.getElapsedTime() + lag;
		if (time_elapsed < SPF) {
			sf::sleep(SPF - time_elapsed);
			lag = sf::seconds(0);
		} else {
			lag = time_elapsed - SPF;
		}
		clock.restart();
	}
	return 0;
}

sf::Vector2f Entity::getPos() const {
	return form.getPosition();
}

float Entity::getRad() const {
	return form.getRadius();
}

bool Entity::isKeyDown(const int& key) {
	short state = GetAsyncKeyState(MapVirtualKey(key, MAPVK_VSC_TO_VK_EX));
	return state >> 15 != 0;
}

void Entity::render(sf::RenderTarget& g) {
	g.draw(form);
}

void Player::update(float dt) {
	form.move(SPEED * direction);
}

void Player::handleInput() {
	direction.x = 0;
	direction.y = 0;
	if (isKeyDown(Keys::UP)) {
		direction.y -= 1;
	}
	if (isKeyDown(Keys::DOWN)) {
		direction.y += 1;
	}
	if (isKeyDown(Keys::LEFT)) {
		direction.x -= 1;
	}
	if (isKeyDown(Keys::RIGHT)) {
		direction.x += 1;
	}
	direction.x *= 1 - std::abs(direction.y)*(std::sqrt(2) - 1)/std::sqrt(2);
	direction.y *= 1 - std::abs(direction.x)*(std::sqrt(2) - 1)/std::sqrt(2);
}

bool EntityManager::circleCollision(const Entity& c1, const Entity& c2) {
	const auto c1p = c1.getPos();
	const auto c2p = c2.getPos();
	return ((c1p.x - c2p.x)*(c1p.x - c2p.x) + (c1p.y - c2p.y)*(c1p.y - c2p.y)) <= ((c1.getRad() + c2.getRad())*(c1.getRad() + c2.getRad()));
}

void EntityManager::addPlayer(Entity* player) {
	players.push_back(dynamic_cast<Player*>(player));
}

void EntityManager::handleInput() {
	for (auto player : players) {
		player->handleInput();
	}
}

void EntityManager::update(float dt) {
	for (auto player : players) {
		player->update(dt);
	}
}

void EntityManager::logic() {
	for (auto bullet : ebullets) {
		for (auto player : players) {
			if (circleCollision(*bullet, *player)) {
				//damage
			}
		}
	}

	for (auto bullet : pbullets) {
		for (auto enemy : enemies) {
			if (circleCollision(*bullet, *enemy)) {
				//destroy enemy
			}
		}
	}
}

void EntityManager::render(sf::RenderTarget& g) {
	for (auto player : players) {
		player->render(g);
	}
}

void initialize() {
	em.addPlayer(new Player());
}