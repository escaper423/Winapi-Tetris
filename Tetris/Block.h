#pragma once
#include <vector>

typedef struct {
	int x;
	int y;
} Point;

class Block {
public:
	Block() {
		block.resize(4);
	}
	Block(Point cp[4]) {
		block.resize(4);
		for (int i = 0; i < 4; i++)
				block[i] = cp[i];
	}
	Block(const Block& cp) {
		*this = cp;
	}

	void setColor(int clr) { color = clr; }
	void setBlock(const std::vector<Point> &p) { block = p; }
	std::vector<Point> getBlock() { return block; }
	int getColor() { return color; }
	
private:
	std::vector<Point> block;
	int color = 0;
};