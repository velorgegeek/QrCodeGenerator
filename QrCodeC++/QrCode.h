#pragma once
#include <string>
#include <iostream>
#include <unordered_map> 
#include <iomanip>
#include <unordered_map>
struct AllData {
public:
	std::vector<std::vector<int>> maxCountData;
	std::vector<std::vector<int>> blockCount;
	std::unordered_map<char, int> letterscode; 
	std::vector<std::vector<int>> countBiteCorrection;
	std::vector<std::vector<int>> versionCorrection;
	std::unordered_map<int, std::vector<int>> genPolynomials;
	std::vector<uint8_t> galoisField;
	std::vector<uint8_t> inverseGaloisField;
	AllData() { init(); }
	void init();
};

struct ByteStream {
	std::vector<uint8_t> bytes;
	uint8_t currentByte = 0;
	int bitsFilled = 0;
public:
	void flush() {
		if (bitsFilled > 0) {
			bytes.push_back(currentByte);
			currentByte = 0;
			bitsFilled = 0;
		}
	}

	ByteStream& operator=(const ByteStream& bytes) {
		if (&bytes != this) {
			this->bytes = bytes.bytes;
		}
		return *this;
	}

	friend std::ostream& operator << (std::ostream& os, const ByteStream& bytes)
	{
		for (auto i : bytes.bytes) {
			os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i) << std::dec<<" ";
		}
		return os;
	}
	size_t countBytes() const noexcept {
		return bytes.size();
	}
	size_t countBits() const noexcept {
		return bytes.size() * 8 + bitsFilled;
	}
	std::vector<uint8_t> getBytes() const noexcept {
		return bytes;
	};
	void writeBits(const uint32_t num, int bitCount);
};

class QrCode {
public:
	enum class CorrectionLevel { L, M, Q, H };
	enum class CodingStatus { number, letters, byte, kandzi };


	void createQRCode(const std::string& str, const QrCode::CodingStatus& status, const QrCode::CorrectionLevel correction);

private:
	//struct
	AllData data;
	ByteStream byteStream;

	//enum
	CorrectionLevel corLevel;
	CodingStatus codeStatus;
	//
	size_t QrVersion;
	
	template<typename Vec>
	void print(Vec vector);
	int seekBestVersion(const size_t& byteSize);
	int seekMaxCountData(const int& version);
	std::vector<uint8_t> createCorrectionBlock(const std::vector<uint8_t>& block, const std::vector<int>& polynomials, const int& numCorrSize);
	void fillBlocks(std::vector<std::vector<uint8_t>>& vec, const int& countBlock);
};