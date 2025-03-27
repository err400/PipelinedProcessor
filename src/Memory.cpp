#include "Headers/Memory.hpp"

Memory::Memory(){
    // // Random number generator
    // std::random_device rd;   // Non-deterministic random number generator
    // std::mt19937 gen(rd());  // Mersenne Twister engine
    // std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX); // Range for uint32_t

    // // Fill memory with random numbers
    // for(auto& val : dataMemory) {
    //     val = dist(gen);
    // }
    std::fill(dataMemory.begin(), dataMemory.end(), 0);
}
uint32_t Memory::readMemory(uint32_t address){
    return dataMemory[address];
}
void Memory::writeMemory(uint32_t address, uint32_t value){
    dataMemory[address] = value;
    return;
}