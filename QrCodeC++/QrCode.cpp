#include "QrCode.h"
#include <vector>

#include <iostream>

void ByteStream::writeBits(const uint32_t num, int bitCount) {
	for (int i = bitCount - 1; i >= 0; i--) {
		bool bit = (num >> i) & 1;
		if (bit) {
			currentByte |= 1 << (7 - bitsFilled);
		}
		++bitsFilled;
		if (bitsFilled == 8) {
			bytes.push_back(currentByte);
			currentByte = 0;
			bitsFilled = 0;
		}
	}
}
void QrCode::createQR(const std::string& str, const QrCode::CodingStatus& status, const QrCode::CorrectionLevel& correction) {
	matrix.init(QrVersion,static_cast<uint8_t>(correction),createQRCode(str, status, correction));
	printer->print(matrix.matrix);
}
std::vector<uint8_t> QrCode::createQRCode(const std::string& str, const QrCode::CodingStatus& status, const QrCode::CorrectionLevel& correction) {
	corLevel = correction;
	const auto& data = AllData::instance();
	codeStatus = status;
	switch (codeStatus) {
	case QrCode::CodingStatus::letters:
		std::vector<std::string> AllText;
		std::string text;
		//01  23  4
		//HE, LL, O
		ByteStream localbyte;

		bool newText = 0;
		for (int i = 0; i < str.size(); i++) {
			newText = 1;
			text += str[i];
			if ((i + 1) % 2 == 0) {
				newText = 0;
				AllText.push_back(text);
				text.clear();
			}
		}
		if (newText == 1) {
			AllText.push_back(text);
			text.clear();
		}

		for (auto& i : AllText) {
			int num = 0;
			for (int b = 0; b < i.size(); b++) {
				int num2 = data.letterscode.at(i[b]);
				num += (b == 0 && i.size() != 1) ? num2 * 45 : num2;
			}
			localbyte.writeBits(num, (i.size() == 1) ? 6 : 11);

		}
		int IndexVersion = seekBestVersion(4 + localbyte.countBits());
		int maxData = seekMaxCountData(IndexVersion + 1);
		QrVersion = seekBestVersion(4 + maxData + localbyte.countBits()) + 1;
		maxData = seekMaxCountData(QrVersion);
		byteStream.writeBits(2, 4);
		byteStream.writeBits(str.size(), maxData);

		for (auto& i : AllText) {
			int num = 0;
			for (int b = 0; b < i.size(); b++) {
				int num2 = data.letterscode.at(i[b]);
				num += (b == 0 && i.size() != 1) ? num2 * 45 : num2;
			}
			byteStream.writeBits(num, (i.size() == 1) ? 6 : 11);
		}
		byteStream.writeBits(0, 4);
	}

	int padBits = (8 - (byteStream.countBits() % 8)) % 8;

	byteStream.writeBits(0, padBits);
	byteStream.flush();

	std::cout << byteStream;

	int versionBytes = data.versionCorrection[static_cast<int>(corLevel)][QrVersion - 1];
	int countAddBytes = (versionBytes - byteStream.countBytes() * 8) / 8;

	for (int i = 0; i < countAddBytes; i++) {
		(i % 2 == 0) ? byteStream.writeBits(236, 8) : byteStream.writeBits(17, 8);
	}

	int countBlock = data.blockCount[static_cast<int>(corLevel)][QrVersion - 1];
	std::vector<std::vector<uint8_t>> blocks(countBlock);

	fillBlocks(blocks, countBlock);

	std::cout << "blocks" << std::endl;
	for (auto i : blocks) {
		for (auto j : i) {
		std::cout<<(int)j<<" ";
		}
		std::cout << std::endl;
	}
	int countBytesCorrection = data.countBiteCorrection[static_cast<int>(corLevel)][QrVersion - 1];
	std::vector<std::vector<uint8_t>> correctionBlocks;
	for (auto i : blocks) {
		auto corr = createCorrectionBlock(i, countBytesCorrection);
		correctionBlocks.push_back(corr);

	}
	std::cout << "corr block" << std::endl;
	for (auto i : correctionBlocks) {
		for (auto j : i) {
			std::cout << (int)j<<" ";
		}
		std::cout << std::endl;
	}
	std::vector<uint8_t> vecUnionBlocks = unionBlocks(correctionBlocks, blocks);
	std::cout << "union" << std::endl;
	for (auto i : vecUnionBlocks) {
			std::cout << (int)i << " ";
	}
	std::cout << std::endl;
	std::vector<uint8_t> buff;
	return toBinary(vecUnionBlocks, buff);
}
std::vector<uint8_t> QrCode::toBinary(const std::vector<uint8_t>& vecUnionBlocks,std::vector<uint8_t>& buff) {
	std::vector<uint8_t> reverseBuff;
	for (const auto& i : vecUnionBlocks) {
		uint8_t val = i;
		for(int k = 0 ; k < 8;k++){
			reverseBuff.emplace(reverseBuff.begin(), val % 2);
			val = val / 2;
		}
		for (const auto& num : reverseBuff) {
			buff.emplace_back(num);
		}
		//buff.emplace_back(2);
		reverseBuff.clear();
	}

	return buff;
}
std::vector<uint8_t> QrCode::unionBlocks(const std::vector<std::vector<uint8_t>>& corrBlock, const std::vector<std::vector<uint8_t>>& blocks){
std::vector<uint8_t> returnedBlocks;


	size_t maxDataLen = 0;
	for (const auto& b : blocks) {
		if (b.size() > maxDataLen) maxDataLen = b.size();
	}

	returnedBlocks.reserve(corrBlock.size() + blocks.size());



	for (size_t i = 0; i < maxDataLen; i++) {	
		for (size_t j = 0; j < blocks.size(); j++) {
			if (i < blocks[j].size()) {
				returnedBlocks.push_back(blocks[j][i]);
			}
		}

	}


	size_t maxCorrLen = 0;
	for (const auto& b : corrBlock) {
		if (b.size() > maxCorrLen) maxCorrLen = b.size();
	}

	for (int i = 0; i < maxCorrLen; i++) {
		for (size_t j = 0; j < corrBlock.size(); j++) {
			if (i < corrBlock[j].size()) {
				returnedBlocks.push_back(corrBlock[j][i]);
			}
		}				
	}
	return returnedBlocks;
}
std::vector<uint8_t> QrCode::createCorrectionBlock(const std::vector<uint8_t>& block, const int& numCorrSize) {

   	std::vector<uint8_t> workVec = block;
	const auto& data = AllData::instance();
	auto polynomials = data.genPolynomials.at(numCorrSize);
	
	workVec.resize(block.size() + numCorrSize, 0);

	for (int i = 0; i < block.size(); i++) {
		uint8_t A = workVec[0];
		workVec.erase(workVec.begin());
		workVec.push_back(0);
		
		if (A != 0) {
			uint8_t B = data.inverseGaloisField[A];
			for (int j = 0; j < numCorrSize; j++) {
				uint8_t V = (polynomials[j] + B) % 255;
				workVec[j] ^= data.galoisField[V];
			}
		}
	}
	return std::vector<uint8_t>(workVec.begin(), workVec.begin() + numCorrSize);

}
void QrCode::fillBlocks(std::vector<std::vector<uint8_t>>& vec,const int& countBlock) {
	int infoInByte = byteStream.countBytes() / countBlock;
	int remains = byteStream.countBytes() % countBlock;
	int indexData = 0;
	const auto vecBytes = byteStream.getBytes();
	for (int i = 0; i < countBlock; i++) {
		//5 - 2  = 4,3 index for +1 byte
		int size = infoInByte + (i >= (countBlock - remains) ? 1 : 0);
		vec[i].resize(size);

		for (int k = 0; k < size; k++, indexData++) {
			vec[i][k] = vecBytes[indexData];
		}

	}
}
int QrCode::seekMaxCountData(const int& version) {
	const auto& data = AllData::instance();
	const auto& vecMaxData = data.maxCountData[static_cast<int>(codeStatus)];
	if (version < 10) {
		 return vecMaxData[0];
	}
	else if (version >= 10 && version <= 26) {
		return vecMaxData[1];
	}
	return vecMaxData[2];
}
//return index
int  QrCode::seekBestVersion(const size_t& byteSize) {	
	const auto& data = AllData::instance();
	const auto vectVersion = data.versionCorrection[static_cast<int>(corLevel)];
	int left = 0;
	int right = vectVersion.size() - 1;
	while (left <= right) {
		int mid = (left + right) / 2;
		if (byteSize < vectVersion[mid]) {
			if (mid > 0) {
				if (byteSize > vectVersion[mid - 1]) {
					return mid;
				}
				else if (vectVersion[mid-1] < byteSize) {
					left = mid + 1;
				}
				else {
					right = mid - 1;
				}

			}
			else {
				return mid;
			}
		}
		else if (vectVersion[mid] < byteSize) {
			left = mid + 1;
		}

	}
	return 0;

}