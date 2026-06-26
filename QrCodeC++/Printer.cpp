#include "Printer.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp> 
#include <iostream>
#include "Matrix.h"

void PrintASCII::print(const std::vector<std::vector<Node>>& matrix) {
	for (const auto& vec : matrix) {
		for (const auto& bytes : vec) {
			std::cout << (bytes.cell == 1) ? "██" : "  ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
void PrintImage::print(const std::vector<std::vector<Node>>& matrix) {
	int rows = static_cast<int>(matrix.size());
	int cols = static_cast<int>(matrix[0].size());

	int cellSize = 32;
	while (cellSize * rows > 800) {
		cellSize /= 2;
	}

	cv::Mat image(rows * cellSize, cols * cellSize, CV_8UC3, cv::Scalar(255, 255, 255));
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			if (matrix[i][j].cell == 1) {
				cv::Rect rect(j * cellSize, i * cellSize, cellSize, cellSize);
				image(rect).setTo(cv::Scalar(0, 0, 0));
			}
		}
	}
	cv::imshow("My Image", image);
	cv::waitKey(0);
}