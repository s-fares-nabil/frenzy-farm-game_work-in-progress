#include "Grass.h"
#include "../Core/Game.h"
#include <iostream>

Grass::Grass(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
    : Drawable(r_pGame, r_point, r_width, r_height)
{
    pCachedImage = new image(img_path, JPEG);
    foodAmount = 5;
}

void Grass::draw() const
{
    window* pWind = pGame->getWind();

    if (pCachedImage)
        pWind->DrawImage(pCachedImage, RefPoint.x, RefPoint.y, width, height);
}

void Grass::decreaseFood() {
    if (foodAmount > 0) foodAmount--;
}

int Grass::getFoodAmount() const {
    return foodAmount;
}


