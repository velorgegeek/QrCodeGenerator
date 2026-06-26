#pragma once
#include <vector>
#include <array>
#include <unordered_map>
class AllData
{
public:	
	std::array<std::array<uint8_t,3>,3> maxCountData;
	std::array<std::array<uint8_t,40>,4> blockCount;
	std::unordered_map<char, uint8_t> letterscode;
	std::array<std::array<uint8_t, 40>, 4> countBiteCorrection;
	std::array<std::array<int,40>,4	> versionCorrection;
	std::unordered_map<int, std::vector<int>> genPolynomials;
	std::array<uint8_t,256> galoisField;
	std::array<uint8_t,256> inverseGaloisField;
	std::array<std::vector<int>, 40> aligmentPatterns;
	
	inline static const AllData& instance() {
		static const AllData data;
		return data;
	}
	static const std::array<std::array<uint8_t, 3>, 34>& getVersionMatrix();
	static const std::array<std::array<std::array<uint8_t, 15>, 8>, 4>& getCodeMaskAndLevelCorr();

	AllData(const AllData&) = delete;
	AllData& operator=(const AllData&) = delete;
	AllData(AllData&&) = delete;
	AllData& operator=(AllData&&) = delete;

private:
	AllData() { init(); }
	void init();
};

