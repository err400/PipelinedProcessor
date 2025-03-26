#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <cstdint>

class Memory{
    private:
    std::array<uint32_t, 1024> dataMemory; // 4KB memory

    public:
    Memory();
    uint32_t readMemory(uint32_t address);
    void writeMemory(uint32_t address, uint32_t value);
};

#endif