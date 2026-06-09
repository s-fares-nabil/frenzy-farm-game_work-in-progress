#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../CMUgraphicsLib/image.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../Entities/Animal.h"
#include "../Entities/Grass.h"
#include "../Entities/Product.h"
class Product;

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	int timer = 120;
	int goal = 300;
	int level = 1;
	long long lastTick;
	Product* productList[200];
	int productCount;

	Wolf* enemyWolfList[50];
	int enemyWolfCount;

	Chick* chickList[50];
	int chickCount;

	Cow* cowList[50];
	int cowCount;

	Grass* grassList[200];
	int grassCount;

	// Cached images for food boxes to avoid re-decoding JPEG every frame
	image* pCachedFoodImage;
	image* pCachedEmptyBoxImage;

public:
	bool isInsideWarehouse(int x, int y) const;
	void openWarehouseWindow();
	int budget = 2000;
	int animalCount = 0;
	bool paused = false;				// Flag: when true, game logic (timer, movement, spawning) is paused
	Game();
	~Game();

	clicktype getMouseClick(int& x, int& y) const;
	string getSrting() const;

	window* CreateWind(int, int, int, int) const;
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void clearStatusBar() const;
	void drawWarehouse() const;

	void printMessage(string msg) const;
	void printStatusBar() const;
	void handleTimer();
	void setTimer(int t);
	int getLevel() const { return level; }		// Getter so Wolf constructor can read level for speed scaling
	void setGoal(int g);
	void setLevel(int l);
	void setAnimalCount(int n);

	// ensure goal is always higher than current budget and depends on level
	void updateGoal();
	void drawField() const;

	int getTimerByLevel(int level);

	void go();

	window* getWind() const;
	void addProduct(Product* p);
	void addEnemyWolf(Wolf* w);
	void addChick(Chick* c);
	void addCow(Cow* c);
	void addGrass(Grass* g);
	void generateRandomWolf();
	void drawFoodBox() const;
	void moveAllAnimals();
	void drawAllAnimals() const;

	bool foodAvailable1 = false;
	bool foodAvailable2 = false;
	bool foodAvailable3 = false;

	int foodX1 = 20;
	int foodY1;
	int foodCounter1 = 0;

	int foodX2 = 0;
	int foodY2;
	int foodCounter2 = 0;

	int foodX3 = 0;
	int foodY3;
	int foodCounter3 = 0;

	int foodWidth = 100;
	int foodHeight = 100;

	void decreaseFoodArea(int areaIndex = 1);

	int warehouseEggCount = 0;
	int warehouseMilkCount = 0;
	int warehouseWoolCount = 0;

	void addToWarehouse(ProductType type);
	void handleProductClick(int x, int y);
	void handleWolfClick(int x, int y);		// Check if a wolf was clicked; increment hitCount, delete at 5 hits
	void removeProduct(int index);

	void saveGame() const;		// Serialize game state to "savegame.txt"
	void loadGame();			// Restore game state from "savegame.txt"
	void resetGame();			// Delete all objects, reset to fresh level 1
	void checkWolfCollisions();
	void checkLevelUp();
};