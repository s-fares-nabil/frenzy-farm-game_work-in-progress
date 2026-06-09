#pragma once
#include <string>
using namespace std;
#include "../Core/Drawable.h"

class Animal :public Drawable
{
protected:
	image* pCachedImage;
	point curr_vel;           // Current velocity (direction and speed)
	int moveTimer;            // Timer for random direction changes
	int eatTimer;             // Timer for eating state (5 seconds)
	int hungerTimer;          // Timer to prevent immediate re-eating
	bool isEating;            // Flag: is the animal currently eating?
	int moveDirection;        // 0=up, 1=down, 2=left, 3=right
	int productionTimer;      // Timer for production (10 seconds = 200 frames at 20 FPS)
	bool isInProductionMode;  // Is the animal waiting to produce?

	int moveCounter;

	// Draw the unique animal number above this animal
	void drawAnimalCounter(int id) const;
public:
	point curr_pos;           // Current position of the animal

	Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	~Animal() { delete pCachedImage; }
	virtual void draw() const override;
	virtual void moveStep() = 0;   // The action that should be taken each time step
	virtual void produce() = 0;

	// Helper methods for animal behavior
	void changeDirectionRandomly();
	void moveAwayFromFood();
	bool isNearFood(Game* game);
	void startEating();
	virtual void stopEating();
	void seekFood(point foodPos);
	point getNearestFoodArea(Game* game);  // Find nearest available food
	bool getIsEating() const { return isEating; }

};

class Chick : public Animal
{
public:
	static int chickCounter;
	int myID;
	static void resetCounter() { chickCounter = 0; }	// Reset static counter so new game starts IDs from 1
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
	virtual void produce() override;
	virtual void draw() const override;
	virtual void stopEating();  // Override to start production timer
};

class Cow : public Animal
{
public:
	static int cowCounter;
	int myID;
	static void resetCounter() { cowCounter = 0; }		// Reset static counter so new game starts IDs from 1
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
	virtual void draw() const override;
	virtual void produce() override;
};
class Wolf : public Animal
{
public:
	static int wolfCounter;
	int myID;
	int hitCount = 0;			// Tracks how many times player clicked this wolf (5 clicks = defeated)
	static void resetCounter() { wolfCounter = 0; }	// Reset static counter so new game starts IDs from 1
	Wolf(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep() override;
	virtual void draw() const override;
	virtual void produce() override;
	bool isClicked(int x, int y) const;	// AABB hit-test: returns true if (x,y) is within this wolf's bounding box
};