#include "history/value.h"
#include <iostream>
#include <sstream>
#include <functional>

std::string organisation::history::value::serialise()
{
    std::string result("H ");

    result += std::to_string((stationary == true) ? 1 : 0) + " Pos=";
    result += position.serialise() + " Data=";
    result += data.serialise() + " Nxt=";
    result += next.serialise() + " Col=";
    result += collision.serialise() + std::to_string(colType) + " Mov,Pat=";
    result += std::to_string(movementIdx) + "," + std::to_string(movementPatternIdx) + " Seq=";
    result += std::to_string(sequence) + " Cli=";
    result += std::to_string(client) + " E=";
    result += std::to_string(epoch);
    result += "\n";

    return result;
}