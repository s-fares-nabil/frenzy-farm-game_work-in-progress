#include "Game.h"
#include "../Config/GameConfig.h"
#include "../Entities/Product.h"
#include "../Entities/Animal.h"
#include <random>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

Game::Game()
{
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	createToolbar();
	createBudgetbar();

	drawField();

	int fieldStartY = 2 * config.toolBarHeight;
	int spacing = 130;

	foodX1 = 20;
	foodX2 = 20;
	foodX3 = 20;

	foodY1 = fieldStartY + 100;
	foodY2 = fieldStartY + 100 + spacing;
	foodY3 = fieldStartY + 100 + (2 * spacing);

	foodCounter1 = 0;
	foodCounter2 = 0;
	foodCounter3 = 0;
	foodAvailable1 = false;
	foodAvailable2 = false;
	foodAvailable3 = false;

	// Load and cache food box images once during initialization
	try {
		pCachedFoodImage = new image("images\\food.jpg", JPEG);
		pCachedEmptyBoxImage = new image("images\\emptybox.jpg", JPEG);
	}
	catch (...) {
		pCachedFoodImage = nullptr;
		pCachedEmptyBoxImage = nullptr;
	}

	drawFoodBox();

	clearStatusBar();

	level = 1;
	timer = getTimerByLevel(level);
	lastTick = time(0);
	updateGoal();
	productCount = 0;
	enemyWolfCount = 0;
	chickCount = 0;
	cowCount = 0;
	grassCount = 0;

	for (int i = 0; i < 200; i++) productList[i] = nullptr;
	for (int i = 0; i < 50; i++) enemyWolfList[i] = nullptr;
	for (int i = 0; i < 50; i++) chickList[i] = nullptr;
	for (int i = 0; i < 50; i++) cowList[i] = nullptr;
	for (int i = 0; i < 200; i++) grassList[i] = nullptr;
}

Game::~Game()
{
	for (int i = 0; i < productCount; i++)
		if (productList[i]) delete productList[i];

	for (int i = 0; i < enemyWolfCount; i++)
		if (enemyWolfList[i]) delete enemyWolfList[i];

	for (int i = 0; i < chickCount; i++)
		if (chickList[i]) delete chickList[i];

	for (int i = 0; i < cowCount; i++)
		if (cowList[i]) delete cowList[i];

	for (int i = 0; i < grassCount; i++)
		if (grassList[i]) delete grassList[i];

	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->WaitMouseClick(x, y);
}

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE) return "";
		if (Key == 13) return Label;
		if (Key == 8)
		{
			if (Label.size() > 0) Label.resize(Label.size() - 1);
			else Key = '\0';
		}
		else Label += Key;
		printMessage(Label);
	}
}

window* Game::CreateWind(int w, int h, int x, int y) const
{
	window* pW = new window(w, h, x, y);
	pW->SetBrush(config.bkGrndColor);
	pW->SetPen(config.bkGrndColor, 1);
	pW->DrawRectangle(0, 0, w, h);
	return pW;
}

void Game::createToolbar()
{
	point toolbarUpperleft;
	toolbarUpperleft.x = 0;
	toolbarUpperleft.y = 0;
	gameToolbar = new Toolbar(this, toolbarUpperleft, 0, config.toolBarHeight);
	gameToolbar->draw();
}

void Game::createBudgetbar()
{
	point budgetbarUpperleft;
	budgetbarUpperleft.x = 0;
	budgetbarUpperleft.y = config.toolBarHeight;
	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, 0, config.toolBarHeight);
	gameBudgetbar->draw();
}

void Game::clearBudget() const
{
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();
	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 200, config.toolBarHeight + 10, msg);
}

void Game::clearStatusBar() const
{
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::printMessage(string msg) const
{
	clearStatusBar();
	pWind->SetPen(WHITE, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);
}

window* Game::getWind() const
{
	return pWind;
}

void Game::setTimer(int t) { timer = t; }
void Game::setGoal(int g) { goal = g; }
void Game::setLevel(int l)
{
	level = l;
	timer = getTimerByLevel(level);
	updateGoal();
}
void Game::setAnimalCount(int n) { animalCount = n; }

int Game::getTimerByLevel(int level)
{
	if (level == 1) return 90;
	if (level == 2) return 60;
	if (level == 3) return 30;
	return 90;
}

void Game::printStatusBar() const
{
	clearStatusBar();
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");

	int mins = timer / 60;
	int secs = timer % 60;
	string timerStr = "Timer: ";
	if (mins < 10) timerStr += "0";
	timerStr += to_string(mins) + ":";
	if (secs < 10) timerStr += "0";
	timerStr += to_string(secs);

	int barY = config.windHeight - (int)(0.85 * config.statusBarHeight);

	pWind->DrawString(10, barY, timerStr);
	pWind->DrawString(250, barY, "Goal: $" + to_string(goal));
	pWind->DrawString(500, barY, "Level: " + to_string(level));
	pWind->DrawString(750, barY, "Animals: " + to_string(animalCount));
}

void Game::drawWarehouse() const
{
	int wareWidth = 120;
	int wareHeight = 120;
	int wareX = config.windWidth - wareWidth - 20;
	int wareY = config.windHeight - config.statusBarHeight - wareHeight - 20;

	pWind->DrawImage("images\\warehouse.jpg", wareX, wareY, wareWidth, wareHeight);

	pWind->SetPen(BLACK);
	pWind->SetFont(16, BOLD, BY_NAME, "Arial");
	pWind->DrawString(wareX + 15, wareY - 20, "Warehouse");

	pWind->SetFont(12, BOLD, BY_NAME, "Arial");
	pWind->DrawString(wareX - 60, wareY, "Eggs: " + to_string(warehouseEggCount));
	pWind->DrawString(wareX - 60, wareY + 20, "Milk: " + to_string(warehouseMilkCount));
	pWind->DrawString(wareX - 60, wareY + 40, "Wool: " + to_string(warehouseWoolCount));
}

void Game::handleTimer()
{
	generateRandomWolf();
	long long currentTick = time(0);
	if (currentTick - lastTick >= 1)
	{
		if (timer > 0) timer--;
		lastTick = currentTick;
	}
}

void Game::drawField() const
{
	int fieldStartY = 2 * config.toolBarHeight;
	int fieldEndY = config.windHeight - config.statusBarHeight;
	int fieldWidth = config.windWidth;

	color grassGreen = color(34, 139, 34);
	pWind->SetBrush(grassGreen);
	pWind->SetPen(grassGreen, 1);
	pWind->DrawRectangle(0, fieldStartY, fieldWidth, fieldEndY, FILLED);

	color lightGrass = color(50, 155, 50);
	pWind->SetBrush(lightGrass);
	pWind->SetPen(lightGrass, 1);
	for (int y = fieldStartY + 40; y < fieldEndY; y += 80)
		pWind->DrawRectangle(0, y, fieldWidth, y + 30, FILLED);

	color fenceBrown = color(139, 69, 19);
	pWind->SetPen(fenceBrown, 8);
	pWind->DrawLine(0, fieldStartY, fieldWidth, fieldStartY);
	pWind->DrawLine(0, fieldEndY, fieldWidth, fieldEndY);
	pWind->DrawLine(0, fieldStartY, 0, fieldEndY);
	pWind->DrawLine(fieldWidth, fieldStartY, fieldWidth, fieldEndY);

	color darkBrown = color(101, 51, 10);
	pWind->SetBrush(darkBrown);
	pWind->SetPen(darkBrown, 2);
	int postWidth = 12;
	for (int x = 0; x < fieldWidth; x += 100)
	{
		pWind->DrawRectangle(x - postWidth / 2, fieldStartY - 5, x + postWidth / 2, fieldStartY + 20, FILLED);
		pWind->DrawRectangle(x - postWidth / 2, fieldEndY - 20, x + postWidth / 2, fieldEndY + 5, FILLED);
	}

	color cornerBrown = color(90, 45, 10);
	pWind->SetBrush(cornerBrown);
	pWind->SetPen(cornerBrown, 1);
	pWind->DrawRectangle(0, fieldStartY - 5, 20, fieldStartY + 25, FILLED);
	pWind->DrawRectangle(fieldWidth - 20, fieldStartY - 5, fieldWidth, fieldStartY + 25, FILLED);
	pWind->DrawRectangle(0, fieldEndY - 25, 20, fieldEndY + 5, FILLED);
	pWind->DrawRectangle(fieldWidth - 20, fieldEndY - 25, fieldWidth, fieldEndY + 5, FILLED);
}

void Game::go()
{
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy - - - - - - - - - -");
	pWind->SetBuffering(true);

	do
	{
		// Only run game logic (timer, movement, wolf spawning, level-up) when NOT paused
		if (!paused)
		{
			handleTimer();
			moveAllAnimals();
			checkWolfCollisions();
			checkLevelUp();
		}

		if (pWind->GetMouseClick(x, y))
		{
			// Debug: show click coordinates
			char buf[128];
			sprintf(buf, "Mouse click at (%d,%d)", x, y);
			printMessage(string(buf));

			if (y >= 0 && y < config.toolBarHeight)
				isExit = gameToolbar->handleClick(x, y);
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
				isExit = gameBudgetbar->handleClick(x, y);
			else {
				// --- NEW: Check if the Warehouse was clicked ---
				if (isInsideWarehouse(x, y))
				{
					openWarehouseWindow();
				}

				handleProductClick(x, y);
				handleWolfClick(x, y);		// Check if a wolf was clicked; increment hitCount, delete at 5 hits
			}
		}

	
		if (isExit) break;

		drawField();
		drawWarehouse();

		for (int i = 0; i < grassCount; i++)
		{
			if (grassList[i]) grassList[i]->draw();
		}

		drawFoodBox();
		drawAllAnimals();

		for (int i = 0; i < productCount; i++)
		{
			if (productList[i]) productList[i]->draw();
		}

		gameToolbar->draw();
		gameBudgetbar->draw();

		printStatusBar();
		printBudget("BUDGET = $" + to_string(budget));

		// Draw red "PAUSED" overlay text when game is paused
		if (paused)
		{
			pWind->SetPen(RED, 5);
			pWind->SetFont(60, BOLD, BY_NAME, "Arial");
			pWind->DrawString(config.windWidth / 2 - 130, config.windHeight / 2 - 50, "PAUSED");
		}

		if (timer <= 0)
		{
			pWind->SetPen(BLUE, 5);
			pWind->SetFont(50, BOLD, BY_NAME, "Arial");
			pWind->DrawString(config.windWidth / 2 - 150, config.windHeight / 2, "Game Over");
			pWind->UpdateBuffer();
			break;
		}

		pWind->UpdateBuffer();
		Sleep(50);

	} while (!isExit);
}

void Game::addProduct(Product* p)
{
	if (productCount < 200 && p != nullptr)
	{
		productList[productCount] = p;
		productCount++;
	}
}

void Game::addEnemyWolf(Wolf* w)
{
	if (enemyWolfCount < 50)
	{
		enemyWolfList[enemyWolfCount] = w;
		enemyWolfCount++;
	}
}

void Game::addChick(Chick* c)
{
	if (chickCount < 50)
	{
		chickList[chickCount] = c;
		chickCount++;
	}
}

void Game::addCow(Cow* c)
{
	if (cowCount < 50)
	{
		cowList[cowCount] = c;
		cowCount++;
	}
}

void Game::addGrass(Grass* g)
{
	if (grassCount < 200)
	{
		grassList[grassCount] = g;
		grassCount++;
	}
}

void Game::moveAllAnimals()
{
	for (int i = 0; i < chickCount; i++)
		if (chickList[i] != nullptr) chickList[i]->moveStep();

	for (int i = 0; i < cowCount; i++)
		if (cowList[i] != nullptr) cowList[i]->moveStep();

	for (int i = 0; i < enemyWolfCount; i++)
		if (enemyWolfList[i] != nullptr) enemyWolfList[i]->moveStep();
}

void Game::drawAllAnimals() const
{
	for (int i = 0; i < chickCount; i++)
		if (chickList[i] != nullptr) chickList[i]->draw();

	for (int i = 0; i < cowCount; i++)
		if (cowList[i] != nullptr) cowList[i]->draw();

	for (int i = 0; i < enemyWolfCount; i++)
		if (enemyWolfList[i] != nullptr) enemyWolfList[i]->draw();
}

// generateRandomWolf: spawns wolves with difficulty scaled by current level.
// Higher levels = higher spawn chance, faster interval, potentially more wolves at once.
void Game::generateRandomWolf()
{
	int spawnChance = level * 4;			// L1=4%, L2=8%, L3=12%, L4=16%, L5=20%
	int maxWolves = 20 + level * 10;		// More wolves allowed on field at higher levels
	if (timer > 0 && timer % max(1, 6 - level) == 0)	// L1=5s, L2=4s, L3=3s, L4=2s, L5=1s
	{
		int randomVal = rand() % 100;
		if (randomVal < spawnChance && enemyWolfCount < maxWolves)
		{
			int count = 1;
			if (level >= 2 && rand() % 100 < 40) count = 2;	// L2+: 40% chance for double spawn
			if (level >= 3 && rand() % 100 < 30) count = 3;	// L3+: 30% chance for triple spawn

			for (int i = 0; i < count && enemyWolfCount < maxWolves; i++)
			{
				int startX = rand() % (config.windWidth - 100);
				int startY = (2 * config.toolBarHeight) + rand() % 100;
				point p; p.x = startX; p.y = startY;
				Wolf* newWolf = new Wolf(this, p, 50, 50, "images\\wolf.jpg");
				addEnemyWolf(newWolf);
			}
		}
	}
}

void Game::drawFoodBox() const
{
	int boxW = 100;
	int boxH = 100;

	auto drawArea = [&](int boxX, int boxY, bool available, int counter, string label)
		{
			if (available)
			{
				if (pCachedFoodImage)
					pWind->DrawImage(pCachedFoodImage, boxX, boxY, boxW, boxH);
				else
					pWind->DrawImage(image("images\\food.jpg", JPEG), boxX, boxY, boxW, boxH);
				pWind->SetPen(WHITE);
				pWind->SetFont(20, BOLD, BY_NAME, "Arial");
				pWind->DrawInteger(boxX + 40, boxY - 25, counter);
			}
			else
			{
				if (pCachedEmptyBoxImage)
					pWind->DrawImage(pCachedEmptyBoxImage, boxX, boxY, 100, 100);
				else
					pWind->DrawImage(image("images\\emptybox.jpg", JPEG), boxX, boxY, 100, 100);
				pWind->SetPen(color(101, 67, 33));
				pWind->SetFont(14, BOLD, BY_NAME, "Arial");
				pWind->DrawString(boxX + 15, boxY + 35, label);
			}
			pWind->SetPen(color(101, 67, 33), 3);
			pWind->DrawRectangle(boxX, boxY, boxX + boxW, boxY + boxH, FRAME);
		};

	drawArea(foodX1, foodY1, foodAvailable1, foodCounter1, "FOOD 1");
	drawArea(foodX2, foodY2, foodAvailable2, foodCounter2, "FOOD 2");
	drawArea(foodX3, foodY3, foodAvailable3, foodCounter3, "FOOD 3");
}

void Game::decreaseFoodArea(int areaIndex)
{
	if (areaIndex == 1)
	{
		if (foodCounter1 > 0) foodCounter1--;
		if (foodCounter1 <= 0) { foodAvailable1 = false; foodCounter1 = 0; }
	}
	else if (areaIndex == 2)
	{
		if (foodCounter2 > 0) foodCounter2--;
		if (foodCounter2 <= 0) { foodAvailable2 = false; foodCounter2 = 0; }
	}
	else if (areaIndex == 3)
	{
		if (foodCounter3 > 0) foodCounter3--;
		if (foodCounter3 <= 0) { foodAvailable3 = false; foodCounter3 = 0; }
	}
}

void Game::addToWarehouse(ProductType type)
{
	switch (type)
	{
	case PRODUCT_EGG:  warehouseEggCount++;  break;
	case PRODUCT_MILK: warehouseMilkCount++; break;
	case PRODUCT_WOOL: warehouseWoolCount++; break;
	}
}

void Game::removeProduct(int index)
{
	if (index >= 0 && index < productCount && productList[index] != nullptr)
	{
		addToWarehouse(productList[index]->getType());
		ProductType type = productList[index]->getType();

		if (type == PRODUCT_EGG)  budget += 50;
		else if (type == PRODUCT_MILK) budget += 100;
		else if (type == PRODUCT_WOOL) budget += 70;

	// Check level up immediately after budget change
	checkLevelUp();

	// Update status bar immediately to reflect possible level change
	printStatusBar();
	pWind->UpdateBuffer();

	delete productList[index];
	productList[index] = nullptr;

		for (int i = index; i < productCount - 1; i++)
			productList[i] = productList[i + 1];
		productCount--;
	}
}

void Game::handleProductClick(int x, int y)
{
	for (int i = 0; i < productCount; i++)
	{
		if (productList[i] != nullptr && productList[i]->isClicked(x, y))
		{
			removeProduct(i);
			break;
		}
	}
}

// handleWolfClick: check if player clicked on a wolf. Each click increments hitCount.
// When hitCount reaches 5, the wolf is deleted and the array is compacted.
void Game::handleWolfClick(int x, int y)
{
	for (int i = 0; i < enemyWolfCount; i++)
	{
		if (enemyWolfList[i] != nullptr && enemyWolfList[i]->isClicked(x, y))
		{
			enemyWolfList[i]->hitCount++;
			char buf[64];
			sprintf(buf, "Wolf hit %d/5", enemyWolfList[i]->hitCount);
			printMessage(string(buf));

			if (enemyWolfList[i]->hitCount >= 5)
			{
				delete enemyWolfList[i];
				// Shift remaining wolves left to fill the gap in the array
				for (int k = i; k < enemyWolfCount - 1; k++)
					enemyWolfList[k] = enemyWolfList[k + 1];
				enemyWolfList[--enemyWolfCount] = nullptr;
				animalCount = (animalCount > 0) ? animalCount - 1 : 0;
				printMessage("Wolf defeated!");
			}
			break;
		}
	}
}

// saveGame: serialize complete game state to "savegame.txt" (plain text format)
void Game::saveGame() const
{
	ofstream file("savegame.txt");
	if (!file.is_open())
	{
		const_cast<Game*>(this)->printMessage("ERROR: Could not open save file!");
		return;
	}

	// Write core game state: level, budget, goal
	file << level << "\n";
	file << budget << "\n";
	file << goal << "\n";

	// Write timer as MM:SS format
	int mins = timer / 60;
	int secs = timer % 60;
	file << mins << ":" << (secs < 10 ? "0" : "") << secs << "\n";

	// Write animal counts (so loader knows how many positions to read)
	file << chickCount << "\n";
	file << cowCount << "\n";
	file << grassCount << "\n";
	file << enemyWolfCount << "\n";

	// Write each chick's position
	for (int i = 0; i < chickCount; i++)
		if (chickList[i])
			file << chickList[i]->curr_pos.x << "\n" << chickList[i]->curr_pos.y << "\n";

	// Write each cow's position
	for (int i = 0; i < cowCount; i++)
		if (cowList[i])
			file << cowList[i]->curr_pos.x << "\n" << cowList[i]->curr_pos.y << "\n";

	// Write grass positions (placeholder)
	for (int i = 0; i < grassCount; i++)
		file << 0 << "\n" << 0 << "\n";

	// Write each wolf's position
	for (int i = 0; i < enemyWolfCount; i++)
		if (enemyWolfList[i])
			file << enemyWolfList[i]->curr_pos.x << "\n" << enemyWolfList[i]->curr_pos.y << "\n";

	// Write warehouse: item count then type+count pairs (1=egg, 2=milk, 3=wool)
	file << 3 << "\n";
	file << 1 << " " << warehouseEggCount << "\n";
	file << 2 << " " << warehouseMilkCount << "\n";
	file << 3 << " " << warehouseWoolCount << "\n";

	// Write food box states: availability (0/1) + remaining counter
	file << (foodAvailable1 ? 1 : 0) << " " << foodCounter1 << "\n";
	file << (foodAvailable2 ? 1 : 0) << " " << foodCounter2 << "\n";
	file << (foodAvailable3 ? 1 : 0) << " " << foodCounter3 << "\n";

	file.close();
	const_cast<Game*>(this)->printMessage("Game saved!");
	// Ensure message is visible immediately before next frame redraw
	pWind->UpdateBuffer();
}

// loadGame: restore complete game state from "savegame.txt"
void Game::loadGame()
{
	ifstream file("savegame.txt");
	if (!file.is_open())
	{
		printMessage("ERROR: No save file found!");
		return;
	}

	// Helper lambdas for reading integer values and timer string from file
	auto readInt = [&](int& val) {
		string line;
		getline(file, line);
		val = stoi(line);
		};

	auto readTimer = [&]() {
		string line;
		getline(file, line);
		int m = 0, s = 0;
		sscanf(line.c_str(), "%d:%d", &m, &s);
		return m * 60 + s;
		};

	// Delete all existing game objects
	for (int i = 0; i < chickCount; i++) { delete chickList[i]; chickList[i] = nullptr; }
	chickCount = 0;
	for (int i = 0; i < cowCount; i++) { delete cowList[i]; cowList[i] = nullptr; }
	cowCount = 0;
	for (int i = 0; i < enemyWolfCount; i++) { delete enemyWolfList[i]; enemyWolfList[i] = nullptr; }
	enemyWolfCount = 0;
	for (int i = 0; i < grassCount; i++) { delete grassList[i]; grassList[i] = nullptr; }
	grassCount = 0;
	for (int i = 0; i < productCount; i++) { delete productList[i]; productList[i] = nullptr; }
	productCount = 0;
	animalCount = 0;

	// Read core game state
	readInt(level);
	readInt(budget);
	readInt(goal);
	timer = readTimer();

	// Read saved animal counts
	int savedChickCount, savedCowCount, savedGrassCount, savedWolfCount;
	readInt(savedChickCount);
	readInt(savedCowCount);
	readInt(savedGrassCount);
	readInt(savedWolfCount);

	// Recreate chicks at saved positions
	for (int i = 0; i < savedChickCount; i++)
	{
		int cx, cy;
		readInt(cx); readInt(cy);
		point p; p.x = cx; p.y = cy;
		addChick(new Chick(this, p, 50, 50, "images\\chick.jpg"));
		animalCount++;
	}

	// Recreate cows at saved positions
	for (int i = 0; i < savedCowCount; i++)
	{
		int cx, cy;
		readInt(cx); readInt(cy);
		point p; p.x = cx; p.y = cy;
		addCow(new Cow(this, p, 60, 50, "images\\cow.jpg"));
		animalCount++;
	}

	// Recreate grass at saved positions
	for (int i = 0; i < savedGrassCount; i++)
	{
		int gx, gy;
		readInt(gx); readInt(gy);
		point p; p.x = gx; p.y = gy;
		addGrass(new Grass(this, p, 150, 150, "images/greenarea.png"));
	}

	// Recreate wolves at saved positions
	for (int i = 0; i < savedWolfCount; i++)
	{
		int wx, wy;
		readInt(wx); readInt(wy);
		point p; p.x = wx; p.y = wy;
		addEnemyWolf(new Wolf(this, p, 60, 55, "images\\wolf.jpg"));
		animalCount++;
	}

	// Read warehouse contents (type+count pairs)
	int warehouseItems;
	readInt(warehouseItems);
	warehouseEggCount = warehouseMilkCount = warehouseWoolCount = 0;
	for (int i = 0; i < warehouseItems; i++)
	{
		string line;
		getline(file, line);
		int type = 0, count = 0;
		sscanf(line.c_str(), "%d %d", &type, &count);
		if (type == 1) warehouseEggCount = count;
		if (type == 2) warehouseMilkCount = count;
		if (type == 3) warehouseWoolCount = count;
	}

	// Read food box states: availability (0/1) + remaining counter
	string line;
	int avail, counter;

	getline(file, line); sscanf(line.c_str(), "%d %d", &avail, &counter);
	foodAvailable1 = (avail == 1); foodCounter1 = counter;

	getline(file, line); sscanf(line.c_str(), "%d %d", &avail, &counter);
	foodAvailable2 = (avail == 1); foodCounter2 = counter;

	getline(file, line); sscanf(line.c_str(), "%d %d", &avail, &counter);
	foodAvailable3 = (avail == 1); foodCounter3 = counter;

	file.close();
	printMessage("Game loaded!");
	// Ensure message is visible immediately before next frame redraw
	pWind->UpdateBuffer();
}

// resetGame: completely wipes all game state and restarts fresh at level 1
void Game::resetGame()
{
	// Delete every existing object in all 5 arrays
	for (int i = 0; i < chickCount; i++) { delete chickList[i]; chickList[i] = nullptr; }
	chickCount = 0;
	for (int i = 0; i < cowCount; i++) { delete cowList[i]; cowList[i] = nullptr; }
	cowCount = 0;
	for (int i = 0; i < enemyWolfCount; i++) { delete enemyWolfList[i]; enemyWolfList[i] = nullptr; }
	enemyWolfCount = 0;
	for (int i = 0; i < grassCount; i++) { delete grassList[i]; grassList[i] = nullptr; }
	grassCount = 0;
	for (int i = 0; i < productCount; i++) { delete productList[i]; productList[i] = nullptr; }
	productCount = 0;
	animalCount = 0;

	// Reset game state to fresh level 1 defaults
	budget = 2000;
	level = 1;
	timer = getTimerByLevel(1);	// 90 seconds for level 1
	lastTick = time(0);
	paused = false;				// Unpause if game was paused
	updateGoal();

	// Clear warehouse
	warehouseEggCount = 0;
	warehouseMilkCount = 0;
	warehouseWoolCount = 0;

	// Reset all 3 food boxes to empty
	foodCounter1 = 0;
	foodCounter2 = 0;
	foodCounter3 = 0;
	foodAvailable1 = false;
	foodAvailable2 = false;
	foodAvailable3 = false;

	// Reset food box Y positions
	int fieldStartY = 2 * config.toolBarHeight;
	int spacing = 130;
	foodY1 = fieldStartY + 100;
	foodY2 = fieldStartY + 100 + spacing;
	foodY3 = fieldStartY + 100 + (2 * spacing);

	// Reset budget bar icons (local animal pointer arrays) and per-type static counters
	if (gameBudgetbar) gameBudgetbar->resetIcons();
	Chick::resetCounter();
	Cow::resetCounter();
	Wolf::resetCounter();

	printMessage("Game Restarted");
}

void Game::checkWolfCollisions()
{
	const int tolerance = 40;

	for (int wi = 0; wi < enemyWolfCount; wi++)
	{
		Wolf* wolf = enemyWolfList[wi];
		if (!wolf) continue;

		int wolfCX = wolf->curr_pos.x + 30;
		int wolfCY = wolf->curr_pos.y + 27;

		for (int ci = 0; ci < chickCount; ci++)
		{
			if (!chickList[ci]) continue;
			int cx = chickList[ci]->curr_pos.x + 25;
			int cy = chickList[ci]->curr_pos.y + 25;
			if (abs(wolfCX - cx) < tolerance && abs(wolfCY - cy) < tolerance)
			{
				delete chickList[ci];
				for (int k = ci; k < chickCount - 1; k++)
					chickList[k] = chickList[k + 1];
				chickList[--chickCount] = nullptr;
				animalCount = (animalCount > 0) ? animalCount - 1 : 0;
				ci--;
			}
		}

		for (int ci = 0; ci < cowCount; ci++)
		{
			if (!cowList[ci]) continue;
			int cx = cowList[ci]->curr_pos.x + 30;
			int cy = cowList[ci]->curr_pos.y + 25;
			if (abs(wolfCX - cx) < tolerance && abs(wolfCY - cy) < tolerance)
			{
				delete cowList[ci];
				for (int k = ci; k < cowCount - 1; k++)
					cowList[k] = cowList[k + 1];
				cowList[--cowCount] = nullptr;
				animalCount = (animalCount > 0) ? animalCount - 1 : 0;
				ci--;
			}
		}
	}
}

void Game::checkLevelUp()
{
	if (budget >= goal)
	{
        level++;
		updateGoal();
		timer = getTimerByLevel(level);
		lastTick = time(0);
		printMessage("Level Up! Now at Level " + to_string(level) + "  |  New Goal: $" + to_string(goal));
	}
}

void Game::updateGoal()
{
    int base = 300;
	int exponent = (level > 1) ? (level - 1) : 0;
	double multiplier = std::pow(1.5, exponent);
	int levelBase = static_cast<int>(base * multiplier);
    // make goal slightly above current budget (5% above) so goal is 2100 when budget is 2000
	int minGoal = static_cast<int>(budget * 1.05);
	goal = (levelBase > minGoal) ? levelBase : minGoal;
}
bool Game::isInsideWarehouse(int x, int y) const
{
	// These must match the exact dimensions/math you used in drawWarehouse()
	int wareWidth = 120;
	int wareHeight = 120;
	int wareX = config.windWidth - wareWidth - 20;
	int wareY = config.windHeight - config.statusBarHeight - wareHeight - 20;

	// Check if the X and Y coordinates of the mouse click fall inside this box
	if (x >= wareX && x <= (wareX + wareWidth) &&
		y >= wareY && y <= (wareY + wareHeight))
	{
		return true;
	}
	return false;
}

void Game::openWarehouseWindow()
{
	// 1. Create the new smaller window
	window* wareWind = new window(400, 400, 200, 200);
	wareWind->ChangeTitle("Warehouse Inventory");

	bool closeWindow = false;
	int clickX, clickY;

	// 2. Keep the window open until the user clicks "CLOSE"
	while (!closeWindow)
	{
		// --- DRAWING PHASE ---

		// Draw Background
		wareWind->SetBrush(WHITE);
		wareWind->SetPen(BLACK, 2);
		wareWind->DrawRectangle(0, 0, 400, 400);

		// Draw Title
		wareWind->SetPen(BLACK);
		wareWind->SetFont(24, BOLD, BY_NAME, "Arial");
		wareWind->DrawString(100, 20, "Warehouse Contents");

		// Loop through the productList array and draw a row for each product
		int yPos = 80; // Starting Y coordinate for the first product

		for (int i = 0; i < productCount; i++)
		{
			if (productList[i] != nullptr)
			{
				// Draw the Product Name
				wareWind->SetPen(BLACK);
				wareWind->SetFont(16, BOLD, BY_NAME, "Arial");
				wareWind->DrawString(50, yPos, "Animal Product");

				// Draw the "SELL" Button (Green Box)
				wareWind->SetBrush(color(0, 200, 0));
				wareWind->SetPen(BLACK, 1);
				wareWind->DrawRectangle(200, yPos - 5, 280, yPos + 25);

				// Draw "SELL" Text inside the button
				wareWind->SetPen(WHITE);
				wareWind->DrawString(220, yPos, "SELL");

				yPos += 40; // Move down 40 pixels for the next product

				// Safety check: Stop drawing if we run out of window space
				if (yPos > 300) break;
			}
		}

		// Draw "CLOSE" Button at the bottom
		wareWind->SetBrush(RED);
		wareWind->SetPen(BLACK, 1);
		wareWind->DrawRectangle(150, 350, 250, 380);
		wareWind->SetPen(WHITE);
		wareWind->DrawString(175, 355, "CLOSE");


		// --- INPUT PHASE ---

		// Pause and wait for the user to click inside this window
		wareWind->WaitMouseClick(clickX, clickY);


		// --- LOGIC PHASE ---

		// Check if they clicked inside the red CLOSE button hitbox
		if (clickX >= 150 && clickX <= 250 && clickY >= 350 && clickY <= 380)
		{
			closeWindow = true; // This will break the while loop
		}

		// Check if they clicked any of the green SELL buttons
		int checkY = 80;
		for (int i = 0; i < productCount; i++)
		{
			if (productList[i] != nullptr)
			{
				// Hitbox math: X is between 200 and 280. Y is between the top and bottom of the current button.
				if (clickX >= 200 && clickX <= 280 && clickY >= (checkY - 5) && clickY <= (checkY + 25))
				{
					// 1. INCREASE THE BUDGET
					budget += 50; // You can change 50 to whatever price you want!

					// 2. DELETE THE PRODUCT FROM MEMORY
					delete productList[i];

					// 3. SHIFT THE ARRAY LEFT 
					// (To prevent empty gaps in the array which crash the game)
					for (int j = i; j < productCount - 1; j++)
					{
						productList[j] = productList[j + 1];
					}

					// 4. CLEAN UP THE END OF THE ARRAY
					productList[productCount - 1] = nullptr;
					productCount--;

					// Break the logic loop immediately so it safely restarts drawing the updated list
					break;
				}

				checkY += 40; // Move our hitbox checker down to the next row
				if (checkY > 300) break;
			}
		}
	}

	// 3. Clean up and close the window
	delete wareWind;
}