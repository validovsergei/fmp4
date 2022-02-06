#ifndef BOX_H
#define BOX_H

#include <string>
#include <fstream>
#include <vector>

namespace {
    template <unsigned int dimensionFieldType>
    std::string byteToString(uint8_t bytes[dimensionFieldType]) {
        std::string str;
        for(auto i = 0; i != dimensionFieldType; ++i) {
            str += bytes[i];
        }
    return (str);
    }

    template <unsigned int dimensionFieldSize>
    unsigned int byteToUnsignInt(uint8_t bytes[dimensionFieldSize]){
        unsigned int size = 0;
        for(auto i = 0; i != dimensionFieldSize; ++i) {
            size = (size << 8) + bytes[i];
        }
        return (size);
    }

    uint8_t getNecessaryBits (std::vector<bool> necessaryBits, uint8_t byte){
        int necBit = 0;
        int multipleTwo = 1;
        for(auto i = necessaryBits.size(); i != -1; --i){
            if(necessaryBits[i]){
                necBit += multipleTwo;
            }
            multipleTwo *= 2;
        }
        return (byte & necBit);
    }

}

template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
class Box {
    unsigned int _size;
    std::string _type;
    unsigned int _boxStartPosition;
    

    public:
        std::vector<Box*> _innerBoxes;
        unsigned int getSize(){return _size;}
        std::string getType(){return _type;}
        unsigned int getPosStart(){return _boxStartPosition;}
        Box(uint8_t size[dimensionFieldSize], uint8_t type[dimensionFieldType], unsigned int boxStartPos);
        Box(Box& newBox);
        Box& operator=(Box&& newBox);
        virtual ~Box();
        void getInnerBoxes(std::ifstream& file);
};

template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
Box<dimensionFieldSize, dimensionFieldType>::Box(uint8_t size[dimensionFieldSize], uint8_t type[dimensionFieldType], unsigned int boxStartPos){
    _size = byteToUnsignInt<dimensionFieldSize>(size);
    _type = byteToString<dimensionFieldType>(type);
    _boxStartPosition = boxStartPos;
}

template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
void Box<dimensionFieldSize, dimensionFieldType>::getInnerBoxes(std::ifstream& file){
    unsigned int newStartPosition = _boxStartPosition + dimensionFieldSize + dimensionFieldType;
    unsigned int sumSizesInnerBoxes = dimensionFieldSize + dimensionFieldType;
    while(sumSizesInnerBoxes < _size) {
        file.seekg(newStartPosition);
        uint8_t size[dimensionFieldSize];
        uint8_t type[dimensionFieldType];
        file.read((char*)size, dimensionFieldSize);
        file.read((char*)type, dimensionFieldType);
        _innerBoxes.push_back(new Box<dimensionFieldSize, dimensionFieldType>(size, type, newStartPosition));
        newStartPosition += _innerBoxes[_innerBoxes.size() - 1]->getSize();
        sumSizesInnerBoxes += _innerBoxes[_innerBoxes.size() - 1]->getSize();
    }
}


template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
Box<dimensionFieldSize, dimensionFieldType>::~Box(){
    for(auto i: _innerBoxes){
        if(i != nullptr){
            delete i;
        }
    }
}


template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
Box<dimensionFieldSize, dimensionFieldType>::Box(Box& newBox){
    _size = newBox._size;
    _type = newBox._type;
    _boxStartPosition = newBox._boxStartPosition;
    for(auto i: newBox._innerBoxes){
        _innerBoxes.push_back(i);
    }
}


template <unsigned int dimensionFieldSize, unsigned int dimensionFieldType>
Box<dimensionFieldSize, dimensionFieldType>& Box<dimensionFieldSize, dimensionFieldType>::operator=(Box&& newBox){
    _size = std::move(newBox._size);
    _type = std::move(newBox._type);
    _boxStartPosition = std::move(newBox._boxStartPosition);
    _innerBoxes = std::move(newBox._innerBoxes);
    
    return (*this);
}


#endif