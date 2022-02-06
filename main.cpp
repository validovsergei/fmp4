#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include "Box.h"
#include "FuncReader.h"


int main()
{
    std::string path;
    std::cout << "Input path of segment:\n";
    std::cin >> path;
    std::ifstream file(path, std::ios::binary);
    if(!file)
    {
        std::cout << "File could not be open\n";
        return -1;
    }
    int moofStartPosition = transmissionToNameBlock<4, 4>(file, "moof");
    int mdatStartPosition = transmissionToNameBlock<4, 4>(file, "mdat");
    if(moofStartPosition == -1)
    {
        std::cout << "Moof block could not be found\n";
        return -1; 
    }
    if(mdatStartPosition == -1){
        std::cout << "Mdat block is not detected in file\n";
        return -1;
    }
    file.seekg(moofStartPosition);
    uint8_t sizeByte[4];
    uint8_t typeByte[4];
    file.read((char*)sizeByte, 4);
    file.read((char*)typeByte, 4);
    Box<4, 4> moof(sizeByte, typeByte, moofStartPosition);
    file.seekg(mdatStartPosition);
    file.read((char*)sizeByte, 4);
    file.read((char*)typeByte, 4);
    Box<4, 4> mdat(sizeByte, typeByte, moofStartPosition);
    moof.getInnerBoxes(file);
    int posTraf0InMoof = findBoxInBox(moof, "traf");
    if(posTraf0InMoof == -1){
        std::cout << "Traf block is not detected in moof block\n";
        return -1;
    }
    Box<4, 4> firstTraf(*moof._innerBoxes[posTraf0InMoof]);
    firstTraf.getInnerBoxes(file);
    int posTrun0InFirstTraf = findBoxInBox(firstTraf, "trun");
    if(posTrun0InFirstTraf == -1){
        std::cout << "Trun block is not detected in first traf block\n";
        return -1;
    }
    Box<4, 4> firstTrunInFirstTraf(*firstTraf._innerBoxes[posTrun0InFirstTraf]);
    u_int8_t dataOffsetBinary[4];
    file.seekg(firstTrunInFirstTraf.getPosStart() + 16);
    file.read((char*)dataOffsetBinary, 4);
    unsigned int dataoffset = byteToUnsignInt<4>(dataOffsetBinary);
    //std::cout << "dataoffset " << dataoffset << std::endl;
    int posStart = moofStartPosition + dataoffset;
    int posFinish = mdatStartPosition + mdat.getSize();
    int posTraf1InMoof = findBoxInBox(moof, "traf", 2);
    if(posTraf1InMoof != -1)
    {
        Box<4, 4> secondTraf(*(moof._innerBoxes[posTraf1InMoof]));
        int posTrun0InSecondTraf = findBoxInBox(secondTraf, "trun");
        if(posTrun0InFirstTraf == -1){
            Box<4, 4> firstTrunInSecondTraf(*secondTraf._innerBoxes[posTrun0InSecondTraf]);
            file.seekg(firstTrunInSecondTraf.getPosStart() + 16);
            u_int8_t dataOffsetBinaryFinish[4];
            file.read((char*)dataOffsetBinaryFinish, 4);
            posFinish = moofStartPosition + byteToUnsignInt<4>(dataOffsetBinaryFinish);
        }
        

    }
    file.seekg(posStart);
    auto i = ReadNalUnit(file);
    std::cout << "size NaL unit: " << i.first << " type nal unit " << i.second << std::endl;
    while(i.second != 5 && i.second != 1 && moofStartPosition + i.first + 4 < posFinish)
    {
        posStart += i.first + 4;
        file.seekg(posStart);
        i = ReadNalUnit(file);
        std::cout << "size NaL unit: " << i.first << " type nal unit " << i.second << std::endl;
    }
    std::cout << i.second << std::endl; 
    if(i.second == 5)
    {
        std::cout << "this fmp4 segment starts with idr frame\n";
        return 0;
    }
    if(i.second == 1)
    {
        std::cout << "this fmp4 segment starts with no idr frame\n";
        return 0;
    }
    std::cout << "frame not found\n";
    return -1;
}