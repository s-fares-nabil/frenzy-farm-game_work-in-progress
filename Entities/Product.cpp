#include "Product.h"
#include "../Core/Game.h"
#include <iostream>

using namespace std;

Product::Product(Game* r_pGame, point r_point, int r_width, int r_height, std::string img_path, ProductType type)
	: Drawable(r_pGame, r_point, r_width, r_height), productType(type), pCachedImage(nullptr)
{
	image_path = img_path;
	cout << "[DEBUG] Product created at position: (" << r_point.x << ", " << r_point.y << ") with image: " << img_path << endl;

	// Load image once during construction (cache it)
	try {
		pCachedImage = new image(img_path, JPEG);
		cout << "[DEBUG] Image cached: " << img_path << endl;
	}
	catch (...) {
		cout << "[ERROR] Failed to load image: " << img_path << endl;
		pCachedImage = nullptr;
	}
}

Product::~Product()
{
	// Clean up cached image
	if (pCachedImage) {
		delete pCachedImage;
		pCachedImage = nullptr;
	}
}

void Product::draw() const
{
	window* pWind = pGame->getWind();

	// Draw the cached product image (no re-decoding!)
	if (pCachedImage) {
		pWind->DrawImage(*pCachedImage, RefPoint.x, RefPoint.y, width, height);
	}
	else {
		// Fallback: try to load from path if cache failed (slow path, rarely taken)
		pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
	}
}

bool Product::isClicked(int x, int y) const
{
	// Check if click is within product bounds
	return (x >= RefPoint.x && x <= RefPoint.x + width &&
			y >= RefPoint.y && y <= RefPoint.y + height);
}