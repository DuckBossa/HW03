#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <string>
#include <vector>
#include <cmath>
#include <iostream> /* error-checking */


namespace COLORS {
	const auto PLAYER = sf::Color::Blue;
	const auto PBULLET = sf::Color::White;
	const auto ENEMY = sf::Color::Red;
	const auto EBULLET = sf::Color::Magenta;
}

class Entity {
	protected:
	sf::CircleShape form;
	public:
	Entity(float radius,const sf::Color &col) : form(radius) {
		form.setFillColor(col);
	}
	sf::Vector2f getPos() const;
	float getRad() const;
	bool isKeyDown(const int& key);
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
	void move(const sf::Vector2f &dir) {
		form.move(dir);
	}
};

class Player : public Entity {
	private:
	static constexpr float RADIUS = 10.0f;
	static constexpr float SPEED = 5.0f;
	sf::Vector2f direction;
	
	public:
	Player(const sf::Vector2f &start_pos) : Entity(RADIUS,COLORS::PLAYER), direction(0, 0) {
		form.setPosition(start_pos);
		form.setOrigin(sf::Vector2f(RADIUS, RADIUS));
	}
	void update(float dt) override;
	void handleInput();
};

class Behavior {
public:
	virtual void behave() = 0;
};

class MoveForwardBehavior : public Behavior{
public:

};

class Bullet : public Entity {
	
};

class Enemy : public Entity {
	
};

class StageDirector {

};

class EntityManager {
	private:
	Player* player;
	std::vector<Bullet*> pbullets;
	std::vector<Bullet*> ebullets;
	std::vector<Enemy*> enemies;
	bool circleCollision(const Entity& c1, const Entity& c2);
	
	public:
	EntityManager() {}
	void addPlayer(Entity* player);
	void handleInput();
	void update(float dt);
	void logic();
	void render(sf::RenderTarget& g);
	void resolveWallCollision();
} em;

void initialize();