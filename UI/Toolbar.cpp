#include "Toolbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>

ToolbarIcon::ToolbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	pCachedImage = new image(img_path, JPEG);
}

RestartIcon::RestartIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}

// RestartIcon::onClick: completely wipes all game state and starts fresh at level 1
void RestartIcon::onClick()
{
	pGame->resetGame();
}

ExitIcon::ExitIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}

void ExitIcon::onClick()
{
	pGame->getWind()->SetWaitClose(false);
}

PauseIcon::PauseIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}
// PauseIcon::onClick: freeze all game logic by setting paused flag
void PauseIcon::onClick()
{
	pGame->paused = true;
	pGame->printMessage("Game Paused");
}

ResumeIcon::ResumeIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}
// ResumeIcon::onClick: re-enable all game logic by clearing paused flag
void ResumeIcon::onClick()
{
	pGame->paused = false;
	pGame->printMessage("Game Resumed");
}

SaveIcon::SaveIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}
// SaveIcon::onClick: serialize complete game state to "savegame.txt"
void SaveIcon::onClick()
{
    std::cout << "Save icon clicked" << std::endl;
	// provide immediate feedback and trigger save
	pGame->printMessage("Saving game...");
	pGame->saveGame();
}

LoadingIcon::LoadingIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {}
// LoadingIcon::onClick: restore game state from "savegame.txt"
void LoadingIcon::onClick()
{
    std::cout << "Load icon clicked" << std::endl;
	// provide immediate feedback and trigger load
	pGame->printMessage("Loading game...");
	pGame->loadGame();
}

void ToolbarIcon::draw() const
{
	window* pWind = pGame->getWind();
	if (pCachedImage)
		pWind->DrawImage(pCachedImage, RefPoint.x, RefPoint.y, width, height);
}

Toolbar::Toolbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	string iconsImages[ICON_COUNT];
	iconsImages[ICON_RESTART] = "images\\RESTART.jpg";
	iconsImages[ICON_PAUSE] = "images\\PAUSE.jpg";
	iconsImages[ICON_RESUME] = "images\\RESUME.jpg";
	iconsImages[ICON_SAVE] = "images\\SAVE.jpg";
	iconsImages[ICON_LOADING] = "images\\LOADING.jpg";
	iconsImages[ICON_EXIT] = "images\\EXIT.jpg";

	point p;
	p.x = 0; p.y = 0;

	iconsList = new ToolbarIcon * [ICON_COUNT];

	iconsList[ICON_RESTART] = new RestartIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESTART]);
	p.x += config.iconWidth;
	iconsList[ICON_PAUSE] = new PauseIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_PAUSE]);
	p.x += config.iconWidth;
	iconsList[ICON_RESUME] = new ResumeIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESUME]);
	p.x += config.iconWidth;
	iconsList[ICON_SAVE] = new SaveIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_SAVE]);
	p.x += config.iconWidth;
	iconsList[ICON_LOADING] = new LoadingIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_LOADING]);
	p.x += config.iconWidth;
	iconsList[ICON_EXIT] = new ExitIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_EXIT]);
}

Toolbar::~Toolbar()
{
	for (int i = 0; i < ICON_COUNT; i++)
		delete iconsList[i];
    delete[] iconsList;
}

void Toolbar::draw() const
{
	for (int i = 0; i < ICON_COUNT; i++)
		iconsList[i]->draw();
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, config.toolBarHeight, pWind->GetWidth(), config.toolBarHeight);
}

bool Toolbar::handleClick(int x, int y)
{
	if (x > ICON_COUNT * config.iconWidth)
		return false;

	int clickedIconIndex = (x / config.iconWidth);
	// safety check
	if (clickedIconIndex < 0 || clickedIconIndex >= ICON_COUNT)
	{
		pGame->printMessage("Toolbar: Click index out of range");
		return false;
	}

	// debug feedback
    std::cout << "Toolbar icon clicked: " << clickedIconIndex << std::endl;
	pGame->printMessage("Toolbar icon clicked: " + to_string(clickedIconIndex));

	iconsList[clickedIconIndex]->onClick();

	if (clickedIconIndex == ICON_EXIT) return true;

	return false;
}