#pragma once
#include "../Core/Drawable.h"
#include "../CMUgraphicsLib/image.h"
#include <string>

class Game;

enum ProductType
{
	PRODUCT_EGG,
	PRODUCT_MILK,
	PRODUCT_WOOL
};

class Product : public Drawable
{
private:
	std::string image_path;
	ProductType productType;
	image* pCachedImage;  // Cached image to avoid re-loading every frame
public:
	Product(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path, ProductType type = PRODUCT_EGG);
	~Product();  // Destructor to clean up cached image
	virtual void draw() const override;
	ProductType getType() const { return productType; }
	bool isClicked(int x, int y) const;
};