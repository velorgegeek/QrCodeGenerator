#pragma once
#include <vector>
struct Node {
	enum class CellStatus :uint8_t {
		none, data, serviceData
	};
	CellStatus status :2;
	uint8_t cell : 1;
	void operator=(bool byte) {
		cell = byte;
	}
	Node() {
		cell = 0;
		status = CellStatus::none;
	}
};

class Matrix {

	uint8_t sizeMatrix;
	uint8_t qrVersion;
	uint8_t correctionLevel;
	std::vector<uint8_t> data;


	void createMatrix();
	void searchPatternFill(int i, int j, int ik, int jk, bool flag = false, int count = 0);
	void syncBands(int i, int j);
	void codeVersionFill();
	void addLevelingPattern();
	void addMaskMatrix(std::vector < std::vector<Node>>& matrix, int mask, std::vector<std::pair<std::vector<std::vector<Node>>, size_t>>* result);
	void addMaskAndCorrection(std::vector < std::vector<Node>>& matrix,int flag = -1);
	void addPadding();
	void seekBestMask();
	void addLevelingPatternFill(int i, int j, int ik, int jk, bool flag, int count = 0);
	void addMask(std::vector < std::vector<Node>>& matrix,int mask);
	void addData();
	void step(int& i, int& j, bool& up, bool& a);
	void costPenaltyPoints(const std::vector < std::vector<Node>>& matrix, size_t& penaltyPoints) const;
public:
	std::vector<std::vector<Node>> matrix;
	void init(const uint8_t& size, const uint8_t correctionLevel, std::vector<uint8_t> data);
};