 #include "Animal.h"
#include "../Entities/Product.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Per-type static counters
int Chick::chickCounter = 0;
int Cow::cowCounter = 0;
int Wolf::wolfCounter = 0;

static bool randomSeeded = false;
void seedRandom() {
    if (!randomSeeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        randomSeeded = true;
    }
}

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
    : Drawable(r_pGame, r_point, r_width, r_height)
{
    pCachedImage = new image(img_path, JPEG);
    curr_pos = r_point;
    RefPoint = r_point;

    productionTimer = 0;
    isInProductionMode = false;

    seedRandom();
    curr_vel.x = (rand() % 3) + 3;
    curr_vel.y = (rand() % 3) + 3;
    if (rand() % 2 == 0) curr_vel.x = -curr_vel.x;
    if (rand() % 2 == 0) curr_vel.y = -curr_vel.y;

    moveTimer = 0;
    eatTimer = 0;
    hungerTimer = 0;
    isEating = false;
}

// Draw the unique animal number above the animal image
void Animal::drawAnimalCounter(int id) const {
    window* pWind = pGame->getWind();

    // Put the counter centered above the animal
    pWind->SetPen(BLACK, 2);
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->DrawInteger(RefPoint.x + (width / 2) - 5, RefPoint.y - 22, id);
}

// Parent draw: image only (no counter - subclasses handle it)
void Animal::draw() const {
    window* pWind = pGame->getWind();

    // Draw the animal image
    if (pCachedImage)
        pWind->DrawImage(pCachedImage, RefPoint.x, RefPoint.y, width, height);
}

// Chick draw: image + per-type counter + production timer if needed
void Chick::draw() const {
    window* pWind = pGame->getWind();

    // Draw image
    Animal::draw();

    // Draw per-type counter
    drawAnimalCounter(myID);

    // Draw the production timer only if in production mode (after eating)
    if (isInProductionMode && productionTimer > 0)
    {
        int secondsRemaining = (productionTimer + 19) / 20;
        pWind->SetPen(RED, 3);
        pWind->SetFont(20, BOLD, BY_NAME, "Arial");
        pWind->DrawInteger(RefPoint.x + (width / 2) - 5, RefPoint.y - 45, secondsRemaining);
    }
}

void Cow::draw() const {
    window* pWind = pGame->getWind();
    Animal::draw();
    drawAnimalCounter(myID);
}

void Wolf::draw() const {
    window* pWind = pGame->getWind();
    Animal::draw();
    drawAnimalCounter(myID);
}

void Animal::changeDirectionRandomly()
{
    {
        curr_vel.x = (rand() % 4) + 3;
        if (rand() % 2 == 0) curr_vel.x = -curr_vel.x;
    }
    if (rand() % 100 < 5)
    {
        curr_vel.y = (rand() % 4) + 3;
        if (rand() % 2 == 0) curr_vel.y = -curr_vel.y;
    }
    
    // Safety: never zero
    if (curr_vel.x == 0) curr_vel.x = (rand() % 2 == 0) ? 4 : -4;
    if (curr_vel.y == 0) curr_vel.y = (rand() % 2 == 0) ? 4 : -4;
}

static bool overlaps(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

static bool touches(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
    int gapX = (ax + aw <= bx) ? bx - (ax + aw) :
               (ax >= bx + bw) ? ax - (bx + bw) : 0;
    int gapY = (ay + ah <= by) ? by - (ay + ah) :
               (ay >= by + bh) ? ay - (by + bh) : 0;
    return gapX <= 5 && gapY <= 5;
}

bool Animal::isNearFood(Game* game)
{
    int ax = curr_pos.x, ay = curr_pos.y, aw = width, ah = height;

    if (game->foodAvailable1 && touches(ax, ay, aw, ah, game->foodX1, game->foodY1, game->foodWidth, game->foodHeight))
        return true;
    if (game->foodAvailable2 && touches(ax, ay, aw, ah, game->foodX2, game->foodY2, game->foodWidth, game->foodHeight))
        return true;
    if (game->foodAvailable3 && touches(ax, ay, aw, ah, game->foodX3, game->foodY3, game->foodWidth, game->foodHeight))
        return true;

    return false;
}

void Animal::moveAwayFromFood()
{
    // Just start moving randomly instead of forcefully bouncing away
    changeDirectionRandomly();
}

void Animal::startEating()
{
    isEating = true;
    eatTimer = 50; // 2.5 seconds (20 FPS * 2.5)
    curr_vel.x = 0;
    curr_vel.y = 0;

    cout << "\n===== CHICK EATING STARTED =====" << endl;
    cout << "[DEBUG] Chick STARTED eating!" << endl;
    cout << "[DEBUG] Eat Timer: " << eatTimer << " frames (2.5 seconds)" << endl;
    cout << "[DEBUG] Current Position: (" << curr_pos.x << ", " << curr_pos.y << ")" << endl;
    cout << "================================\n" << endl;
}

void Animal::stopEating()
{
    cout << "\n[DEBUG] Animal::stopEating() called!" << endl;

    isEating = false;
    eatTimer = 0;
    hungerTimer = 400; // 20 seconds cooldown

    // Find which food area the animal is at and decrease that one
    int ax = curr_pos.x, ay = curr_pos.y, aw = width, ah = height;

    auto atFoodArea = [&](int fx, int fy) {
        return touches(ax, ay, aw, ah, fx, fy, pGame->foodWidth, pGame->foodHeight);
    };

    if (pGame->foodAvailable1 && atFoodArea(pGame->foodX1, pGame->foodY1))
    {
        pGame->decreaseFoodArea(1);
        moveAwayFromFood();
        return;
    }
    if (pGame->foodAvailable2 && atFoodArea(pGame->foodX2, pGame->foodY2))
    {
        pGame->decreaseFoodArea(2);
        moveAwayFromFood();
        return;
    }
    if (pGame->foodAvailable3 && atFoodArea(pGame->foodX3, pGame->foodY3))
    {
        pGame->decreaseFoodArea(3);
        moveAwayFromFood();
        return;
    }

    moveAwayFromFood();
}

void Animal::seekFood(point foodPos)
{
    int centerX = curr_pos.x + width / 2;
    int centerY = curr_pos.y + height / 2;

    double dx = foodPos.x - centerX;
    double dy = foodPos.y - centerY;
    double dist = sqrt(dx*dx + dy*dy);

    if (dist > 5)
    {
        double speed = 7.0;
        curr_vel.x = (int)((dx / dist) * speed);
        curr_vel.y = (int)((dy / dist) * speed);
    }
}

point Animal::getNearestFoodArea(Game* game)
{
    int animalCenterX = curr_pos.x + width / 2;
    int animalCenterY = curr_pos.y + height / 2;
    point nearest;
    nearest.x = animalCenterX;
    nearest.y = animalCenterY;
    double minDist = 999999;

    // Check Food Area 1
    if (game->foodAvailable1)
    {
        int foodCenterX = game->foodX1 + 50;
        int foodCenterY = game->foodY1 + 50;
        double dist = sqrt(pow(animalCenterX - foodCenterX, 2) + pow(animalCenterY - foodCenterY, 2));
        if (dist < minDist)
        {
            minDist = dist;
            nearest.x = foodCenterX;
            nearest.y = foodCenterY;
        }
    }

    // Check Food Area 2
    if (game->foodAvailable2)
    {
        int foodCenterX = game->foodX2 + 50;
        int foodCenterY = game->foodY2 + 50;
        double dist = sqrt(pow(animalCenterX - foodCenterX, 2) + pow(animalCenterY - foodCenterY, 2));
        if (dist < minDist)
        {
            minDist = dist;
            nearest.x = foodCenterX;
            nearest.y = foodCenterY;
        }
    }

    // Check Food Area 3
    if (game->foodAvailable3)
    {
        int foodCenterX = game->foodX3 + 50;
        int foodCenterY = game->foodY3 + 50;
        double dist = sqrt(pow(animalCenterX - foodCenterX, 2) + pow(animalCenterY - foodCenterY, 2));
        if (dist < minDist)
        {
            minDist = dist;
            nearest.x = foodCenterX;
            nearest.y = foodCenterY;
        }
    }

    return nearest;
}

void handleAnimalMovement(Animal* animal, Game* pGame, int& hungerTimer, bool& isEating, int& eatTimer, point& curr_pos, point& curr_vel, int width, int height, point& RefPoint, bool canEatFromFood = true)
{
    // 0. Cooldown logic
    if (hungerTimer > 0) hungerTimer--;

    // 1. EATING STATE - ABSOLUTE PRIORITY
    if (isEating)
    {
        eatTimer--;
        curr_vel.x = 0; // Stay still
        curr_vel.y = 0;
        if (eatTimer <= 0) 
        {
            animal->stopEating();
            // Allow them to start moving away in the same frame
        }
        else
        {
            return; // Don't process movement if still eating
        }
    }

    // 2. MOVEMENT PHASE

    // Forced Departure (3 seconds after eating)
    if (hungerTimer > 340)
    {
        // Direction already set by stopEating()
    }
    // Seek Food (If hungry and food exists and animal is NEAR the food area)
    else if (canEatFromFood && (pGame->foodAvailable1 || pGame->foodAvailable2 || pGame->foodAvailable3) && hungerTimer <= 0)
    {
        if (animal->isNearFood(pGame))
        {
            animal->startEating();
            return; 
        }

        // Get nearest food area and seek it
        point nearestFood = animal->getNearestFoodArea(pGame);
        int animalCenterX = curr_pos.x + width / 2;
        int animalCenterY = curr_pos.y + height / 2;

        int distToFood = sqrt(pow(animalCenterX - nearestFood.x, 2) + pow(animalCenterY - nearestFood.y, 2));

        if (distToFood < 300)  // Only animals within 300 pixels will seek food
        {
            animal->seekFood(nearestFood);
        }
        else
        {
            // Animals far from food just wander normally
            animal->changeDirectionRandomly();
        }
    }
    // Normal Wander
    else
    {
        animal->changeDirectionRandomly();
    }

    // Apply Velocity
    curr_pos.x += curr_vel.x;
    curr_pos.y += curr_vel.y;

    // Bound Checks
    int fieldLeft = 0;
    int fieldRight = config.windWidth - width;
    int fieldTop = 2 * config.toolBarHeight;
    int fieldBottom = config.windHeight - config.statusBarHeight - height;

    if (curr_pos.x <= fieldLeft) { curr_pos.x = fieldLeft; curr_vel.x = abs(curr_vel.x); }
    if (curr_pos.x >= fieldRight) { curr_pos.x = fieldRight; curr_vel.x = -abs(curr_vel.x); }
    if (curr_pos.y <= fieldTop) { curr_pos.y = fieldTop; curr_vel.y = abs(curr_vel.y); }
    if (curr_pos.y >= fieldBottom) { curr_pos.y = fieldBottom; curr_vel.y = -abs(curr_vel.y); }

    // Obstacle collision: warehouse + food boxes
    {
        int ax = curr_pos.x, ay = curr_pos.y, aw = width, ah = height;
        auto resolveObstacle = [&](int ox, int oy, int ow, int oh) {
            if (ax < ox + ow && ax + aw > ox && ay < oy + oh && ay + ah > oy) {
                int overlapLeft = (ax + aw) - ox;
                int overlapRight = (ox + ow) - ax;
                int overlapTop = (ay + ah) - oy;
                int overlapBottom = (oy + oh) - ay;

                int minOverlap = overlapLeft;
                int dir = 0;
                if (overlapRight < minOverlap) { minOverlap = overlapRight; dir = 1; }
                if (overlapTop < minOverlap) { minOverlap = overlapTop; dir = 2; }
                if (overlapBottom < minOverlap) { minOverlap = overlapBottom; dir = 3; }

                if (dir == 0) { curr_pos.x = ox - aw; curr_vel.x = -abs(curr_vel.x); }
                else if (dir == 1) { curr_pos.x = ox + ow; curr_vel.x = abs(curr_vel.x); }
                else if (dir == 2) { curr_pos.y = oy - ah; curr_vel.y = -abs(curr_vel.y); }
                else if (dir == 3) { curr_pos.y = oy + oh; curr_vel.y = abs(curr_vel.y); }
            }
        };

        int wareX = config.windWidth - 120 - 20;
        int wareY = config.windHeight - config.statusBarHeight - 120 - 20;
        resolveObstacle(wareX, wareY, 120, 120);

        // Food box collision (always bounce off, no condition)
        resolveObstacle(pGame->foodX1, pGame->foodY1, pGame->foodWidth, pGame->foodHeight);
        resolveObstacle(pGame->foodX2, pGame->foodY2, pGame->foodWidth, pGame->foodHeight);
        resolveObstacle(pGame->foodX3, pGame->foodY3, pGame->foodWidth, pGame->foodHeight);
    }

    // After obstacle collision: if pushed to food edge while hungry, start eating
    if (canEatFromFood && hungerTimer <= 0 && !isEating && animal->isNearFood(pGame))
    {
        animal->startEating();
        RefPoint = curr_pos;
        return;
    }

    // Stuck Prevention
    if (curr_vel.x == 0 && curr_vel.y == 0 && !isEating)
    {
        curr_vel.x = 5;
        curr_vel.y = -5;
    }

    RefPoint = curr_pos;
}

void Chick::moveStep()
{
    // Always handle movement and eating first so the chick can move while the
    // production timer counts down.
    handleAnimalMovement(this, pGame, hungerTimer, isEating, eatTimer, curr_pos, curr_vel, width, height, RefPoint);

    // If in production mode, count down the timer and produce when it reaches 0
    if (isInProductionMode)
    {
        productionTimer--;

        int secondsLeft = (productionTimer + 19) / 20;

        // Only print every 20 frames (once per second) to avoid spam
        if (productionTimer % 20 == 0)
        {
            cout << "[DEBUG] Production Timer: " << secondsLeft << " seconds remaining (frames: " << productionTimer << ")" << endl;
        }

        if (productionTimer <= 0)
        {
            cout << "\n===== EGG PRODUCTION COMPLETE =====" << endl;
            cout << "[DEBUG] Chick producing EGG NOW!" << endl;
            this->produce();      // Produce the egg
            isInProductionMode = false;
            productionTimer = 0;
            cout << "==================================\n" << endl;
        }
    }
}

void Chick::produce()
{
    // Create egg at current position
    cout << "[DEBUG] Creating egg at position: (" << RefPoint.x << ", " << RefPoint.y << ")" << endl;
    Product* pEgg = new Product(pGame, RefPoint, 30, 30, "images\\egg.jpg", PRODUCT_EGG);
    pGame->addProduct(pEgg);
    cout << "[DEBUG] Egg added to product list!" << endl;
}

void Chick::stopEating()
{
    // Call parent stopEating first
    Animal::stopEating();

    // Then start the 10-second production timer
    productionTimer = 200;  // 10 seconds at 20 FPS
    isInProductionMode = true;

    cout << "\n====== CHICK PRODUCTION MODE ACTIVATED ======" << endl;
    cout << "[DEBUG] Chick finished eating! Starting 10-second production timer." << endl;
    cout << "[DEBUG] Position: (" << RefPoint.x << ", " << RefPoint.y << ")" << endl;
    cout << "[DEBUG] Production Timer: " << productionTimer << " frames" << endl;
    cout << "[DEBUG] Is In Production Mode: " << (isInProductionMode ? "TRUE" : "FALSE") << endl;
    cout << "==========================================\n" << endl;
}

void Cow::produce()
{
    // Cow produces milk
    Product* pMilk = new Product(pGame, RefPoint, 30, 30, "images\\milk.jpg", PRODUCT_MILK);
    pGame->addProduct(pMilk);
}

void Wolf::produce()
{
    // Wolf doesn't produce anything
}

void Cow::moveStep()
{
    handleAnimalMovement(this, pGame, hungerTimer, isEating, eatTimer, curr_pos, curr_vel, width, height, RefPoint, false);
}

void Wolf::moveStep()
{
    // Randomly change direction occasionally
    moveTimer--;
    if (moveTimer <= 0) {
        changeDirectionRandomly();
        moveTimer = 20 + rand() % 30; // Reset timer
    }

    curr_pos.x += curr_vel.x;
    curr_pos.y += curr_vel.y;

    // Field boundaries
    int fieldLeft = 0;
    int fieldRight = config.windWidth - width;
    int fieldTop = 2 * config.toolBarHeight;
    int fieldBottom = config.windHeight - config.statusBarHeight - height;

    // Bounce off walls
    if (curr_pos.x <= fieldLeft) { curr_pos.x = fieldLeft; curr_vel.x = abs(curr_vel.x); }
    if (curr_pos.x >= fieldRight) { curr_pos.x = fieldRight; curr_vel.x = -abs(curr_vel.x); }
    if (curr_pos.y <= fieldTop) { curr_pos.y = fieldTop; curr_vel.y = abs(curr_vel.y); }
    if (curr_pos.y >= fieldBottom) { curr_pos.y = fieldBottom; curr_vel.y = -abs(curr_vel.y); }

    // Obstacle collision: warehouse + food boxes
    {
        int ax = curr_pos.x, ay = curr_pos.y, aw = width, ah = height;
        auto resolveObstacle = [&](int ox, int oy, int ow, int oh) {
            if (ax < ox + ow && ax + aw > ox && ay < oy + oh && ay + ah > oy) {
                int overlapLeft = (ax + aw) - ox;
                int overlapRight = (ox + ow) - ax;
                int overlapTop = (ay + ah) - oy;
                int overlapBottom = (oy + oh) - ay;

                int minOverlap = overlapLeft;
                int dir = 0;
                if (overlapRight < minOverlap) { minOverlap = overlapRight; dir = 1; }
                if (overlapTop < minOverlap) { minOverlap = overlapTop; dir = 2; }
                if (overlapBottom < minOverlap) { minOverlap = overlapBottom; dir = 3; }

                if (dir == 0) { curr_pos.x = ox - aw; curr_vel.x = -abs(curr_vel.x); }
                else if (dir == 1) { curr_pos.x = ox + ow; curr_vel.x = abs(curr_vel.x); }
                else if (dir == 2) { curr_pos.y = oy - ah; curr_vel.y = -abs(curr_vel.y); }
                else if (dir == 3) { curr_pos.y = oy + oh; curr_vel.y = abs(curr_vel.y); }
            }
        };

        int wareX = config.windWidth - 120 - 20;
        int wareY = config.windHeight - config.statusBarHeight - 120 - 20;
        resolveObstacle(wareX, wareY, 120, 120);

        // Food box collision
        {
            resolveObstacle(pGame->foodX1, pGame->foodY1, pGame->foodWidth, pGame->foodHeight);
            resolveObstacle(pGame->foodX2, pGame->foodY2, pGame->foodWidth, pGame->foodHeight);
            resolveObstacle(pGame->foodX3, pGame->foodY3, pGame->foodWidth, pGame->foodHeight);
        }
    }

    RefPoint = curr_pos;
}

Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
    : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
    chickCounter++;
    myID = chickCounter;
}

Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
    : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
    cowCounter++;
    myID = cowCounter;
}

Wolf::Wolf(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
    : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
    wolfCounter++;
    myID = wolfCounter;
    hitCount = 0;

    // Scale velocity with current game level: higher level = faster wolves
    // Formula: base speed = lvl * 2, plus random bonus 0..lvl
    // Level 1: 2-3  |  Level 2: 4-6  |  Level 3: 6-9  |  Level 5: 10-15
    int lvl = pGame->getLevel();
    if (lvl < 1) lvl = 1;
    int baseSpeed = lvl * 2;
    curr_vel.x = (rand() % (lvl + 1)) + baseSpeed;
    curr_vel.y = (rand() % (lvl + 1)) + baseSpeed;
    // Randomize direction (positive or negative on each axis)
    if (rand() % 2 == 0) curr_vel.x = -curr_vel.x;
    if (rand() % 2 == 0) curr_vel.y = -curr_vel.y;
}

// AABB (Axis-Aligned Bounding Box) point-in-rectangle hit test.
// Returns true if mouse click (x, y) is inside this wolf's bounding box.
bool Wolf::isClicked(int x, int y) const
{
    return x >= RefPoint.x && x <= RefPoint.x + width &&
           y >= RefPoint.y && y <= RefPoint.y + height;
}