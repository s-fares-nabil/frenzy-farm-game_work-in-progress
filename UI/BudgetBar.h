#pragma once
#include <string>
using namespace std;
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"
#include <random>
#include <limits>
//Ranges for generating a random location for the animals
const int range_min_x = 50;
const int range_max_x = config.windWidth - 50;
const int range_min_y = (config.toolBarHeight * 2) + 50;
const int range_max_y = config.windHeight - config.statusBarHeight - 50;

//Base class for all toolbar icons 
class BudgetbarIcon :public Drawable
{
protected:
	image* pCachedImage;
	string image_path;
public:
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	~BudgetbarIcon() { delete pCachedImage; }
	virtual void draw() const override;
	virtual void onClick() = 0;   //The action that should be taken when this icon is clicked
	virtual void resetCounts() {} // Reset local animal pointer arrays (called on game restart via Budgetbar::resetIcons())
};

class ChickIcon : public BudgetbarIcon
{
public:
	Chick** chickList; //an array of Chick pointers
	int count = 0;
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	virtual void resetCounts() override;
};
class CowIcon : public BudgetbarIcon
{
public:
	Cow** cowList;
	int count = 0;
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
	virtual void resetCounts() override;
};
class WaterIcon : public BudgetbarIcon
{
public:
	WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;  // ← add this line
	virtual void onClick();
};
class FoodIcon : public BudgetbarIcon
{
public:
	FoodIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() override;
};


// TO DO: The rest of icons in the toolbar

enum ANIMAL_ICONS
{
	ICON_CHICK,
	ICON_COW,
	ICON_WATER,
	ICON_FOOD,
	ANIMAL_COUNT
};

class Budgetbar : public Drawable
{
private:
	BudgetbarIcon** iconsList; //an array of toolbarIcon pointers
	string iconsImages[ANIMAL_COUNT];

public:
	Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Budgetbar();
	void draw() const override;
	bool handleClick(int x, int y);	//handles clicks on toolbar icons, returns true if exit is clicked
	void resetIcons();	// Call resetCounts() on every icon in the budget bar (used on game restart)

};

