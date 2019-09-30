#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	Penelope* getPenelope() const;
	void doOnOverlap(Actor* o);
	bool doesOverlap(Actor* o) const;
	bool doesIntersect(double x, double y, Actor* o) const;
	bool flameBlocked(Actor* o) const;
	bool shouldVomit(double x, double y) const;
	bool shouldPursue(Actor* o, Actor*& target) const;
	void followOrRun(double x, double y, double& sqdist_z, double& sqdist_p) const;
	bool noHumans() const;
	void removeDead();
	void addActor(Actor* a);
	void levelComplete();
	void updateStatus();
private:
	Penelope* m_Penelope;
	list<Actor*> m_actors;
	int m_level;
	bool m_complete;
};

#endif // STUDENTWORLD_H_
