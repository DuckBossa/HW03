#include "Main.h"

#define FPS	60.0f
#define SPF sf::seconds(1.0f/FPS)

const double PI = std::acos(-1);
const auto window_height = 600;
const auto window_width = 600;

int main() {
	sf::RenderWindow window(sf::VideoMode(window_width,window_height), "My Window");
	sf::Clock clock;
	sf::Time lag = sf::seconds(0);
	
	em.initialize();
	
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

void Score::add() {
	++score;
}

void Score::draw(sf::RenderTarget& g) {
	text.setString(SCORE + std::to_string(score));
	g.draw(text);
}

/* Move Behaviors */
void MoveStraight::behave(sf::CircleShape& form, float dt) {
	form.move(speed*dt*direction);
}

/* Shoot Behaviors */
void ShootStraight::behave(sf::CircleShape& form, float dt) {
	timer += dt;
	if (timer >= firing_rate*dt) {
		float rotation = form.getRotation();
		sf::Vector2f direction(std::cos(rotation*PI/180), std::sin(rotation*PI/180));
		em.enemyShoot(form.getPosition(), Speeds::BULLET*dt*direction);
		timer = 0;
	}
}

/* Rotate Behaviors */
void RotateConstantly::behave(sf::CircleShape& form, float dt) {
	form.setRotation(form.getRotation() + rotational_speed*dt);
}

/* Entity */
sf::Vector2f Entity::getPos() const {
	return form.getPosition();
}

void Entity::setPos(float x, float y) {
	form.setPosition(x, y);
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

/* Bullets */
void Bullet::update(float dt) {
	move_behavior->behave(form, dt);
}

/* Player */
void Player::update(float dt) {
	form.move(Speeds::PLAYER*dt*direction);
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

void Player::fire() {
	if (isKeyDown(Keys::FIRE)) {
		
	}
}

void Player::takeDamage() {
	score.add();
}

void Player::renderScore(sf::RenderTarget& g) {
	score.draw(g);
}

/* Enemy */
void Enemy::update(float dt) {
	rotate_behavior->behave(form, dt);
	shoot_behavior->behave(form, dt);
}

/* Pool */
Bullet* Pool::getBullet() {
	if (!bQueue.empty()) {
		Bullet* tmp = bQueue.front();
		bQueue.pop();
		return tmp;
	} else {
		return nullptr;
	}
}

void Pool::returnBullet(Bullet* bull) {
	bQueue.push(bull);
}

Enemy* Pool::getEnemy() {
	if (!eQueue.empty()) {
		Enemy* tmp = eQueue.front();
		eQueue.pop();
		return tmp;
	} else {
		return nullptr;
	}
}

void Pool::returnEnemy(Enemy* enem) {
	eQueue.push(enem);
}

void Pool::Init() {
	for (int i = 0; i < 2000; ++i) {
		bQueue.push(new Bullet());
	}
}

/* Entity Manager */
bool EntityManager::circleCollision(const Entity& c1, const Entity& c2) {
	const auto c1p = c1.getPos();
	const auto c2p = c2.getPos();
	return ((c1p.x - c2p.x)*(c1p.x - c2p.x) + (c1p.y - c2p.y)*(c1p.y - c2p.y)) <= ((c1.getRad() + c2.getRad())*(c1.getRad() + c2.getRad()));
}

void EntityManager::addPlayer(Entity* player) {
	this->player = dynamic_cast<Player*>(player);
}

void EntityManager::handleInput() {
	player->handleInput();
}

void EntityManager::update(float dt) {
	player->update(dt);
	for (auto bullet : ebullets) {
		bullet->update(dt);
	}
	for (auto enemy : enemies) {
		enemy->update(dt);
	}
	std::cout << "Num of bullets: " << ebullets.size() << "\n";
}

void EntityManager::logic() {
	resolveWallCollision();
	for (int i = ebullets.size() - 1; i >= 0; i--) {
		Bullet* bullet = ebullets[i];
			if (circleCollision(*bullet, *player)) {
				player->takeDamage();
				ebullets.erase(ebullets.begin() + i);
				pool.returnBullet(bullet);
			}
			if (recycle(bullet)) {
				ebullets.erase(ebullets.begin() + i);
				pool.returnBullet(bullet);
			}
	}
/*
	for (auto bullet : pbullets) {
		for (auto enemy : enemies) {
			if (circleCollision(*bullet, *enemy)) {
				//destroy enemy
			}
		}
	}
*/
}

void EntityManager::render(sf::RenderTarget& g) {
	player->render(g);
	player->renderScore(g);
	
	for (auto bullet : ebullets) {
		bullet->render(g);
	}
	for (auto enemy : enemies) {
		enemy->render(g);
	}
}

void EntityManager::enemyShoot(const sf::Vector2f &startpos, const sf::Vector2f& vel) {
	Bullet* temp = pool.getBullet();
	temp->init(startpos, new MoveStraight(Speeds::BULLET, vel));
	ebullets.push_back(temp);
}

bool EntityManager::recycle(Bullet* bull) {
	return (bull->getPos().x - bull->getRad() > window_width) || (bull->getPos().x + bull->getRad() < 0) || (bull->getPos().y + bull->getRad() < 0) || (bull->getPos().y - bull->getRad() > window_height);
}

void EntityManager::resolveWallCollision() {
	if (player->getPos().x + player->getRad() > window_width) {
		player->setPos(window_width - player->getRad(), player->getPos().y);
	} else if (player->getPos().x - player->getRad() < 0) {
		player->setPos(player->getRad(), player->getPos().y);
	}
	if (player->getPos().y + player->getRad() > window_height) {
		player->setPos(player->getPos().x, window_height - player->getRad());
	} else if (player->getPos().y - player->getRad() < 0) {
		player->setPos(player->getPos().x, player->getRad());
	}
}

void EntityManager::initialize() {
	addPlayer(new Player(sf::Vector2f(window_width/2, window_height*3.0/4.0)));
	pool.Init();
	enemies.push_back(new Enemy(sf::Vector2f(window_width/2, window_height/4), new ShootStraight(FiringRates::ENEMY, Colors::EBULLET), new RotateConstantly(RotationalSpeeds::ENEMY)));
}