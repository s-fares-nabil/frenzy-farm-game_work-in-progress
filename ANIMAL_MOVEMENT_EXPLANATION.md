# Animal Movement with Pointers - Complete Explanation

## Overview
Animals in the Farm Frenzy game move using a pointer-based system where the Game class stores pointers to animal objects and calls movement functions on them each frame.

---

## 1. STORING ANIMALS WITH POINTERS (Game.h)

```cpp
Chick* chickList[50];        // Array of 50 pointers to Chick objects
int chickCount;              // Counter for how many chickens exist

Cow* cowList[50];            // Array of 50 pointers to Cow objects
int cowCount;                // Counter for how many cows exist

Wolf* enemyWolfList[50];     // Array of 50 pointers to Wolf objects
int enemyWolfCount;          // Counter for how many wolves exist
```

**Why pointers?**
- Each animal is a dynamically created object (created at runtime with `new`)
- Pointers allow us to store references to these objects in arrays
- We can later call functions on these objects through the pointers

---

## 2. ADDING ANIMALS TO THE GAME (Game.cpp)

### When user clicks on Chick Icon:
```cpp
// From BudgetBar.cpp - WaterIcon::onClick()
Chick* newChick = new Chick(pGame, p, 50, 50, image_path);
// ^^ Creates a new Chick object in memory
// ^^ newChick is a POINTER to this new object

chickList[count] = newChick;  // Store the pointer in the array
pGame->addChick(newChick);    // Pass pointer to Game class
count++;
```

### The addChick function (Game.cpp):
```cpp
void Game::addChick(Chick* c)  // c is a POINTER to a Chick object
{
    if (chickCount < 50) {     // Make sure we have space
        chickList[chickCount] = c;  // Store the pointer in array
        chickCount++;           // Increment counter
    }
}
```

**What happens:**
- `new Chick(...)` allocates memory for a new Chick object
- The pointer `c` holds the memory address of this object
- We store this pointer in `chickList[chickCount]`
- Now the Game class "owns" a reference to this animal

---

## 3. MAIN GAME LOOP (Game.cpp - go() function)

```cpp
void Game::go()
{
    do
    {
        // ... other code ...

        // THIS IS THE KEY LINE:
        moveAllAnimals();  // Call movement function every frame

        pWind->UpdateBuffer();
        Sleep(50);  // Wait 50ms (20 FPS)

    } while (!isExit);
}
```

**Flow:**
1. Game loop runs continuously
2. Every 50ms (20 FPS), `moveAllAnimals()` is called
3. This makes all animals move 20 times per second

---

## 4. MOVING ALL ANIMALS (Game.cpp)

```cpp
void Game::moveAllAnimals()
{
    // Move all chickens
    for (int i = 0; i < chickCount; i++)
    {
        if (chickList[i])  // Check if pointer is valid (not null)
        {
            chickList[i]->moveStep();  // Call moveStep() on this chicken
            // chickList[i] is a POINTER to a Chick object
            // -> is the pointer-to-member operator
            // It calls moveStep() function on the object the pointer points to
        }
    }

    // Move all cows
    for (int i = 0; i < cowCount; i++)
    {
        if (cowList[i])
        {
            cowList[i]->moveStep();
        }
    }

    // Move all wolves
    for (int i = 0; i < enemyWolfCount; i++)
    {
        if (enemyWolfList[i])
        {
            enemyWolfList[i]->moveStep();
        }
    }
}
```

**Understanding the pointer operator `->`:**
```cpp
chickList[i]->moveStep();
// This is equivalent to:
(*chickList[i]).moveStep();
// They both mean:
// 1. chickList[i] is a pointer
// 2. Dereference it (*) to get the actual object
// 3. Call moveStep() on that object
```

---

## 5. THE MOVEMENT LOGIC (Animal.cpp - handleAnimalMovement)

This is called from each animal's `moveStep()`:

```cpp
void Chick::moveStep()
{
    handleAnimalMovement(this, pGame, ...);
    //                   ^^^^ 'this' is a pointer to THIS Chick object
}

void handleAnimalMovement(Animal* animal, Game* pGame, ...)
//                        ^^^^^^ This is a pointer to an Animal
{
    // PHASE 1: Check if eating
    if (isEating)
    {
        eatTimer--;
        curr_vel.x = 0;  // Stop moving
        curr_vel.y = 0;
        if (eatTimer <= 0) 
        {
            animal->stopEating();  // Call function on the animal pointer
            // animal-> dereferences the pointer and calls stopEating()
        }
        return;
    }

    // PHASE 2: Decide behavior based on situation
    if (hungerTimer > 340)
    {
        // Just finished eating - keep moving away
    }
    else if (pGame->foodAvailable && hungerTimer <= 0)
    {
        // Food is available and animal is hungry
        if (animal->isNearFood(pGame))
        {
            animal->startEating();
            return;
        }

        // Check if within detection range
        int distToFood = sqrt(...);
        if (distToFood < 300)
        {
            animal->seekFood(point{...});  // Move towards food
        }
        else
        {
            animal->changeDirectionRandomly();  // Wander
        }
    }
    else
    {
        animal->changeDirectionRandomly();  // Normal wandering
    }

    // PHASE 3: Apply movement
    curr_pos.x += curr_vel.x;  // Update X position
    curr_pos.y += curr_vel.y;  // Update Y position

    // PHASE 4: Keep animal in bounds
    if (curr_pos.x <= 0) curr_pos.x = 0;
    if (curr_pos.x >= fieldRight) curr_pos.x = fieldRight;
    // ... and Y bounds ...
}
```

---

## 6. KEY FUNCTIONS THAT CONTROL MOVEMENT

### A) changeDirectionRandomly() - Random Wandering
```cpp
void Animal::changeDirectionRandomly()
{
    if (rand() % 100 < 5)  // 5% chance each frame
    {
        curr_vel.x = (rand() % 4) + 3;  // Random X velocity (-3 to +6)
        if (rand() % 2 == 0) curr_vel.x = -curr_vel.x;  // Maybe make it negative
    }
    if (rand() % 100 < 5)  // 5% chance each frame
    {
        curr_vel.y = (rand() % 4) + 3;  // Random Y velocity
        if (rand() % 2 == 0) curr_vel.y = -curr_vel.y;
    }
}
```
**What it does:** Occasionally changes direction randomly so animals don't move in straight lines

### B) seekFood() - Moving towards Food
```cpp
void Animal::seekFood(point foodPos)
{
    int centerX = curr_pos.x + width / 2;    // Animal's center
    int centerY = curr_pos.y + height / 2;

    double dx = foodPos.x - centerX;  // Distance to food (X)
    double dy = foodPos.y - centerY;  // Distance to food (Y)
    double dist = sqrt(dx*dx + dy*dy);  // Total distance to food

    if (dist > 5)  // If not already at food
    {
        double speed = 7.0;
        curr_vel.x = (int)((dx / dist) * speed);  // Move towards food
        curr_vel.y = (int)((dy / dist) * speed);
    }
}
```
**What it does:** Calculates direction to food and moves towards it

### C) startEating() - When Animal Reaches Food
```cpp
void Animal::startEating()
{
    isEating = true;
    eatTimer = 50;      // Eat for 2.5 seconds
    curr_vel.x = 0;     // Stop moving
    curr_vel.y = 0;
}
```
**What it does:** Stops movement and starts eating timer

### D) stopEating() - After Animal Finishes Eating
```cpp
void Animal::stopEating()
{
    isEating = false;
    eatTimer = 0;
    hungerTimer = 400;  // 20 seconds before can eat again
    moveAwayFromFood();  // Start wandering again
}
```
**What it does:** Allows animal to move again, sets cooldown

---

## 7. DRAWING ANIMALS (Game.cpp - go() function)

```cpp
void Game::go()
{
    do
    {
        moveAllAnimals();  // Update positions

        // Now draw all animals
        for (int i = 0; i < chickCount; i++)
        {
            if (chickList[i]) 
                chickList[i]->draw();  // Draw this chicken at new position
        }

        for (int i = 0; i < cowCount; i++)
        {
            if (cowList[i]) 
                cowList[i]->draw();
        }

        // ... draw wolves ...

        pWind->UpdateBuffer();
        Sleep(50);
    } while (!isExit);
}
```

**The sequence each frame:**
1. `moveAllAnimals()` - Updates position (curr_pos.x, curr_pos.y)
2. `draw()` - Draws animal at the new position
3. `UpdateBuffer()` - Shows the new frame
4. `Sleep(50)` - Wait 50ms before next frame

---

## 8. COMPLETE POINTER FLOW DIAGRAM

```
User clicks Chick icon
    ↓
new Chick(...) creates object in memory
    ↓
chickList[count] = pointer to this object
    ↓
Game::go() runs in a loop
    ↓
moveAllAnimals() is called every frame
    ↓
for (int i = 0; i < chickCount; i++)
    ↓
chickList[i]->moveStep()  (dereference pointer, call function)
    ↓
handleAnimalMovement() updates curr_pos.x and curr_pos.y
    ↓
chickList[i]->draw()  (dereference pointer, draw at new position)
    ↓
UpdateBuffer() shows the new frame
    ↓
Sleep(50) waits 50ms
    ↓
Loop repeats 20 times per second
```

---

## 9. QUICK POINTER REFERENCE

```cpp
Animal* animal;                    // Declare a pointer to Animal
animal = new Animal(...);          // Create object, pointer stores its address
animal->moveStep();                // Call function through pointer (-> operator)
*animal;                           // Dereference pointer (get the object itself)
(*animal).moveStep();              // Same as animal->moveStep()
delete animal;                     // Free memory (don't do this in game loop!)
if (animal != nullptr)             // Check if pointer is valid
chickList[i]->draw();              // Array of pointers
```

---

## Summary

**The key concept:** 
- Pointers store memory addresses of animal objects
- The `->` operator lets us call functions on objects through pointers
- The game loop constantly calls `moveAllAnimals()` which:
  1. Iterates through all animal pointers
  2. Calls `moveStep()` on each animal
  3. Updates their position (curr_pos)
  4. Draws them at the new position
- This happens 20 times per second, creating smooth animation

That's how animals move using pointers! 🎮
