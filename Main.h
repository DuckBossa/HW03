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
	const auto FIRE = 0x39;		/*spacebar*/
}

namespace COLORS {
	const auto PLAYER = sf::Color::Blue;
	const auto PBULLET = sf::Color::White;
	const auto ENEMY = sf::Color::Red;
	const auto EBULLET = sf::Color::Magenta;
}

namespace SPEEDS {
	const auto PLAYER = 5.0f;
	const auto BULLETS = 250.0f;
	const auto ENEMY = 3.0f;
}

namespace RADII {
	const auto PLAYER = 10.0f;
	const auto ENEMY = 15.0f;
	const auto EBULLET = 5.0f;
	const auto PBULLET = 5.0f;
}

namespace FIRINGRATES {
	const auto PLAYER = 0.5f;
}

using namespace std;

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
	Entity(){}
	Entity(float radius,const sf::Color &col) : form(radius) {
		form.setFillColor(col);
	}
	sf::Vector2f getPos() const;
	void setPos(float x, float y);
	float getRad() const;
	bool isKeyDown(const int& key);
	virtual void update(float dt) = 0;
	void render(sf::RenderTarget& g);
	void moveEntity(const sf::Vector2f &dir) {
		form.move(dir);
	}
	~Entity(){}
};

class Player : public Entity {
	private:
	static constexpr float SPEED = 5.0f;
	sf::Vector2f direction;
	int spf;//shots per frame
	Score score;
	
	public:
	Player(const sf::Vector2f &start_pos) : Entity(RADII::PLAYER,COLORS::PLAYER), direction(0, 0) {
		form.setPosition(start_pos);
		form.setOrigin(sf::Vector2f(RADII::PLAYER, RADII::PLAYER));
	}
	void handleInput();
	void fire();
	void takeDamage();
	void update(float dt) override;
	void renderScore(sf::RenderTarget& g);
};

class MoveBehavior {
public:
	virtual void behave(Entity* e, float dt) = 0;
};

class MoveForwardBehavior : public MoveBehavior{
private:
	float mag;
public:
	MoveForwardBehavior(float mag) : mag(mag){}
	void behave(Entity* e,float dt) override;
};


class ShootingBehavior {
public:
	ShootingBehavior(){}
	~ShootingBehavior(){}
	virtual void shoot(float dt,Entity* e) = 0;
};

class CircularShoot : public ShootingBehavior{
private:
	float rotatePerSec;
	float angleToRotate;
	float firingRate;
	float timerR;
	float timerS;
public:
	CircularShoot(float rotatePerSec, float angleToRotate,float firingRate) :rotatePerSec(rotatePerSec), angleToRotate(angleToRotate),firingRate(firingRate){
		timerR = timerS = 0;
	}
	void shoot(float dt,Entity* e) override;
};


class Bullet : public Entity {
private:
	sf::Vector2f vel;
public:
	Bullet() {}
	Bullet(float radius, const sf::Color &col, const sf::Vector2f &startpos, const sf::Vector2f &vel) : Entity(radius, col), vel(vel) {
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		form.setPosition(startpos);
	}
	void init(float radius, const sf::Color &col, const sf::Vector2f &startpos, const sf::Vector2f &vel) {
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		form.setPosition(startpos);
		this->vel = vel;
	}
	void update(float dt) override;
};
class Enemy : public Entity {
	//MoveBehavior* move;
	ShootingBehavior* shoot;
public:
	//sf::Vector2f shootingpos;
	std::vector<sf::Vector2f*>shootingPos;
	Enemy(float radius, const sf::Color &col, const sf::Vector2f &startpos, const std::vector<sf::Vector2f*> &shootingpos,ShootingBehavior* shoot) : Entity(radius,col), shootingPos(shootingpos), shoot(shoot){
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		form.setPosition(startpos);
	}
	void init(float radius, const sf::Color &col, const sf::Vector2f &startpos, const std::vector<sf::Vector2f*> &shootingpos, ShootingBehavior* shoot) {
		form.setRadius(radius);
		form.setOrigin(radius, radius);
		form.setFillColor(col);
		form.setPosition(startpos);
		this->shootingPos = shootingpos;
	}

	void rotateShootingPos(float angle);
	void update(float dt) override;
};
class Pool{
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
	void enemyShoot(const sf::Vector2f &startpos, const sf::Vector2f vel);
	void handleInput();
	void update(float dt);
	void logic();
	void render(sf::RenderTarget& g);
	void resolveWallCollision();
	void initialize();
	bool recycle(Bullet* bull);
} em;


class StageDirector {

};