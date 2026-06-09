#pragma once
#include "../Core/Drawable.h"

//Base class for all toolbar icons 
class ToolbarIcon :public Drawable
{
private:
	image* pCachedImage;
public:
	ToolbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	~ToolbarIcon() { delete pCachedImage; }
	virtual void draw() const override;
	virtual void onClick() = 0;   //The action that should be taken when this icon is clicked
};
// PauseIcon: sets Game::paused = true to freeze all game logic
class PauseIcon : public ToolbarIcon {
public:
	PauseIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};

// ResumeIcon: sets Game::paused = false to resume game logic
class ResumeIcon : public ToolbarIcon {
public:
	ResumeIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};

// SaveIcon: triggers serialization of complete game state to file
class SaveIcon : public ToolbarIcon {
public:
	SaveIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};

// LoadingIcon: triggers deserialization of game state from file
class LoadingIcon : public ToolbarIcon {
public:
	LoadingIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};
// RestartIcon: calls Game::resetGame() to wipe all state and start fresh at level 1
class RestartIcon : public ToolbarIcon
{
public:
	RestartIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class ExitIcon : public ToolbarIcon
{
public:
	ExitIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};


// TO DO: The rest of icons in the toolbar

enum ICONS //The icons of the toolbar
{
	ICON_RESTART,
	ICON_PAUSE,
	ICON_RESUME,
	ICON_SAVE,
	ICON_LOADING,
	ICON_EXIT,		//Exit icon
	ICON_COUNT		//no. of menu icons ==> This should be the last line
};

class Toolbar : public Drawable
{
private:
	ToolbarIcon** iconsList; //an array of toolbarIcon pointers
	string iconsImages[ICON_COUNT];

public:
	Toolbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Toolbar();
	void draw() const override;
	bool handleClick(int x, int y);	//handles clicks on toolbar icons, returns true if exit is clicked

};

