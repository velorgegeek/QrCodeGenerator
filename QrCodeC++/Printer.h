#pragma once
#include <vector>
#include "Matrix.h"
class IPrint {
public:
	virtual void print(const std::vector<std::vector<Node>>& matrix) = 0;

};
class PrintASCII : public IPrint
{
public:
	void print(const std::vector<std::vector<Node>>& matrix) override;
};
class PrintImage : public IPrint {
public:
	void print(const std::vector<std::vector<Node>>& matrix) override;
};

