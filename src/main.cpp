#include "Headers/Processor.hpp"

int main() {
    const string Filename = "inputfiles/stringcopy.txt";
    Processor processor;
    processor.loadProgram(Filename.c_str());
    processor.run();

    outputStageandCycles(Filename);

    return 0;
}