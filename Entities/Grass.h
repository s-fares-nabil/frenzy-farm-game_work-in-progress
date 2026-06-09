#pragma once
#include "../Core/Drawable.h"
using namespace std;

class Grass : public Drawable
{
protected:
    image* pCachedImage;
    int foodAmount;

public:
    Grass(Game* r_pGame, point r_point, int r_width, int r_height,string img_path);
    ~Grass() { delete pCachedImage; }
    virtual void draw() const override;
    void decreaseFood();      // بتقلل العداد
    int getFoodAmount() const; // بترجع القيمة الحالية
};
class grassarea : public Grass
{
public:
    grassarea(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
    virtual void moveStep();
};