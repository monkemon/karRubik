#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <memory>
#include <optional>
#include <SDL2/SDL.h>

#include "enums.h"

using Face = std::vector<CubeColor>;
using CubeData = std::vector<Face>;

//faces: F : 1,  R : 2, B : 3, L : 4, U : 5, D : 6
//RC class responsible of holding and transformation of data
class RC
{
private:
    size_t sideLength;
	size_t 		 size;
	CubeData 	 data;

	void rotate(std::vector<CubeColor*>& refs, bool clockwise);
public:
	RC() = delete;
	RC(size_t sideLength);

	size_t getSize();

	void print();
	void restore();
	void set(CubeColor c, int x, int y);
	void rotateFace(int faceID, bool clockwise);

	void performRight(int from, int to, bool clockwise);
	void performUp   (int from, int to, bool clockwise);
	void performFront(int from, int to, bool clockwise);
	void performLeft (int from, int to, bool clockwise);
	void performDown (int from, int to, bool clockwise);
	void performBack (int from, int to, bool clockwise);

    CubeData& getData();
	std::optional<CubeColor> getElement(size_t x, size_t y);
};
