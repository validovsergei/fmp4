#ifndef FUNCREADER_H
#define FUNCREADER_H

#include "Box.h"

namespace{
    template<unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
    int transmissionToNameBlock(std::ifstream& file, std::string blockName){
        size_t pos = 0;
        file.seekg(pos);
        uint8_t sizeByte[dimensionFieldSize];
        uint8_t typeByte[dimensionFieldType];
        file.read((char*)sizeByte, dimensionFieldSize);
        file.read((char*)typeByte, dimensionFieldType);
        Box<dimensionFieldSize, dimensionFieldType> block(sizeByte, typeByte, 0);
        while(block.getType() != blockName && !file.eof()){
            pos += block.getSize();
            file.seekg(pos);
            file.read((char*)sizeByte, dimensionFieldSize);
            file.read((char*)typeByte, dimensionFieldType);
            block = Box<dimensionFieldSize, dimensionFieldType>(sizeByte,typeByte, pos);
        }
        if(block.getType() == blockName){
            return (block.getPosStart());
        }
        return (-1);
    }

    std::pair<unsigned int, std::string> ReadSizeType(std::ifstream& file)
    {
        u_int8_t sizeBinary[4];
        u_int8_t typeBinary[4];
        file.read((char*)sizeBinary, 4);
        file.read((char*)typeBinary, 4);
        unsigned int size = 0;
        std::string type;
        for(auto i = 0; i != 4; ++i)
        {
            size = (size << 8) + sizeBinary[i];
            type += typeBinary[i];
        }
        return std::make_pair(size, type);
    }


    std::pair<unsigned int, unsigned int> ReadNalUnit(std::ifstream& file)
    {
        u_int8_t sizeUnitBinary[4];
        u_int8_t NalUnitBinary[1];
        file.read((char*)sizeUnitBinary, 4);
        file.read((char*)NalUnitBinary, 1);
        unsigned int size = 0;
        u_int8_t lastFiveBitsNalUnitHeader = NalUnitBinary[0] & (0b00011111);
        unsigned int nalUnitType = lastFiveBitsNalUnitHeader;
        for(auto i = 0; i != 4; ++i)
        {
            size = (size << 8) + sizeUnitBinary[i];
        }
        return std::make_pair(size, nalUnitType);
    }


    signed int getDataOffset(std::ifstream& file)
    {
        u_int8_t sampleCountBinary[4];
        u_int8_t dataOffsetBinary[4];
        u_int8_t firstSampleFlagsBinary[4];
        file.read((char*)sampleCountBinary, 4);
        file.read((char*)sampleCountBinary, 4);
        file.read((char*)dataOffsetBinary, 4);
        file.read((char*)firstSampleFlagsBinary, 4);
        signed int dataOffset = 0;
        for(auto i = 0; i != 4; ++i)
        {
            dataOffset = (dataOffset << 8) + dataOffsetBinary[i];
        }
        return dataOffset;
    }


    std::pair<unsigned int, std::string> ReadMoofSizeType(std::ifstream& file, unsigned int pos)
    {
        static unsigned int _pos = 8;
        _pos += pos;
        file.seekg(_pos);
        return ReadSizeType(file);
    }

    template<unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
    int findBoxInBox(Box<dimensionFieldSize, dimensionFieldType>& searchBlock, std::string nameDesiredBlock, unsigned int countDesiredBlock = 1){
        int count = 0;
        int posInInnerBox = 0;
        for(auto i: searchBlock._innerBoxes){
            if(i->getType() == nameDesiredBlock){
                count += 1;
            }
            if(count == countDesiredBlock){
                return posInInnerBox;
            }
            posInInnerBox += 1;
        }
        return -1;
    }
}

#endif