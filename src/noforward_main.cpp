#include "Headers/Processor.hpp"

int main(int argc, char* argv[]) {
    const string Filename = argv[1];
    printf("filename: %s\n", Filename.c_str()); // debug
    int cyclecount = stoi(argv[2]); // debug
    // is_forwarded = false;
    Processor processor(false);
    
    processor.loadProgram(Filename.c_str());
    processor.run(cyclecount);
    outputStageandCycles(Filename);

    return 0;
}