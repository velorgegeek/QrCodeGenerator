#pragma once
#include <string>
#include <iostream>
#include <unordered_map> 
#include <iomanip>
#include "AllData.h"
#include "Matrix.h"
#include "Printer.h"
#include <memory>

struct ByteStream {
	std::vector<uint8_t> bytes;
	uint8_t currentByte = 0;
	uint8_t bitsFilled = 0;
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
	enum class CorrectionLevel { L, M, Q, H};
	enum class CodingStatus { number, letters, byte, kandzi };



	void createQR(const std::string& str, const QrCode::CodingStatus& status, const QrCode::CorrectionLevel& correction);
private:

	IPrint* printer = new PrintImage();

	//struct
	Matrix matrix;
	ByteStream byteStream;

	//enum
	CorrectionLevel corLevel;
	CodingStatus codeStatus;
	//
	uint8_t QrVersion;
	


	int seekBestVersion(const size_t& byteSize);
	int seekMaxCountData(const int& version);
	std::vector<uint8_t> createQRCode(const std::string& str, const QrCode::CodingStatus& status, const QrCode::CorrectionLevel& correction);
	std::vector<uint8_t> unionBlocks(const std::vector<std::vector<uint8_t>>& corrBlock, const std::vector<std::vector<uint8_t>>& blocks);
	std::vector<uint8_t> toBinary(const std::vector<uint8_t>& vecUnionBlocks, std::vector<uint8_t>& buff);
	std::vector<uint8_t> createCorrectionBlock(const std::vector<uint8_t>& block, const int& numCorrSize);
	void fillBlocks(std::vector<std::vector<uint8_t>>& vec, const int& countBlock);
};