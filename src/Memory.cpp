#include "Headers/Memory.hpp"

Memory::Memory(){
    std::fill(dataMemory.begin(), dataMemory.end(), 0);
}
uint32_t Memory::readMemory(uint32_t address){
    return dataMemory[address];
}
void Memory::writeMemory(uint32_t address, uint32_t value){
    dataMemory[address] = value;
    return;
}