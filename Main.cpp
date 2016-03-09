#include "Main.h"

#define FPS	60.0f
#define SPF sf::seconds(1.0f/FPS)

const double PI = std::acos(-1);
const auto window_height = 800;
const auto window_width = 400;

namespace Keys {
	const auto UP = 0x11;		/* w */
	const auto DOWN = 0x1f;		/* s */
	const auto LEFT = 0x1e;		/* a */
	const auto RIGHT = 0x20;	/* d */
	const auto FIRE = 0x39;		/*spacebar*/
}

int main() {
	sf::RenderWindow window(sf::VideoMode(window_width,window_height), "My Window");
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

void Score::add() {
	++score;
}

void Score::draw(sf::RenderTarget& g) {
	text.setString(SCORE + std::to_string(score));
	g.draw(text);
}

sf::Vector2f Entity::getPos() const {
	return form.getPosition();
}

void Entity::setPos(float x, float y) {
	form.setPosition(x, y);
}

float Entity::getRad() const {
	return form.getRadius();
}

void Entity::render(sf::RenderTarget& g) {
	g.draw(form);
}

void Entity::move(const sf::Vector2f &dir) {
	form.move(dir);
}

void Player::update(float dt) {
	form.move(SPEED*direction);
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
	
	if (isKeyDown(Keys::FIRE)) {
		fire();
	}
}

void Player::fire(){
	//pew pew;
	// class pool...?
}

void Player::takeDamage() {
	score.add();
}

void Player::renderScore(sf::RenderTarget& g) {
	score.draw(g);
}

void Bullet::update(float dt) {
	/* the bullet doesn't move yet. for testing purposes. */
}

void Enemy::update(float dt) {
	/* enemy's don't move for testing purposes */
}

Bullet* Pool::getBullet(){
	if(!bQueue.empty()){
		Bullet* tmp = bQueue.front();
		bQueue.pop();
		return tmp;
	}
	else{
		return new Bullet(sf::Vector2f(0, 0));
	}
}

void Pool::returnBullet(Bullet* bull){
	bQueue.push(bull);
}

Enemy* Pool::getEnemy(){
	if(!eQueue.empty()){
		Enemy* tmp = eQueue.front();
		eQueue.pop();
		return tmp;
	}
	else{
		return new Enemy(sf::Vector2f(0, 0));
	}
}

void Pool::returnEnemy(Enemy* enem){
	eQueue.push(enem);
}

bool EntityManager::circleCollision(const Entity& c1, const Entity& c2) {
	const auto c1p = c1.getPos();
	const auto c2p = c2.getPos();
	return ((c1p.x - c2p.x)*(c1p.x - c2p.x) + (c1p.y - c2p.y)*(c1p.y - c2p.y)) <= ((c1.getRad() + c2.getRad())*(c1.getRad() + c2.getRad()));
}

void EntityManager::addPlayer(Entity* player) {
	this->player = dynamic_cast<Player*>(player);
}

void EntityManager::addEnemy(Entity* enemy) {
	this->enemies.push_back(dynamic_cast<Enemy*>(enemy));
}

void EntityManager::addBullet(Entity* bullet) {
	this->ebullets.push_back(dynamic_cast<Bullet*>(bullet)); /* temporarily adding all bullets into enemy-bullets vector */
}

void EntityManager::handleInput() {
	player->handleInput();
}

void EntityManager::update(float dt) {
	player->update(dt);
}

void EntityManager::logic() {
	resolveWallCollision();

	for (int i = 0; i < ebullets.size() ; i += 0) {
		if (circleCollision(*ebullets.at(i), *player)) {
			player->takeDamage();
			ebullets.erase(ebullets.begin() + i);
		} else {
			++i;
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
	player->render(g);
	player->renderScore(g);
	
	for (Bullet* bullet : ebullets) {
		bullet->render(g);
	}
}

void EntityManager::resolveWallCollision() {
	if (player->getPos().x + player->getRad() > window_width)
		player->setPos(window_width - player->getRad(), player->getPos().y);
	else if (player->getPos().x - player->getRad() < 0)
		player->setPos(player->getRad(), player->getPos().y);
	if (player->getPos().y + player->getRad() > window_height)
		player->setPos(player->getPos().x, window_height - player->getRad());
	else if (player->getPos().y - player->getRad() < 0)
		player->setPos(player->getPos().x, player->getRad());
}

void initialize() {
	em.addPlayer(new Player(sf::Vector2f(50, 50)));
	for (int i = 0; i < window_width; i += window_width/10) {
		for (int j = 0; j < window_height; j += window_height/10) {
			em.addBullet(new Bullet(sf::Vector2f(i, j))); /* creating a stationary bullet */
		}
	}
	//instantiate 50 pbullets
	//instantiate 1000 enemies
	//instantiate 1000 ebullets
}

bool isKeyDown(const int& key) {
	short state = GetAsyncKeyState(MapVirtualKey(key, MAPVK_VSC_TO_VK_EX));
	return state >> 15 != 0;
}