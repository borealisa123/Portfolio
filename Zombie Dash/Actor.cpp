#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

/////////////////////////////////////////////////////////
// Actor
/////////////////////////////////////////////////////////
Actor::Actor(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: GraphObject(imageID, startX, startY, dir, depth, size) {
	m_alive = true;
	m_world = world;
}

// returns true if object is alive
bool Actor::isAlive() {
	return m_alive;
}

// sets object status to not alive (dead)
void Actor::makeDead() {
	m_alive = false;
}

// return pointer to world
StudentWorld* Actor::getWorld() {
	return m_world;
}

// default - no;	exception  - humans
bool Actor::canBeInfected() {
	return false;
}

// default - no;	exceptions - humans, zomies, goodies, landmines
bool Actor::canBeDestroyed() {
	return false;
}

// default - no;	exceptions - walls, exits
bool Actor::blocksFlames() {
	return false;
}

// default - no;	exception  - zombies
bool Actor::isDangerous() {
	return false;
}

// to be overridden by things
void Actor::activate(Actor* o) {
}

// to be overridden by humans
void Actor::useExit() {
}

// to be overridden by humans (Citizen and penelope) and zombies for the sounds
void Actor::beMurdered() {
	makeDead();
}

// changes coordinates by dist amount in the dir direction
void Actor::switchCoords(double& x, double&y, double dist, Direction dir) {
	switch (dir) {
	case up:
		y += dist;
		break;
	case down:
		y -= dist;
		break;
	case left:
		x -= dist;
		break;
	case right:
		x += dist;
		break;
	}
}


/////////////////////////////////////////////////////////
// Wall
/////////////////////////////////////////////////////////

// wall constructor
Wall::Wall(double x, double y, StudentWorld* world)
	: Actor(IID_WALL, x, y, world, right, 0) {
}

void Wall::doSomething() {
	// I am a wall
	// Walls don't do things
	// Therefore, I don't do things
}

// yes they do
bool Wall::blocksFlames() {
	return true;
}

// yes, they have a bound box
bool Wall::checkIntersect() {
	return true;
}

/////////////////////////////////////////////////////////
// Bounded
/////////////////////////////////////////////////////////
Bounded::Bounded(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: Actor(imageID, startX, startY, world, dir, depth, size), m_paralyzed(false) {
}

// yes, they're bounded
bool Bounded::checkIntersect() {
	return true;
}

// checks if moving is a direction is valid and moves
bool Bounded::moveMyself(double x, double y) {
	if (!getWorld()->doesIntersect(x, y, this)) {
		moveTo(x, y);
		return true;
	}
	return false;
}

// toggles paralyzed variable and returns its value before toggling
bool Bounded::isParalyzed() {
	m_paralyzed = !m_paralyzed;
	return !m_paralyzed;
}

// haha smart zombies are not so smart
bool Bounded::beSmarter(Actor* o, Direction d) {
	return true;
}

// returns true if direction was set (and has moved for citizens)
bool Bounded::setSmartDir(Actor* o, Actor* t) {
	double x1 = o->getX();
	double y1 = o->getY();
	double x2 = t->getX();
	double y2 = t->getY();
	// same column
	if (x1 == x2) {
		if (y1 < y2 && beSmarter(o, up)) {
			setDirection(up);
			return true;
		}
		else if (y1 > y2 && beSmarter(o, down)) {
			setDirection(down);
			return true;
		}
		else {
			return false;
		}
	}
	// same row
	if (y1 == y2) {
		if (x1 < x2 && beSmarter(o, right)) {
			setDirection(right);
			return true;
		}
		else if (x1 > x2 && beSmarter(o, left)){
			setDirection(left);
			return true;
		}
		else {
			return false;
		}
	}
	// neither same row nor same column, do random
	int rand = randInt(1, 2);
	// southwest
	if (x1 < x2 && y1 < y2) {
		if (rand == 1 && beSmarter(o, up)) {
			setDirection(up);
			return true;
		}
		else if (beSmarter(o, right)){
			setDirection(right);
			return true;
		}
		// it's repetitious but necessary :(
		else if (beSmarter(o, up)) {
			setDirection(up);
			return true;
		}
		else {
			return false;
		}
	}
	// northwest
	if (x1 < x2 && y1 > y2) {
		// if citizen can't move down, move right
		if (rand == 1 && beSmarter(o, down)) {
			setDirection(down);
			return true;
		}
		// if moving right is not possible
		else if (beSmarter(o, right)) {
			setDirection(right);
			return true;
		}
		// it's repetitious but necessary :(
		// without it, if rand == 2, then citizen would not move at
		// all if moving right is not possible, even if moving down is
		else if (beSmarter(o, down)) {
			setDirection(down);
			return true;
		}
		else {
			return false;
		}
	}
	// northeast
	if (x1 > x2 && y1 > y2) {
		if (rand == 1 && beSmarter(o, down)) {
			setDirection(down);
			return true;
		}
		else if (beSmarter(o, left)) {
			setDirection(left);
			return true;
		}
		// it's repetitious but necessary :(
		else if (beSmarter(o, down)) {
			setDirection(down);
			return true;
		}
		else {
			return false;
		}
	}
	// southeast
	if (x1 > x2 && y1 < y2) {
		if (rand == 1 && beSmarter(o, up)) {
			setDirection(up);
			return true;
		}
		else if (beSmarter(o, left)) {
			setDirection(left);
			return true;
		}
		// it's repetitious but necessary :(
		else if (beSmarter(o, up)) {
			setDirection(up);
			return true;
		}
		else {
			return false;
		}
	}
	// not possible for x1 == x2 && y1 == y2, but just in case?
	return false;
}

/////////////////////////////////////////////////////////
// Human
/////////////////////////////////////////////////////////
Human::Human(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: Bounded(imageID, startX, startY, world, dir, depth, size) {
	m_infected = false;
	m_infectionCount = 0;
}

// yes
bool Human::canBeInfected() {
	return true;
}

// yes
bool Human::canBeDestroyed() {
	return true;
}

// returns true if person has turned
bool Human::doInfectionStuff() {
	if (m_infected) {
		m_infectionCount++;
		if (m_infectionCount >= 500) {
			turn();
			return true;
		}
	}
	return false;
}

// for Penelope to use a vaccine on herself
void Human::setInfectionStatus(bool b) {
	m_infected = b;
}

// for Penelope to use a vaccine on herself
void Human::clearInfectionCount() {
	m_infectionCount = 0;
}

// aka become infected
void Human::activate(Actor* o) {
	if (!isAlive()) {
		return;
	}
	if (!m_infected && this != getWorld()->getPenelope()) {
		getWorld()->playSound(SOUND_CITIZEN_INFECTED);
	}
	setInfectionStatus(true);
}

// to display Penelop's infection count
int Human::getInfectionCount() {
	return m_infectionCount;
}

/////////////////////////////////////////////////////////
// Penelope
/////////////////////////////////////////////////////////
Penelope::Penelope(double x, double y, StudentWorld* world)
	: Human(IID_PLAYER, x, y, world, right, 0) {
	/*	for testing (cough cheating) purposes
	m_mines = 5;
	m_gas = 5;
	m_vaccines = 5;
	*/
	m_mines = 0;
	m_gas = 0;
	m_vaccines = 0;
}

// make three flame objects in front, if possible
void Penelope::useFlame() {
	m_gas--;
	getWorld()->playSound(SOUND_PLAYER_FIRE);
	Direction d = getDirection();
	double x = getX();
	double y = getY();
	for (int i = 1; i <= 3; i++) {
		switchCoords(x, y, SPRITE_HEIGHT, d);
		Actor* f = new Flame(x, y, getWorld(), getDirection());
		if (!getWorld()->flameBlocked(f)) {
			getWorld()->addActor(f);
		}
		else {
			delete f;
			break;
		}
	}
}

// create landmine at current coordinates
void Penelope::useMine() {
	m_mines--;
	Actor* m = new Landmine(getX(), getY(), getWorld());
	getWorld()->addActor(m);
}

// uninfect self
void Penelope::useVacc() {
	m_vaccines--;
	setInfectionStatus(false);
	clearInfectionCount();
	// does infection count get reset? apparently yes
}

// self explanatory
void Penelope::addMine() {
	m_mines += 2;
}

// self explanatory
void Penelope::addGas() {
	m_gas += 5;
}

// self explanatory
void Penelope::addVacc() {
	m_vaccines++;
}

// self explanatory
int Penelope::getVaccs() {
	return m_vaccines;
}

// self explanatory
int Penelope::getFlames() {
	return m_gas;
}

// self explanatory
int Penelope::getMines() {
	return m_mines;
}

// she just dies
void Penelope::turn() {
	beMurdered();
}

// everything Penelope has to do
void Penelope::doSomething() {
	if (!isAlive()) {
		return;
	}
	if (doInfectionStuff()) {
		return;
	}
	int key = 0;
	getWorld()->getKey(key);
	switch (key) {
		// flamethrower
	case KEY_PRESS_SPACE:
		if (m_gas > 0) {
			useFlame();
		}
		break;
		// landmine
	case KEY_PRESS_TAB:
		if (m_mines > 0) {
			useMine();
		}
		break;
		// vaccine
	case KEY_PRESS_ENTER:
		if (m_vaccines > 0) {
			useVacc();
		}
		break;
		// move in a direction
	case KEY_PRESS_UP:
		setDirection(up);
		moveMyself(getX(), getY() + 4);
		break;
	case KEY_PRESS_DOWN:
		setDirection(down);
		moveMyself(getX(), getY() - 4);
		break;
	case KEY_PRESS_LEFT:
		setDirection(left);
		moveMyself(getX() - 4, getY());
		break;
	case KEY_PRESS_RIGHT:
		setDirection(right);
		moveMyself(getX() + 4, getY());
		break;
	}
}

// level complete!
void Penelope::useExit() {
	getWorld()->playSound(SOUND_LEVEL_FINISHED);
	getWorld()->levelComplete();
}

// scream, die
void Penelope::beMurdered() {
	if (!isAlive()) {
		return;
	}
	getWorld()->playSound(SOUND_PLAYER_DIE);
	makeDead();
}

/////////////////////////////////////////////////////////
// Citizen
/////////////////////////////////////////////////////////
Citizen::Citizen(double x, double y, StudentWorld* world)
	: Human(IID_CITIZEN, x, y, world, right, 0) {
}

// everything a citizen has to do
void Citizen::doSomething() {
	if (!isAlive()) {
		return;
	}
	if (doInfectionStuff()) {
		return;
	}
	if (isParalyzed()) {
		return;
	}
	double sqd_z;
	double sqd_p;
	// get the two distances
	getWorld()->followOrRun(getX(), getY(), sqd_z, sqd_p);
	if ((sqd_z < 0 || sqd_p < sqd_z) && sqd_p <= 6400) {
		// this returns true if citizen has already moved
		if (setSmartDir(this, getWorld()->getPenelope())) {
			return;
		}
	}
	if (sqd_z >= 0 && sqd_z <= 6400) {
		// this makes citizen move/stay, depending on which is better in this situation
		calcRunWhere(getX(), getY(), sqd_z);
	}
}

// play the sound, increase score, be dead but not murdered
void Citizen::useExit() {
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
	getWorld()->increaseScore(500);
	makeDead();
}

// scream, lose points, die
void Citizen::beMurdered() {
	if (!isAlive()) {
		return;
	}
	getWorld()->playSound(SOUND_CITIZEN_DIE);
	getWorld()->increaseScore(-1000);
	makeDead();
}

// play sfx, lose points, make new zombie, die
void Citizen::turn() {
	getWorld()->playSound(SOUND_ZOMBIE_BORN);
	getWorld()->increaseScore(-1000);
	int rand = randInt(1, 10);
	Actor* z;
	if (rand > 7) {
		z = new SmartZombie(getX(), getY(), getWorld());
	}
	else {
		z = new DumbZombie(getX(), getY(), getWorld());
	}
	getWorld()->addActor(z);
	makeDead();
}

// citizens are extra smart
// returns true if can move (and has moved) in that direction
bool Citizen::beSmarter(Actor* o, Direction d) {
	double x = getX();
	double y = getY();
	switchCoords(x, y, 2, d);
	return moveMyself(x, y);
}

// figure out which direction makes citizen go farthest from zombeis
bool Citizen::calcRunWhere(double x, double y, double osqd_z) {
	double newsqd_z = 0;
	double temp;
	Direction d;
	double trash;
	// up
	if (!getWorld()->doesIntersect(x, y + 2, this)) {
		getWorld()->followOrRun(x, y + 2, temp, trash);
		if (temp > newsqd_z) {
			newsqd_z = temp;
			d = up;
		}
	}
	// down
	if (!getWorld()->doesIntersect(x, y - 2, this)) {
		getWorld()->followOrRun(x, y - 2, temp, trash);
		if (temp > newsqd_z) {
			newsqd_z = temp;
			d = down;
		}
	}
	// left
	if (!getWorld()->doesIntersect(x - 2, y, this)) {
		getWorld()->followOrRun(x - 2, y, temp, trash);
		if (temp > newsqd_z) {
			newsqd_z = temp;
			d = left;
		}
	}
	// right
	if (!getWorld()->doesIntersect(x + 2, y, this)) {
		getWorld()->followOrRun(x + 2, y, temp, trash);
		if (temp > newsqd_z) {
			newsqd_z = temp;
			d = right;
		}
	}
	// check that moving is better than not moving
	if (newsqd_z > osqd_z) {
		setDirection(d);
		switchCoords(x, y, 2, d);
		moveMyself(x, y);
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////
// Zombies
/////////////////////////////////////////////////////////
Zombies::Zombies(double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: Bounded(IID_ZOMBIE, startX, startY, world, right, 0), m_movePlan(0) {
}

// yes, die
bool Zombies::canBeDestroyed() {
	return true;
}

// yes
bool Zombies::isDangerous() {
	return true;
}

// everything a zombie can do
void Zombies::doSomething() {
	if (!isAlive() || isParalyzed()) {
		return;
	}
	double x, y;
	Direction d = getDirection();
	getVomitCoords(this, x, y);
	if (getWorld()->shouldVomit(x, y)) {
		// 1/3 chance of vomitting
		int rand = randInt(1, 3);
		if (rand == 1) {
			vomit(x, y, d);
			return;
		}
	}
	// make a move plan
	if (m_movePlan == 0) {
		m_movePlan = randInt(3, 10);
		// dumb zombies do dumb stuff, smart zombies be smart
		doSomethingUnique();
	}
	// I can reuse these variables
	x = getX();
	y = getY();
	// move by following move plan
	switchCoords(x, y, 1, getDirection());
	if (moveMyself(x, y)) {
		m_movePlan--;
	}
	else {
		m_movePlan = 0;
	}
}

// make the vomit, play sfx
void Zombies::vomit(double x, double y, Direction dir) {
	getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
	Actor* v = new Vomit(x, y, getWorld(), dir);
	getWorld()->addActor(v);
}

// get coords for where vomit would go
void Zombies::getVomitCoords(Actor* z, double& x, double& y) {
	Direction d = z->getDirection();
	x = z->getX();
	y = z->getY();
	switchCoords(x, y, SPRITE_HEIGHT, d);
}

// play sfx, gain points, die
void Zombies::beMurdered() {
	if (!isAlive()) {
		return;
	}
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	awardPoints();
	makeDead();
}

// self explanatory
Direction Zombies::getRandDir() {
	int rand = randInt(1, 4);
	switch (rand) {
	case 1:
		return up;
		break;
	case 2:
		return down;
		break;
	case 3:
		return left;
		break;
	case 4:
		return right;
		break;
	}
	// should always be one of those, but just in case
	return right;
}

/////////////////////////////////////////////////////////
// Dumb Zombies
/////////////////////////////////////////////////////////
DumbZombie::DumbZombie(double x, double y, StudentWorld* world)
	: Zombies(x, y, world) {
}

// just move randomly
void DumbZombie::doSomethingUnique() {
	setDirection(getRandDir());
}

// get 1000 points! and maybe drop a vaccine
void DumbZombie::awardPoints() {
	getWorld()->increaseScore(1000);
	int rand = randInt(1, 10);
	if (rand == 1) {
		dropVacc();
	}
}

// randomly throw a vaccine
void DumbZombie::dropVacc() {
	Direction d = getRandDir();
	double x = getX();
	double y = getY();
	// move the vaccine by SPRITE_HEIGHT or SPRITE_WIDTH
	if (d == up || d == down) {
		switchCoords(x, y, SPRITE_HEIGHT, d);
	}
	else {
		switchCoords(x, y, SPRITE_WIDTH, d);
	}
	Actor* v = new Vaccine(x, y, getWorld());
	// if the vaccine won't overlap with anything
	if (!getWorld()->doesOverlap(v)) {
		getWorld()->addActor(v);
	}
	else {
		delete v;
	}
}

/////////////////////////////////////////////////////////
// Smart Zombies
/////////////////////////////////////////////////////////
SmartZombie::SmartZombie(double x, double y, StudentWorld* world)
	: Zombies(x, y, world) {
}

// go towards target if close enough, or just do random stuff
void SmartZombie::doSomethingUnique() {
	Actor* target;
	if (getWorld()->shouldPursue(this, target)) {
		setSmartDir(this, target);
	}
	else {
		setDirection(getRandDir());
	}
}

// 2000 points!
void SmartZombie::awardPoints() {
	getWorld()->increaseScore(2000);
}

/////////////////////////////////////////////////////////
// Unbounded
/////////////////////////////////////////////////////////
Unbounded::Unbounded(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: Actor(imageID, startX, startY, world, dir, depth, size) {
}

// no, that's why they're called unbounded
bool Unbounded::checkIntersect() {
	return false;
}

/////////////////////////////////////////////////////////
// Exit
/////////////////////////////////////////////////////////
Exit::Exit(double x, double y, StudentWorld* world)
	: Unbounded(IID_EXIT, x, y, world, right, 1) {
}

// wait for something to activate this
void Exit::doSomething() {
	getWorld()->doOnOverlap(this);
}

// yes
bool Exit::blocksFlames() {
	return true;
}

// let humans use it
void Exit::activate(Actor* o) {
	// return if not human
	if (!o->canBeInfected()) {
		return;
	}
	// if not penelope
	if (o != getWorld()->getPenelope()) {
		o->useExit();
	}
	// if no more humans
	else if (getWorld()->noHumans()) {
		o->useExit();
	}
}

/////////////////////////////////////////////////////////
// Goodies
/////////////////////////////////////////////////////////
Goodies::Goodies(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size)
	: Unbounded(imageID, startX, startY, world, right, 1) {
}

// yes
bool Goodies::canBeDestroyed() {
	return true;
}

// if penelope is overlapping, pass on to getPickedUp with penelope pointer
void Goodies::activate(Actor* o) {
	if (!isAlive()) {
		return;
	}
	if (o == getWorld()->getPenelope()) {
		getPickedUp(getWorld()->getPenelope());
	}
}

// get picked up by penelope, play sfx, 50 points, die
void Goodies::getPickedUp(Penelope* p) {
	getWorld()->increaseScore(50);
	makeDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	giveSpecificGoodie(p);
}

// wait for something to activate it
void Goodies::doSomething() {
	if (!isAlive()) {
		return;
	}
	getWorld()->doOnOverlap(this);
}

/////////////////////////////////////////////////////////
// Vaccine
/////////////////////////////////////////////////////////
Vaccine::Vaccine(double x, double y, StudentWorld* world)
	: Goodies(IID_VACCINE_GOODIE, x, y, world) {
}

// addVacc()
void Vaccine::giveSpecificGoodie(Penelope* p) {
	p->addVacc();
}

/////////////////////////////////////////////////////////
// Gas Can
/////////////////////////////////////////////////////////
GasCan::GasCan(double x, double y, StudentWorld* world)
	: Goodies(IID_GAS_CAN_GOODIE, x, y, world) {
}

// addGas()
void GasCan::giveSpecificGoodie(Penelope* p) {
	p->addGas();
}

/////////////////////////////////////////////////////////
// Landmine Goodie
/////////////////////////////////////////////////////////
LandmineGoodie::LandmineGoodie(double x, double y, StudentWorld* world)
	: Goodies(IID_LANDMINE_GOODIE, x, y, world) {
}

// addMine()
void LandmineGoodie::giveSpecificGoodie(Penelope* p) {
	p->addMine();
}

/////////////////////////////////////////////////////////
// Flame
/////////////////////////////////////////////////////////
Flame::Flame(double x, double y, StudentWorld* world, Direction dir)
	: Unbounded(IID_FLAME, x, y, world, dir, 0), m_life(2) {
}

// die after two ticks
void Flame::expire() {
	if (m_life > 0) {
		m_life--;
	}
	else {
		makeDead();
	}
}

// murder what can be murdered
void Flame::activate(Actor* o) {
	if (!o->canBeDestroyed()) {
		return;
	}
	o->beMurdered();
}

// wait for overlap until death
void Flame::doSomething() {
	expire();
	if (!isAlive()) {
		return;
	}
	getWorld()->doOnOverlap(this);
}

/////////////////////////////////////////////////////////
// Vomit
/////////////////////////////////////////////////////////
Vomit::Vomit(double x, double y, StudentWorld* world, Direction dir)
	: Unbounded(IID_VOMIT, x, y, world, dir, 0), m_life(2) {
}

// wait for overlap until death
void Vomit::doSomething() {
	expire();
	if (!isAlive()) {
		return;
	}
	getWorld()->doOnOverlap(this);
}

// die after two ticks
void Vomit::expire() {
	if (m_life > 0) {
		m_life--;
	}
	else {
		makeDead();
	}
}

// infect humans
void Vomit::activate(Actor* o) {
	if (!isAlive()) {
		return;
	}
	if (o->canBeInfected()) {
		o->activate(this);
	}
}

/////////////////////////////////////////////////////////
// Pit
/////////////////////////////////////////////////////////
Pit::Pit(double x, double y, StudentWorld* world) 
	: Unbounded(IID_PIT, x, y, world, right, 0) {
}

// wait for overlap
void Pit::doSomething() {
	getWorld()->doOnOverlap(this);
}

// murder anything that can be murdered
void Pit::activate(Actor* o) {
	if (o->canBeInfected() || o->isDangerous()) {
		o->beMurdered();
	}
}

/////////////////////////////////////////////////////////
// Landmine
/////////////////////////////////////////////////////////
Landmine::Landmine(double x, double y, StudentWorld* world) 
	: Unbounded(IID_LANDMINE, x, y, world, right, 1), m_safety(30) {
}

// yes
bool Landmine::canBeDestroyed() {
	return true;
}

// murder what can be murdered
void Landmine::activate(Actor* o) {
	if (!isAlive()) {
		return;
	}
	if (o->canBeInfected() || o->isDangerous()) {
		beMurdered();
	}
}

// overriding this one so when the flames destroy it, it'll work
void Landmine::beMurdered() {
	// prevent infinite loop
	if (!isAlive()) {
		return;
	}
	makeDead();
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	double x = getX();
	double y = getY();
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			Actor* a;
			// add a pit
			if (i == 0 && j == 0) {
				a = new Pit(x, y, getWorld());
				getWorld()->addActor(a);
			}
			// add 9 flames
			a = new Flame(x + i * SPRITE_WIDTH, y + j * SPRITE_HEIGHT, getWorld(), up);
			if (!getWorld()->flameBlocked(a)) {
				getWorld()->addActor(a);
			}
			else {
				delete a;
			}
		}
	}
}

// wait to be activated
void Landmine::doSomething() {
	if (!isAlive()) {
		return;
	}
	if (m_safety > 0) {
		m_safety--;
		return;
	}
	getWorld()->doOnOverlap(this);
}