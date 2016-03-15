#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <iostream> /* error-checking */

namespace Fonts {
	const std::string SCORE = "fonts/ShareTechMono-Regular.ttf";
}

namespace Keys {
	const auto UP = 0x11;		/* w */
	const auto DOWN = 0x1f;		/* s */
	const auto LEFT = 0x1e;		/* a */
	const auto RIGHT = 0x20;	/* d */
	const auto FIRE = 0x39;		/* spacebar */
}

namespace Colors {
	const auto PLAYER = sf::Color::Blue;
	const auto PBULLET = sf::Color::White;
	const auto ENEMY = sf::Color::Red;
	const auto EBULLET = sf::Color::Magenta;
}

namespace Speeds { /* pixels per frame */
	const auto PLAYER = 200.0f;
	const auto BULLET = 15.0f;
	const auto ENEMY = 3.0f;
}

namespace Radii {
	const auto PLAYER = 10.0f;
	const auto ENEMY = 15.0f;
	const auto EBULLET = 2.5f;
	const auto PBULLET = 5.0f;
}

namespace FiringRates {
	const auto PLAYER = 0.5f;
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

class MoveBehavior {
protected:
	float speed;
	sf::Vector2f direction;
	
public:
	MoveBehavior(float speed, const sf::Vector2f& direction) : speed(speed), direction(direction) {}
	virtual void move(sf::CircleShape& form, float dt) = 0;
};

class MoveStraightBehavior : public MoveBehavior {
public:
	MoveStraightBehavior(float speed, const sf::Vector2f& direction) : MoveBehavior(speed, direction) {}
	void move(sf::CircleShape& form, float dt) override;
};

class Entity {
protected:
	sf::CircleShape form;

public:
	Entity() {}
	Entity(const sf::Vector2f& start_pos, float radius, const sf::Color& col) : form(radius) {
		form.setFillColor(col);
		form.setPosition(start_pos);
	}
	sf::Vector2f getPos() const;
	void setPos(float x, float y);
	float getRad() const;
	bool isKeyDown(const int& key);
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
};

class ShootingBehavior {
public:
	ShootingBehavior() {}
	~ShootingBehavior() {}
	virtual void shoot(float dt, Entity* e) = 0;
};

class CircularShoot : public ShootingBehavior {
private:
	float rotatePerSec;
	float angleToRotate;
	float firingRate;
	float timerR;
	float timerS;
	
public:
	CircularShoot(float rotatePerSec, float angleToRotate,float firingRate) : rotatePerSec(rotatePerSec), angleToRotate(angleToRotate), firingRate(firingRate) {
		timerR = timerS = 0;
	}
	void shoot(float dt,Entity* e) override;
};

class Bullet : public Entity {
private:
	MoveBehavior* move_behavior;
public:
	Bullet() : Entity(sf::Vector2f(0, 0), Radii::EBULLET, Colors::EBULLET) {}
	~Bullet() {
		delete move_behavior;
	}
	void init(const sf::Vector2f& start_pos, MoveBehavior* _move_behavior) {
		form.setPosition(start_pos);
		form.setOrigin(Radii::EBULLET, Radii::EBULLET);
		move_behavior = _move_behavior;
	}
	void update(float dt) override;
};

class Player : public Entity {
private:
	int spf; //shots per frame
	sf::Vector2f direction;
	Score score;
	
public:
	Player(const sf::Vector2f& start_pos) : Entity(start_pos, Radii::PLAYER, Colors::PLAYER), direction(0, 0) {
		form.setOrigin(Radii::PLAYER, Radii::PLAYER);
	}
	void handleInput();
	void fire();
	void takeDamage();
	void update(float dt) override;
	void renderScore(sf::RenderTarget& g);
};

class Enemy : public Entity {
private:
	/* MoveBehavior* move; */
	ShootingBehavior* shooting_behavior;
	
public:
	std::vector<sf::Vector2f*> shooting_pos;
	Enemy(const sf::Vector2f& start_pos, const std::vector<sf::Vector2f*>& shooting_pos, ShootingBehavior* shooting_behavior) : Entity(start_pos, Radii::ENEMY, Colors::ENEMY), shooting_pos(shooting_pos), shooting_behavior(shooting_behavior) {
		form.setOrigin(Radii::ENEMY, Radii::ENEMY);
	}
	void init(const sf::Vector2f& start_pos, const std::vector<sf::Vector2f*>& _shooting_pos, ShootingBehavior* _shooting_behavior) {
		form.setPosition(start_pos);
		form.setOrigin(Radii::ENEMY, Radii::ENEMY);
		shooting_pos = _shooting_pos;
		shooting_behavior = _shooting_behavior;
	}
	void rotateShootingPos(float angle);
	void update(float dt) override;
};

class Pool {
private:
	std::queue<Bullet*> bQueue;
	std::queue<Enemy*> eQueue;

public:
	Bullet* getBullet();
	Enemy* getEnemy();


	void returnBullet(Bullet* bull);
	void returnEnemy(Enemy* enem);

	void Init();
};

class EntityManager {
private:
	Player* player;
	std::vector<Bullet*> pbullets;
	std::vector<Bullet*> ebullets;
	std::vector<Enemy*> enemies;
	Pool pool;
	bool circleCollision(const Entity& c1, const Entity& c2);
	
public:
	EntityManager() {}
	void addPlayer(Entity* player);
	void enemyShoot(const sf::Vector2f &startpos, const sf::Vector2f& vel);
	void handleInput();
	void update(float dt);
	void logic();
	void render(sf::RenderTarget& g);
	void resolveWallCollision();
	void initialize();
	bool recycle(Bullet* bull);
} em;

/* class StageDirector {

}; */