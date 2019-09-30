#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include "GraphObject.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_complete(false), m_Penelope(nullptr) {
}

// set m_complete to be true so move() will know when to finish
void StudentWorld::levelComplete() {
	m_complete = true;
}

// returns penelope pointer
Penelope* StudentWorld::getPenelope() const {
	return m_Penelope;
}

// add an actor~~
void StudentWorld::addActor(Actor* a) {
	m_actors.push_back(a);
}

// check that there are no more surviving humans in the world (other than penelope)
bool StudentWorld::noHumans() const {
	list<Actor*>::const_iterator it = m_actors.begin();
	it++;
	while (it != m_actors.end()) {
		if ((*it)->canBeInfected() && (*it)->isAlive()) {
			return false;
		}
		it++;
	}
	return true;
}

// check if this object intersects with another one (bound box)
bool StudentWorld::doesIntersect(double x, double y, Actor* o) const {
	list<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end()) {
		if ((*it) != o && (*it)->checkIntersect()) {
			double x2 = (*it)->getX();
			double y2 = (*it)->getY();
			// check if bottom left corner of o is in the bounding box of another object
			if (x >= x2 && x < x2 + SPRITE_WIDTH &&
				y >= y2 && y < y2 + SPRITE_HEIGHT) {
				return true;
			}
			x += SPRITE_HEIGHT - 1;
			// check bottom right corner of o
			if (x >= x2 && x < x2 + SPRITE_WIDTH &&
				y >= y2 && y < y2 + SPRITE_HEIGHT) {
				return true;
			}
			// check top right corner of o
			y += SPRITE_HEIGHT - 1;
			if (x >= x2 && x < x2 + SPRITE_WIDTH &&
				y >= y2 && y < y2 + SPRITE_HEIGHT) {
				return true;
			}
			// check top left corner of o
			x -= SPRITE_HEIGHT - 1;
			if (x >= x2 && x < x2 + SPRITE_WIDTH &&
				y >= y2 && y < y2 + SPRITE_HEIGHT) {
				return true;
			}
			// go back to bottom left corner of o
			y -= SPRITE_HEIGHT - 1;
		}
		it++;
	}
	// no intersect
	return false;
}

// activates any objects that overlap with Actor o
void StudentWorld::doOnOverlap(Actor* o) {
	// does getX return the center or bottom left corner?
	// doesn't matter unless the sizes of the sprites are different, which they aren't
	list<Actor*>::iterator it = m_actors.begin();
	bool doesOverlap = false;
	while (it != m_actors.end()) {
		if ((*it) != o) {
			double x2 = (*it)->getX();
			double y2 = (*it)->getY();
			double deltaX = o->getX() - x2;
			double deltaY = o->getY() - y2;
			if (deltaX * deltaX + deltaY * deltaY <= 100) {
				// activate the object upon overlap
				o->activate(*it);
				doesOverlap = true;
			}
		}
		it++;
	}
	// return doesOverlap;
}

// checks if any objects overlap with Actor o
bool StudentWorld::doesOverlap(Actor* o) const {
	list<Actor*>::const_iterator it = m_actors.begin();
	bool doesOverlap = false;
	while (it != m_actors.end()) {
		if ((*it) != o) {
			double x2 = (*it)->getX();
			double y2 = (*it)->getY();
			double deltaX = o->getX() - x2;
			double deltaY = o->getY() - y2;
			if (deltaX * deltaX + deltaY * deltaY <= 100) {
				return true;
			}
		}
		it++;
	}
	return false;
}

// checks if there are any humans that overlap with these coords
bool StudentWorld::shouldVomit(double x, double y) const {
	list<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end()) {
		if ((*it)->canBeInfected()) {		// humans
			double deltaX = (*it)->getX() - x;
			double deltaY = (*it)->getY() - y;
			if (deltaX * deltaX + deltaY * deltaY <= 100) {
				return true;
			}
		}
		it++;
	}
	return false;
}

// checks if any human is in 80 px range of o
// sets target equal to pointer to that human
bool StudentWorld::shouldPursue(Actor* o, Actor*& target) const {
	double min = 6401;		// won't return true unless someone is in range
	list<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end()) {
		if ((*it)->canBeInfected()) {
			double x2 = (*it)->getX();
			double y2 = (*it)->getY();
			double deltaX = o->getX() - x2;
			double deltaY = o->getY() - y2;
			double dist = deltaX * deltaX + deltaY * deltaY;
			if (dist < min) {
				min = dist;
				target = *it;
			}
		}
		it++;
	}
	if (min <= 6400) {
		return true;
	}
	return false;
}

// i have no clue why flames are overlap rather than using the bounding box, but eh
// checks if flames will overlap with anything
bool StudentWorld::flameBlocked(Actor* o) const {
	list<Actor*>::const_iterator it = m_actors.begin();
	while (it != m_actors.end()) {
		if (!(*it)->blocksFlames()) {
			it++;
			continue;
		}
		if ((*it) != o) {
			double x2 = (*it)->getX();
			double y2 = (*it)->getY();
			double deltaX = o->getX() - x2;
			double deltaY = o->getY() - y2;
			// check if overlap
			if (deltaX * deltaX + deltaY * deltaY <= 100) {
				return true;
			}
		}
		it++;
	}
	return false;
}

// calculates distance between the coordinates (x,y) and penelope
// calculates distance between the coordinates (x,y) and the nearest zombie
void StudentWorld::followOrRun(double x, double y, double& sqdist_z, double& sqdist_p) const {
	// penelope
	list<Actor*>::const_iterator it = m_actors.begin();
	double x2 = (*it)->getX();
	double y2 = (*it)->getY();
	double deltaX = x - x2;
	double deltaY = y - y2;
	sqdist_p = deltaX * deltaX + deltaY * deltaY;
	sqdist_z = -1;		// if no zombies
	it++;

	// zombies
	while (it != m_actors.end()) {
		if ((*it)->isDangerous()) {
			x2 = (*it)->getX();
			y2 = (*it)->getY();
			deltaX = x - x2;
			deltaY = y - y2;
			double dist = deltaX * deltaX + deltaY * deltaY;
			if (sqdist_z == -1 || dist < sqdist_z) {
				sqdist_z = dist;
			}
		}
		it++;
	}
}

// delete dead actors, excluding penelope, and remove pointers from list
void StudentWorld::removeDead() {
	list<Actor*>::iterator it = m_actors.begin();
	it++;
	while (it != m_actors.end()) {
		if ((*it)->isAlive()) {
			it++;
			continue;
		}
		delete *it;
		it = m_actors.erase(it);
	}
}

// updates status message
void StudentWorld::updateStatus() {
	ostringstream oss;
	oss.fill('0');
	oss << "Score: " << setw(6) << internal << getScore() << "  ";
	oss.fill(' ');
	oss << "Level: " << setw(2) << getLevel() << "  ";
	oss << "Lives: " << getLives() << "  ";
	oss << "Vaccines: " << m_Penelope->getVaccs() << "  ";
	oss << "Flames: " << m_Penelope->getFlames() << "  ";
	oss << "Mines: " << m_Penelope->getMines() << "  ";
	oss << "Infected: " << m_Penelope->getInfectionCount();
	string s = oss.str();
	setGameStatText(s);
}

// initializes the game
// puts things where they belong based on the level.txt files
int StudentWorld::init()
{
	m_complete = false;

	Level lev(assetPath());
	string levelFile = "level";
	m_level = '0' + getLevel();
	if (getLevel() < 10) {
		levelFile += "0";
	}
	levelFile += m_level;
	levelFile += ".txt";

	Actor* temp;
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int y = 0; y < 16; y++) {
			for (int x = 0; x < 16; x++) {
				Level::MazeEntry ge = lev.getContentsOf(x, y); // level_x=5, level_y=10
				switch (ge)
				{
				case Level::empty:
					break;
				case Level::smart_zombie:
					temp = new SmartZombie(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::dumb_zombie:
					temp = new DumbZombie(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::player:
					m_Penelope = new Penelope(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					temp = m_Penelope;
					m_actors.push_front(temp);
					break;
				case Level::exit:
					temp = new Exit(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::wall:
					temp = new Wall(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::pit:
					break;
				case Level::citizen:
					temp = new Citizen(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::vaccine_goodie:
					temp = new Vaccine(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::gas_can_goodie:
					temp = new GasCan(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				case Level::landmine_goodie:
					temp = new LandmineGoodie(x * LEVEL_WIDTH, y * LEVEL_HEIGHT, this);
					m_actors.push_back(temp);
					break;
				}
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

// moves all the actors
int StudentWorld::move()
{
	// let Penelope doSomething
	if (m_Penelope->isAlive()) {
		m_Penelope->doSomething();
	}
	else {
		// Penelope died
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (m_complete) {
		return GWSTATUS_FINISHED_LEVEL;
	}
	// let all living things do something
	list<Actor*>::iterator it = m_actors.begin();
	it++;		// skip penelope
	while (it != m_actors.end()) {
		if ((*it)->isAlive()) {
			(*it)->doSomething();
		}
		it++;
	}
	// remove and delete all dead objects
	removeDead();
	// update status line
	updateStatus();
	return GWSTATUS_CONTINUE_GAME;
}

// delete all the actors
void StudentWorld::cleanUp() {
	delete m_Penelope;
	m_Penelope = nullptr;
	m_actors.pop_front();
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end()) {
		delete *it;
		*it = nullptr;
		m_actors.pop_front();
		it = m_actors.begin();
	}
}

// destructor
StudentWorld::~StudentWorld() {
	cleanUp();
}