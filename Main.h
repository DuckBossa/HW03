#include <string>
#include <vector>
#include <cmath>
#include <iostream> /* error-checking */
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>




class Entity {
	protected:
	sf::CircleShape form;
	
	public:
	Entity(float radius) : form(radius) {}
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
	sf::Vector2f getPos() const {
		return  form.getPosition();
	}

	float getRad() const {
		return form.getRadius();
	}

};

class Player : public Entity {
	private:
	static constexpr float RADIUS = 10.0f;
	static constexpr float SPEED = 5.0f;
	sf::Vector2f direction;
	
	public:
	Player() : Entity(RADIUS), direction(0, 0) {
		form.setPosition(0, 0);
	}
	void update(float dt) override;
	void handleInput();
};

class Bullet : public Entity {
	
};

class Enemy : public Entity {
	
};

class StageDirector {

};

class EntityManager {
	private:
	std::vector<Player*> players;
	std::vector<Bullet*> pbullets;
	std::vector<Bullet*> ebullets;
	std::vector<Enemy*> enemies;
	public:
	EntityManager() {}
	void addPlayer(Entity* player);
	void handleInput();
	void update(float dt);
	void logic();
	void render(sf::RenderTarget& g);
	bool circleCollision(const Entity &c1, const Entity &c2);
} em;

void initialize();
bool isKeyDown(const int& key);