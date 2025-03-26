#include "Headers/Processor.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <cycle_count>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    // int cycleCount = std::stoi(argv[2]);  // debug
    

    // Create a forwarding processor
    Processor processor(true);  // true indicates forwarding
    
    // Load program
    processor.loadProgram(inputFile.c_str());
    
    // Run the processor
    processor.run();

    return 0;
}