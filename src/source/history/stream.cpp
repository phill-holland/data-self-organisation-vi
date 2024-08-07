#include "history/stream.h"
#include <fstream>
#include <sstream>

bool organisation::history::stream::get(value &destination, int index)
{
    if((index < 0)||(index >= data.size())) return false;

    return false;
}
            
bool organisation::history::stream::save(std::string filename)
{
    std::fstream output(filename, std::fstream::out | std::fstream::binary);

    if(output.is_open())
    {
        int previous = 1;
        for(auto &it: data)
        {
            //if(!it.stationary)
            //{
                std::string data;
                if(it.sequence != previous) data += "\r\n";
                data += it.serialise();                
                output.write(data.c_str(), data.size());
                previous = it.sequence;
            //}
        }
    }

    output.close();

    return true;
}