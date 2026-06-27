#include "Matrix.h"
#include <thread>
#include "AllData.h" 
#include <array>
#include <iostream>
void Matrix::init(const uint8_t& size, const uint8_t correctionLevel, std::vector<uint8_t> data) {
	this->qrVersion = size;
	this->sizeMatrix = (4*size) + 17;
	this->correctionLevel = correctionLevel;
	this->data = std::move(data);
	matrix.resize(sizeMatrix, std::vector<Node>(sizeMatrix));
	createMatrix();
}
void Matrix::searchPatternFill(int i, int j, int ik, int jk,bool flag, int count ) {
	count++;
	if (ik == 0) {
		matrix[i][j] = !flag;
		matrix[i][j].status = Node::CellStatus::serviceData;
		ik = i;
		jk = j;

	}
	else {
		int indexI = i;
		int  indexJ = j;
		for (indexI; indexI < ik; indexI++) {
			if (indexI < 0 || indexJ < 0 || indexI >= sizeMatrix || indexJ >= sizeMatrix) {
				continue;
			}
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
		for (indexJ; indexJ < jk; indexJ++) {
			if (indexI < 0 || indexJ < 0 || indexI >= sizeMatrix || indexJ >= sizeMatrix) {
				continue;
			}
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
		for (indexI; indexI > i; indexI--) {
			if (indexI < 0 || indexJ < 0 || indexI >= sizeMatrix || indexJ >= sizeMatrix) {
				continue;
			}
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;

		}

		for (indexJ; indexJ > j; indexJ--) {
			if (indexI < 0 || indexJ < 0 || indexI >= sizeMatrix || indexJ >= sizeMatrix) {
				continue;
			}
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
	}
	if (count < 5) {

		searchPatternFill(i - 1, j - 1, ik + 1, jk + 1, !flag, count);
	}
}
void Matrix::syncBands(int i, int j) {
	for (int firstI = 0; matrix[firstI + i][j].status == Node::CellStatus::none; firstI++) {
		bool flag = (firstI % 2 == 0);
		matrix[firstI + i][j] = flag;
		matrix[j][firstI + i] = flag;
		matrix[firstI + i][j].status = Node::CellStatus::serviceData;
		matrix[j][firstI + i].status = Node::CellStatus::serviceData;
	}
}
void Matrix::codeVersionFill() {
	if (qrVersion < 7) {
		return;
	}
	int index = sizeMatrix - 9;
	const AllData& data = AllData::instance();
	const auto version = AllData::getVersionMatrix()[qrVersion-7];
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 6; i++) {
			uint8_t num = version[j];
			bool b = num & (1 << (5 - i));

			matrix[index - j][i] = b;	
			matrix[i][index - j] = b;
			matrix[index - j][i].status = Node::CellStatus::serviceData;
			matrix[i][index - j].status = Node::CellStatus::serviceData;
		}
	}
}
bool Matrix::levelingPatternIntersect(const std::vector < std::pair<int, int>>& cord) {
	for (const auto& pair : cord) {
		if (matrix[pair.first][pair.second].status == Node::CellStatus::serviceData) {
			return true;
		}
	}
	return false;
}
void Matrix::addLevelingPattern() {
	if (qrVersion < 2) {
		return;
	}

	const AllData& data = AllData::instance();
	const auto positionLeveling = data.aligmentPatterns[qrVersion-1];
	
	for (const auto& i : positionLeveling) {
		for (const auto& j : positionLeveling) {
			std::vector < std::pair<int, int>> cord;

			cord.emplace_back(std::make_pair(i-2, j-2));
			cord.emplace_back(std::make_pair(i - 2, j + 2));
			cord.emplace_back(std::make_pair(i + 2, j - 2));
			if (!levelingPatternIntersect(cord)) {
				addLevelingPatternFill(i, j, 0, 0, true);
			}
		}
	}
}
void Matrix::addLevelingPatternFill(int i, int j, int ik, int jk, bool flag, int count) {

	count++;
	if (ik == 0) {
		matrix[i][j] = flag;
		matrix[i][j].status = Node::CellStatus::serviceData;
		ik = i;
		jk = j;
	}
	else {
		int indexI = i;
		int  indexJ = j;
		for (indexI; indexI < ik; indexI++) {

			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
		for (indexJ; indexJ < jk; indexJ++) {
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
		for (indexI; indexI > i; indexI--) {
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;

		}
		for (indexJ; indexJ > j; indexJ--) {
			matrix[indexI][indexJ].status = Node::CellStatus::serviceData;
			matrix[indexI][indexJ] = flag;
		}
	}
	if (count < 3) { 
		addLevelingPatternFill(i - 1, j - 1, ik + 1, jk + 1, !flag, count);
	}
}
void Matrix::addMaskMatrix(std::vector < std::vector<Node>>& matrix,int mask,std::vector<std::pair<std::vector<std::vector<Node>>, size_t>>* result = nullptr) {
	std::vector < std::vector<Node>> m = matrix;
	size_t penaltyPoints = 0;
	addMaskAndCorrection(m,mask);
	addMask(m,mask);
	costPenaltyPoints(m,penaltyPoints);
	if (result != nullptr) {
		(*result)[mask] = std::make_pair(m, penaltyPoints);
	}
	else {
		matrix = m;
	}
}
void Matrix::addMask(std::vector < std::vector<Node>>& matrix,int mask) {
	for (int i = 0; i < sizeMatrix; i++) {
		for (int j = 0; j < sizeMatrix; j++) {
			if (matrix[i][j].status == Node::CellStatus::data) {
				bool value = matrix[i][j].cell;
				switch (mask) {
				case 0:
					if ((j + i) % 2 == 0) {

						matrix[i][j] = !value;
					}
					break;
				case 1:
					if (i % 2 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 2:
					if (j % 3 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 3:
					if ((j + i) % 3 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 4:
					if ((j / 3 + i / 2) % 2 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 5:
					if ((j * i) % 2 + (j * i) % 3 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 6:
					if (((j * i) % 2 + (j * i) % 3) % 2 == 0) {
						matrix[i][j] = !value;
					}
					break;
				case 7:
					if (((j * i) % 3 + (j + i) % 2) % 2 == 0) {
						matrix[i][j] = !value;
					}
					break;
				}
			}
		}
	}
}
void Matrix::addMaskAndCorrection(std::vector < std::vector<Node>>& matrix,int flag) {
	const auto& data = AllData::instance();
	std::array<uint8_t, 15> mask;
	if (flag != -1) {

		mask = AllData::getCodeMaskAndLevelCorr()[correctionLevel][flag];
		//{1,0,1,0,0,0,1,0,0,1,0,0,1,0,1}
	}
	int index = 0;
	//first data
	int i = 8;
	int jk = 0;

	for (jk = 0; jk < 8; jk++) {

		if (jk != 6) {
			matrix[i][jk] = (flag != -1) ? mask[index] : 1;
			matrix[i][jk].status = Node::CellStatus::serviceData;
			index++;
		}

	}
	for (int ik = i; ik >= 0; ik--) {
		if (ik != 6) {
			matrix[ik][jk] = (flag != -1) ? mask[index] : 1;
			matrix[ik][jk].status = Node::CellStatus::serviceData;
			index++;
		}
	}

	index = 0;
	//second data
	i = sizeMatrix-1;
	for (jk = 0; jk < 7; jk++) {
			matrix[i - jk][8] = (flag != -1) ? mask[index] : 1;
			matrix[i - jk][8].status = Node::CellStatus::serviceData;
			index++;
	}
	matrix[i - jk][8] = 1;
	matrix[i - jk][8].status = Node::CellStatus::serviceData;



	int x = sizeMatrix - 8;
	for (int xk = 0; xk < 8; xk++) {
		matrix[8][x + xk] = (flag != -1) ? mask[index] : 1;
		matrix[8][x + xk].status = Node::CellStatus::serviceData;
		index++;
	}
}
void Matrix::addPadding() {
	std::vector<std::vector<Node>> m(sizeMatrix + 4, std::vector<Node>(sizeMatrix + 4));
	for (int i = 2; i < sizeMatrix + 2; i++) {
		for (int j = 2; j < sizeMatrix + 2; j++) {
			m[i][j] = matrix[i - 2][j - 2].cell;
		}
	}
	matrix = m;
}
void Matrix::seekBestMask() {
	std::vector <std::thread> threads;
	std::vector<std::pair<std::vector<std::vector<Node>>, size_t>>* result = new std::vector<std::pair<std::vector<std::vector<Node>>, size_t>>;
	result->resize(8);
	for (int i = 0; i < 8; i++) {
		threads.push_back(std::thread(&Matrix::addMaskMatrix,this, std::ref(matrix), i, result));
	}
	for (auto& i : threads) {
		i.join();
	}
	size_t bestMaskScore = 1500;
	size_t indexBestMask = 0;
	for (int i = 0; i < result->size(); i++) {
		if ((*result)[i].second < bestMaskScore) {
			indexBestMask = i;
			bestMaskScore = (*result)[i].second;
		}
	}
	matrix = (*result)[indexBestMask].first;
}
void Matrix::step(int& i, int& j, bool& up, bool& a) {
	if (i == sizeMatrix - 1 && !up && !a) {
		j--;
		up = true;
		a = true;
		return;
	}
	else if (i == 0 && up && !a) {

		j--;
		up = false;
		a = true;
		return;
	}

	if (!up) {
		if (a) {
			j--;
			a = !a;
		}
		else {
			i++;
			j++;
			a = !a;
		}
	}
	else {
		if (a) {
			a = false;
			j--;
		}
		else if (!a) {
			j++;
			i--;
			a = true;
		}
	}
	if (j == 6) {
		j--;
	}
}
void Matrix::addData() {
	bool up = true;
	bool a = true;
	int indexData = 0;
	int i = sizeMatrix - 1;
	int j = sizeMatrix - 1;
	while (i >= 0 && j >= 0) {
		if (matrix[i][j].status == Node::CellStatus::none)
		{
			
			matrix[i][j] = (indexData < data.size()) ? data[indexData]: 0;
			matrix[i][j].status = Node::CellStatus::data;

			indexData++;
			step(i, j, up, a);

		}
		else {
			step(i, j, up, a);
			continue;
		}

	}
}

void Matrix::costPenaltyPoints(const std::vector < std::vector<Node>>& matrix,size_t& penaltyPoints) const{
	size_t whiteCount = 0;
	size_t blackCount = 0;


	//first rule	


	bool lastBitVertical = true;
	bool lastBitHorizontal = true;

	size_t sizeSequenceVertical = 0;
	size_t sizeSequenceHorizontal = 0;


	for (int i = 0; i < sizeMatrix; i++) {
		lastBitHorizontal = matrix[i][0].cell;
		lastBitVertical = matrix[0][i].cell;
		sizeSequenceVertical = 0;
		sizeSequenceHorizontal = 0;
		for (int j = 0; j < sizeMatrix; j++) {
			if (!matrix[i][j].cell) {
				blackCount++;
			}

			bool valueHorizontal = matrix[i][j].cell;
			bool valueVertical = matrix[j][i].cell;
			if (lastBitHorizontal == valueHorizontal) {
				sizeSequenceHorizontal++;
			}
			else {
				lastBitHorizontal = valueHorizontal;
				if (sizeSequenceHorizontal >= 5) {
					penaltyPoints += sizeSequenceHorizontal - 2;
				}
				sizeSequenceHorizontal = 1;

			}
			if (lastBitVertical == valueVertical) {
				sizeSequenceVertical++;
			}
			else {
				lastBitVertical = valueVertical;
				if (sizeSequenceVertical >= 5) {
					penaltyPoints += sizeSequenceVertical - 2;
				}
				sizeSequenceVertical = 1;

			}
		}
		if (sizeSequenceHorizontal >= 5) {
			penaltyPoints += sizeSequenceHorizontal - 2;
		}
		sizeSequenceHorizontal = 0;
		if (sizeSequenceVertical >= 5) {
			penaltyPoints += sizeSequenceVertical - 2;
		}
		sizeSequenceVertical = 0;

	}


	//second rule

	for (int i = 0; i < sizeMatrix - 1; i++) {
		for (int j = 0; j < sizeMatrix - 1; j++) {
			bool value = matrix[i][j].cell;
			if (matrix[i + 1][j].cell == value && matrix[i + 1][j + 1].cell == value && matrix[i][j + 1].cell == value) {
				penaltyPoints += 3;

			}

		}
	}



	//third rule
	for (int i = 0; i < sizeMatrix; i++) {
		for (int j = 0; j <= sizeMatrix - 7; j++) {
			if (matrix[i][j].cell && !matrix[i][j + 1].cell &&
				matrix[i][j + 2].cell && matrix[i][j + 3].cell &&
				matrix[i][j + 4].cell && !matrix[i][j + 5].cell
				&& matrix[i][j + 6].cell) {
				bool leftOK = false;
				bool rightOK = false;
				if (j > 3) {
					leftOK = (!matrix[i][j - 4].cell && !matrix[i][j - 3].cell && !matrix[i][j - 2].cell && !matrix[i][j - 1].cell);
				}
				if (j + 10 < sizeMatrix) {
					rightOK = (!matrix[i][j + 7].cell && !matrix[i][j + 8].cell &&
						!matrix[i][j + 9].cell && !matrix[i][j + 10].cell);
				}
				if (leftOK || rightOK) {
					penaltyPoints += 40;

				}

			}
		}
	}
	for (int i = 0; i <= sizeMatrix - 7; i++) {
		for (int j = 0; j < sizeMatrix; j++) {
			if (matrix[i][j].cell && !matrix[i + 1][j].cell &&
				matrix[i + 2][j].cell && matrix[i + 3][j].cell &&
				matrix[i + 4][j].cell && !matrix[i + 5][j].cell
				&& matrix[i + 6][j].cell) {
				bool leftOK = false;
				bool rightOK = false;
				if (i > 3) {
					leftOK = (!matrix[i - 4][j].cell && !matrix[i - 3][j].cell && !matrix[i - 2][j].cell && !matrix[i - 1][j].cell);
				}
				if (i + 10 < sizeMatrix) {
					rightOK = (!matrix[i + 7][j].cell && !matrix[i + 8][j].cell && !matrix[i + 9][j].cell && !matrix[i + 10][j].cell);
				}
				if (leftOK || rightOK) {
					penaltyPoints += 40;

				}

			}
		}
	}


	//fourth rule
	double persentage = blackCount / (21 * 21);
	persentage *= 100;
	persentage -= 50;
	penaltyPoints += std::abs(static_cast<int>(persentage)) * 2;
}
void print(const std::vector<std::vector<Node>>& matrix) noexcept
{
	for (const auto& vec : matrix) {
		for (const auto& bytes : vec) {
			std::cout << static_cast<bool>(bytes.cell) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
void Matrix::createMatrix() {
	searchPatternFill(3, 3, 0, 0);

	searchPatternFill(3, sizeMatrix - 4, 0, 0);
	searchPatternFill(sizeMatrix - 4, 3, 0, 0);

	syncBands(8, 6);

	addMaskAndCorrection(matrix, -1);


	codeVersionFill();

	addLevelingPattern();




	addData();

	addMaskMatrix(matrix,2, nullptr);


	addPadding();
	print(matrix);
	std::cout << qrVersion<<std::endl;

}