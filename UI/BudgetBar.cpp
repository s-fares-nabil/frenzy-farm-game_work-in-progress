#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../Entities/Grass.h"
#include <iostream>
using namespace std;


FoodIcon::FoodIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}

void FoodIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetBrush(color(255, 200, 50));
	pWind->SetPen(color(180, 120, 0), 2);
	pWind->DrawRectangle(RefPoint.x, RefPoint.y,
		RefPoint.x + width, RefPoint.y + height, FILLED);
	pWind->SetPen(color(100, 50, 0));
	pWind->SetFont(14, BOLD, BY_NAME, "Arial");
	pWind->DrawString(RefPoint.x + 10, RefPoint.y + 25, "FOOD");
}

void FoodIcon::onClick()
{
	if (pGame->budget >= 20)
	{
		pGame->budget -= 20;
		// Add food to all 3 areas
		pGame->foodAvailable1 = true;
		pGame->foodCounter1 = 5;
		pGame->foodAvailable2 = true;
		pGame->foodCounter2 = 5;
		pGame->foodAvailable3 = true;
		pGame->foodCounter3 = 5;

		pGame->clearBudget();
		pGame->printBudget("BUDGET = $" + to_string(pGame->budget));
		pGame->drawFoodBox();
		pGame->printMessage("Food added to all 3 areas!");
	}
	else
		pGame->printMessage("Not enough budget!");
}

BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	pCachedImage = new image(img_path, JPEG);
}

void BudgetbarIcon::draw() const
{
	window* pWind = pGame->getWind();
	if (pCachedImage)
		pWind->DrawImage(pCachedImage, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	chickList = new Chick * [15];
	for (int i = 0; i < 10; i++) {
		chickList[i] = nullptr;
	}
}



void ChickIcon::resetCounts()
{
	for (int i = 0; i < count; i++) chickList[i] = nullptr;
	count = 0;
}

void ChickIcon::onClick()
{
	cout << "Icon Chick Clicked" << endl;
	if (pGame->budget >= 100) {
		pGame->budget -= 100;
		pGame->clearBudget();
		pGame->printBudget("BUDGET = $" + to_string(pGame->budget));

		point p;
		std::random_device rd1, rd2;
		std::mt19937 gen1(rd1()), gen2(rd2());
		std::uniform_int_distribution<int> distX(range_min_x, range_max_x);
		std::uniform_int_distribution<int> distY(range_min_y, range_max_y);
		p.x = distX(gen1);
		p.y = distY(gen2);

		// Create new chick and add to BOTH arrays
		Chick* newChick = new Chick(pGame, p, 50, 50, image_path);
		chickList[count] = newChick;  // Local array for icon
		pGame->addChick(newChick);    // Game's array for movement
		count++;

		newChick->draw();
		pGame->setAnimalCount(pGame->animalCount + 1);
		pGame->printStatusBar();
	}
}
CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	cowList = new Cow * [15];
	for (int i = 0; i < 15; i++) cowList[i] = nullptr;
}

void CowIcon::resetCounts()
{
	for (int i = 0; i < count; i++) cowList[i] = nullptr;
	count = 0;
}

void CowIcon::onClick()
{
	cout << "Icon Cow Clicked" << endl;
	if (pGame->budget >= 200)
	{
		pGame->budget -= 200;
		pGame->clearBudget();
		pGame->printBudget("BUDGET = $" + to_string(pGame->budget));

		std::random_device rd1, rd2;
		std::mt19937 gen1(rd1()), gen2(rd2());
		std::uniform_int_distribution<int> distX(range_min_x, range_max_x);
		std::uniform_int_distribution<int> distY(range_min_y, range_max_y);

		point p;
		p.x = distX(gen1);
		p.y = distY(gen2);

		Cow* newCow = new Cow(pGame, p, 60, 50, image_path);
		cowList[count] = newCow;
		pGame->addCow(newCow);
		newCow->draw();
		count++;

		// update animal count in status bar
		pGame->setAnimalCount(pGame->animalCount + 1);
		pGame->printStatusBar();
	}
}
WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}
void WaterIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(color(0, 100, 200));
	pWind->SetBrush(color(100, 180, 255));
	pWind->DrawRectangle(RefPoint.x, RefPoint.y,
		RefPoint.x + width, RefPoint.y + height, FILLED);
	pWind->SetPen(WHITE);
	pWind->SetFont(14, BOLD, BY_NAME, "Arial");
	pWind->DrawString(RefPoint.x + 5, RefPoint.y + 15, "WATER");
}

void WaterIcon::onClick()
{
	cout << "Icon Water Clicked" << endl;
	if (pGame->budget >= 20)
	{
		pGame->budget -= 20;
		pGame->clearBudget();
		pGame->printBudget("BUDGET = $" + to_string(pGame->budget));

		// Generate random position for grass
		std::random_device rd1, rd2;
		std::mt19937 gen1(rd1()), gen2(rd2());
		std::uniform_int_distribution<int> distX(range_min_x, range_max_x);
		std::uniform_int_distribution<int> distY(range_min_y, range_max_y);

		point p;
		p.x = distX(gen1);
		p.y = distY(gen2);

		// Create and add grass to the game
		Grass* newGrass = new Grass(pGame, p, 150, 150, "images/grass.jpg");
		pGame->addGrass(newGrass);

		pGame->printMessage("Water bought! Grass has grown.");
	}
	else
	{
		pGame->printMessage("Not enough budget for water!");
	}
}




Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	//First prepare List of images for each icon
	//To control the order of these images in the menu, reoder them in enum ICONS above	
	iconsImages[ICON_CHICK] = "images\\chick.jpg";
	iconsImages[ICON_COW] = "images\\cow.jpg";
	iconsImages[ICON_WATER] = "images\\Water.jpg";
	iconsImages[ICON_FOOD] = "images\\food.jpg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
	p.x += config.iconWidth;
	iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]); 
	p.x += config.iconWidth;
	iconsList[ICON_WATER] = new WaterIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_WATER]); 
	p.x += config.iconWidth;
	iconsList[ICON_FOOD] = new FoodIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_FOOD]);
	p.x += config.iconWidth;

}

Budgetbar::~Budgetbar()
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		delete iconsList[i];
    delete[] iconsList;
}

void Budgetbar::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);

	for (int i = 0; i < ANIMAL_COUNT; i++)
		iconsList[i]->draw();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, 2*config.toolBarHeight, pWind->GetWidth(), 2*config.toolBarHeight);
	pWind->SetFont(13, BOLD, BY_NAME, "Arial");

	// Chicken label
	pWind->SetPen(color(255, 140, 0));
	pWind->DrawString(5, config.toolBarHeight + 20, "Chicken");
	pWind->DrawString(5, config.toolBarHeight + 40, "$100");

	// Cow label
	pWind->SetPen(color(139, 90, 43));
	pWind->DrawString(config.iconWidth + 5, config.toolBarHeight + 20, "Cow");
	pWind->DrawString(config.iconWidth + 5, config.toolBarHeight + 40, "$200");

	// Water label
	pWind->SetPen(color(0, 120, 255));
	pWind->DrawString(2 * config.iconWidth + 5, config.toolBarHeight + 20, "Water");
	pWind->DrawString(2 * config.iconWidth + 5, config.toolBarHeight + 40, "$20");

	//Food label
	pWind->SetPen(color(180, 120, 0));
	pWind->DrawString(3 * config.iconWidth + 5, config.toolBarHeight + 20, "Food");
	pWind->DrawString(3 * config.iconWidth + 5, config.toolBarHeight + 40, "$20");
}


void Budgetbar::resetIcons()
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		iconsList[i]->resetCounts();
}

bool Budgetbar::handleClick(int x, int y)
{
	if (x > ANIMAL_COUNT * config.iconWidth)	//click outside toolbar boundaries
		return false;


	//Check whick icon was clicked
	//==> This assumes that menu icons are lined up horizontally <==
	//Divide x co-ord of the point clicked by the icon width (int division)
	//if division result is 0 ==> first icon is clicked, if 1 ==> 2nd icon and so on

	int clickedIconIndex = (x / config.iconWidth);
    if (clickedIconIndex < 0 || clickedIconIndex >= ANIMAL_COUNT)
	{
		pGame->printMessage("Budgetbar: Click index out of range");
		return false;
	}
	pGame->printMessage("Budgetbar icon clicked: " + to_string(clickedIconIndex));
	iconsList[clickedIconIndex]->onClick();	//execute onClick action of clicled icon

	//if (clickedIconIndex == ICON_EXIT) return true;

	return false;

}