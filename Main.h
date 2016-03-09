#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <iostream> /* error-checking */

namespace Fonts {
	const std::string SCORE = "fonts/ShareTechMono-Regular.ttf";
}

namespace COLORS {
	const auto PLAYER = sf::Color::Blue;
	const auto PBULLET = sf::Color::White;
	const auto ENEMY = sf::Color::Red;
	const auto EBULLET = sf::Color::Magenta;
}

namespace SPEEDS {
	const auto PLAYER = 5.0f;
	const auto BULLETS = 5.0f;
	const auto ENEMY = 3.0f;
}

class Score {
private:
	const int CHAR_SIZE = 16;
	const std::string SCORE = "Score: ";
	sf::Text text;
	sf::Font font;
	int score;
	
public:
	Score() : score(0) {
		if (!font.loadFromFile(Fonts::SCORE)) {
			/* font loading error */
		}
		text.setFont(font);
		text.setCharacterSize(CHAR_SIZE);
		text.setColor(sf::Color::White);
		text.setPosition(0, 0);
	}
	void setPosition();
	void add();
	void draw(sf::RenderTarget& g);
};

class Entity {
protected:
	sf::CircleShape form;
	
public:
	Entity(float radius,const sf::Color &col) : form(radius) {
		form.setFillColor(col);
	}
	sf::Vector2f getPos() const;
	void setPos(float x, float y);
	float getRad() const;
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
	void move(const sf::Vector2f &dir);
};

class Player : public Entity {
private:
	static constexpr float RADIUS = 10.0f;
	static constexpr float SPEED = 5.0f;
	sf::Vector2f direction;
	Score score;
	
public:
	Player(const sf::Vector2f &start_pos) : Entity(RADIUS,COLORS::PLAYER), direction(0, 0) {
		form.setPosition(start_pos);
		form.setOrigin(sf::Vector2f(RADIUS, RADIUS));
	}
	void handleInput();
	void fire();
	void takeDamage();
	void update(float dt) override;
	void renderScore(sf::RenderTarget& g);
};

class Behavior {
public:
	virtual void behave(Entity* e) = 0;
};

class MoveForwardBehavior : public Behavior{
private:
	float mag;
public:
	MoveForwardBehavior(float mag) : mag(mag){}
	void behave(Entity* e) override;
};

class ShootForwardBehavior : public Behavior {};

class Bullet : public Entity {
private:
	static constexpr float RADIUS = 5.0f;
	static constexpr float SPEED = 10.0f;
	Behavior* move;
	
public:
	Bullet(const sf::Vector2f &start_pos) : Entity(RADIUS, COLORS::EBULLET) {
		form.setPosition(start_pos.x, start_pos.y);
	}
	Bullet(float radius, const sf::Color &col, Behavior* move) : Entity(radius, col), move(move) {}
	void init(float radius, const sf::Color &col, Behavior* move) {
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		this->move = move;
	}
	void update(float dt) override;
	//Entity(float radius,const sf::Color &col) :
};

class Enemy : public Entity {
private:
	static constexpr float RADIUS = 10.0f;
	static constexpr float SPEED = 5.0f;
	Behavior* move;
	Behavior* shoot;
	
public:
	Enemy(const sf::Vector2f &start_pos) : Entity(RADIUS, COLORS::ENEMY) {
		form.setPosition(start_pos.x, start_pos.y);
	}
	Enemy(float radius, const sf::Color &col, Behavior* move,Behavior* shoot) : Entity(radius,col),move(move),shoot(shoot){}
	void init(float radius, const sf::Color &col, Behavior* move, Behavior* shoot) {
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		this->move = move;
		this->shoot = shoot;
	}
	void update(float dt) override;
};

class Pool {
private:
	std::queue<Bullet*> bQueue;
	std::queue<Enemy*> eQueue;
	
public:
	Bullet* getBullet();
	void returnBullet(Bullet* bull);
	Enemy* getEnemy();
	void returnEnemy(Enemy* enem);
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
	void addEnemy(Entity* enemy);
	void addBullet(Entity* bullet); /* not sure if there's gonna be separate functions for adding enemy- and player-bullets */
	void handleInput();
	void update(float dt);
	void logic();
	void render(sf::RenderTarget& g);
	void resolveWallCollision();
} em;

void initialize();
bool isKeyDown(const int& key);