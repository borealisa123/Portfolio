#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Goodie;
class StudentWorld;
class Penelope;

class Actor : public GraphObject {
public:
	Actor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
	virtual void doSomething() = 0;
	bool isAlive();
	virtual void makeDead();					// just sets status m_alive to false
	virtual void beMurdered();					// plays the sound or triggers landmine
	StudentWorld* getWorld();
	virtual bool checkIntersect() = 0;
	virtual bool canBeInfected();				// default - no;	exception  - humans
	virtual bool canBeDestroyed();				// default - no;	exceptions - humans, zomies, goodies, landmines
	virtual bool blocksFlames();				// default - no;	exceptions - walls, exits
	virtual bool isDangerous();					// default - no;	exception  - zombies
	virtual void activate(Actor* o);			// do a specific action
	virtual void useExit();
protected:
	void switchCoords(double& x, double&y, double dist, Direction dir);
private:
	bool m_alive;
	StudentWorld* m_world;
};

class Wall : public Actor {
public:
	Wall(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual bool blocksFlames();
	virtual bool checkIntersect();		// yes
};

class Bounded : public Actor {
public:
	Bounded(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
	virtual bool checkIntersect();		// yes
protected:
	bool moveMyself(double x, double y);
	bool setSmartDir(Actor* o, Actor* t);
	bool isParalyzed();
private:
	virtual bool beSmarter(Actor* o, Direction d);
	bool m_paralyzed;
};


// not virtual :(
class Human : public Bounded {
public:
	Human(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
	virtual bool canBeInfected();				// yes
	virtual bool canBeDestroyed();				// yes
	virtual void activate(Actor* o);			// aka getInfected
	int getInfectionCount();
protected:
	void setInfectionStatus(bool b);
	void clearInfectionCount();					// only used by Penelope
	bool doInfectionStuff();
private:
	virtual void turn() = 0;
	int m_infectionCount;
	bool m_infected;
};

// special person, will be at front of list
class Penelope : public Human {
public:
	Penelope(double x, double y, StudentWorld* world);
	virtual void doSomething();
	void addMine();
	void addGas();
	void addVacc();
	virtual void useExit();
	virtual void beMurdered();
	int getVaccs();
	int getFlames();
	int getMines();
private:
	virtual void turn();
	void useFlame();
	void useMine();
	void useVacc();
	int m_mines;
	int m_gas;
	int m_vaccines;
};

// follow penelope around, be "stupid"
class Citizen : public Human {
public:
	Citizen(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void useExit();
	virtual void beMurdered();
private:
	virtual void turn();
	virtual bool beSmarter(Actor* o, Direction d);
	bool calcRunWhere(double x, double y, double osqd_z);
};

class Zombies : public Bounded {
public:
	Zombies(double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
	virtual bool canBeDestroyed();				// yes
	virtual bool isDangerous();					// yes
	virtual void doSomething();
	virtual void beMurdered();
protected:
	Direction getRandDir();
private:
	int m_movePlan;
	virtual void doSomethingUnique() = 0;
	void vomit(double x, double y, Direction d);
	void getVomitCoords(Actor* z, double& x, double& y);
	virtual void awardPoints() = 0;
};

// vomit at canBeInfected()
class DumbZombie : public Zombies {
public:
	DumbZombie(double x, double y, StudentWorld* world);
	// later
private:
	virtual void doSomethingUnique();
	virtual void awardPoints();
	void dropVacc();
};

// if canBeInfected() is near, go towards it and vomit
class SmartZombie : public Zombies {
public:
SmartZombie(double x, double y, StudentWorld* world);
private:
	virtual void doSomethingUnique();
	virtual void awardPoints();
};

// use circular overlap to check distance
class Unbounded : public Actor {
public:
	Unbounded(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
	virtual bool checkIntersect();		// no
};

// if overlap w/ canBeInfected(), save them (penelope only if all Citizen done) and update score
class Exit : public Unbounded {
public:
	Exit(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual bool blocksFlames();
	virtual void activate(Actor* o);
};

// if overlap w/ Penelope, get picked up and add to her inv
class Goodies : public Unbounded {
public:
	Goodies(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 0);
	virtual bool canBeDestroyed();				// yes
	virtual void activate(Actor* o);
	virtual void doSomething();
private:
	void getPickedUp(Penelope* p);
	virtual void giveSpecificGoodie(Penelope* p) = 0;
};

class Vaccine : public Goodies {
public:
	Vaccine(double x, double y, StudentWorld* world);
private:
	virtual void giveSpecificGoodie(Penelope* p);
};

class GasCan : public Goodies {
public:
	GasCan(double x, double y, StudentWorld* world);
private:
	virtual void giveSpecificGoodie(Penelope* p);
};

class LandmineGoodie : public Goodies {
public:
	LandmineGoodie(double x, double y, StudentWorld* world);
private:
	virtual void giveSpecificGoodie(Penelope* p);
};

// if overlap, infect whatever is there tha canBeInfected()
class Vomit : public Unbounded {
public:
	Vomit(double x, double y, StudentWorld* world, Direction dir);
	virtual void doSomething();
	virtual void activate(Actor* o);
private:
	void expire();
	int m_life;
};

// if overlap, kill whatever is there that canBeDestroyed()
class Flame : public Unbounded {
public:
	Flame(double x, double y, StudentWorld* world, Direction dir);
	virtual void doSomething();
	virtual void activate(Actor* o);
private:
	void expire();
	int m_life;
};

// if overlap, kill whatever is there that canBeDestroyed()
class Pit : public Unbounded {
public:
	Pit(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual void activate(Actor* o);
};

// if overlap, make flames and pit
class Landmine : public Unbounded {
public:
	Landmine(double x, double y, StudentWorld* world);
	virtual void doSomething();
	virtual bool canBeDestroyed();
	virtual void activate(Actor* o);
	virtual void beMurdered();
private:
	int m_safety;
};

#endif // ACTOR_H_
