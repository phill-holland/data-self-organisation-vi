#include "history/value.h"
#include <iostream>
#include <sstream>
#include <functional>

std::string organisation::history::value::serialise()
{
    std::string result("H=");
    std::string next_collisions_result, current_collisions_result;
    std::string link_result;

    for(auto &it:nextCollisions)
    {
        if(next_collisions_result.size() > 0) next_collisions_result += ",";
        next_collisions_result += "(" + std::to_string(std::get<0>(it)) + "," + std::to_string(std::get<1>(it)) + "," + std::to_string(std::get<2>(it)) + "," + std::to_string(std::get<3>(it)) + ")";
    }
    
    for(auto &it:currentCollisions)
    {
        if(current_collisions_result.size() > 0) current_collisions_result += ",";
        current_collisions_result += "(" + std::to_string(std::get<0>(it)) + "," + std::to_string(std::get<1>(it)) + "," + std::to_string(std::get<2>(it)) + "," + std::to_string(std::get<3>(it)) + ")";
    }

    for(auto &it:links)
    {
        link_result += "{" + std::to_string(it.first) + ":[";
        for(auto &jt:it.second)
        {
            link_result += "(" + std::to_string(std::get<0>(jt)) + "," + std::to_string(std::get<1>(jt)) + "," + std::to_string(std::get<2>(jt)) + "," + std::to_string(std::get<3>(jt)) + ")";
        }
        link_result += "]}";
    }

    result += std::to_string((stationary == true) ? 1 : 0) + " Pos=";
    result += position.serialise() + " Data=";
    result += data.serialise() + " Life=";
    result += std::to_string(lifetime) + " LpIter=";
    result += std::to_string(loop) + " Nxt=";
    result += next.serialise() + " Col=[" + next_collisions_result + "|";
    result += current_collisions_result + "] Mov,Pat=" + std::to_string(movementIdx) + "," + std::to_string(movementPatternIdx);
    result += " Lnk=" + link_result;
    result += " Seq=" + std::to_string(sequence) + " Cli=";
    result += std::to_string(client) + " E=";
    result += std::to_string(epoch);
    result += "\n";

    return result;
}