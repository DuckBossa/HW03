#include <string>
#include <vector>
#include <cmath>
#include <iostream> /* error-checking */

class Entity {
	protected:
	sf::CircleShape form;
	
	public:
	Entity(float radius) : form(radius) {}
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
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

class EntityManager {
	private:
	std::vector<Player*> players;
	
	public:
	EntityManager() {}
	void addPlayer(Entity* player);
	void handleInput();
	void update(float dt);
	void render(sf::RenderTarget& g);
} em;

void initialize();
bool isKeyDown(const int& key);