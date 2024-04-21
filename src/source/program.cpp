#include "program.h"
#include "position.h"
#include "genetic/templates/genetic.h"
#include "genetic/templates/serialiser.h"
#include "general.h"
#include <stack>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <fstream>
#include <sstream>
#include <functional>

std::mt19937_64 organisation::program::generator(std::random_device{}());

void organisation::program::reset(parameters &settings)
{
    init = false; cleanup();

    _width = settings.width;
    _height = settings.height;
    _depth = settings.depth;

    length = _width * _height * _depth;

    clear();

    init = true;
}

void organisation::program::clear()
{    
    caches.clear();
    collisions.clear();
    insert.clear();
    links.clear();
}

bool organisation::program::empty()
{
    templates::genetic *genes[] = 
    { 
        &collisions,
        &insert,
        &links
    }; 

    const int components = sizeof(genes) / sizeof(templates::genetic*);
    for(int i = 0; i < components; ++i)
    {
        if(genes[i]->empty())
        { 
            std::cout << "empty " << i << "\r\n";
            return true;
        }
    }

    return false;
}

void organisation::program::generate(data &source, inputs::input &epochs)
{
    clear();

    templates::genetic *genes[] = 
    { 
        &caches,
        &collisions,
        &insert,
        &links
    }; 

    const int components = sizeof(genes) / sizeof(templates::genetic*);
    for(int i = 0; i < components; ++i)
{
        genes[i]->generate(source, epochs);
    }    
}

bool organisation::program::mutate(data &source, inputs::input &epochs)
{    
    templates::genetic *genes[] = 
    { 
        &caches,
        &collisions,
        &insert,
        &links
    }; 

    const int components = sizeof(genes) / sizeof(templates::genetic*);

    const int idx = (std::uniform_int_distribution<int>{0, components - 1})(generator);

    return genes[idx]->mutate(source, epochs);    
}

std::string organisation::program::run(std::string input, data &source, int max)
{
    return std::string("");    
}

bool organisation::program::validate(data &source)
{
    templates::genetic *genes[] = 
    { 
        &caches,
        &collisions,
        &insert,
        &links
    }; 

    for(auto &it: genes)
    {
        if(!it->validate(source)) return false;
    }

    return true;
}

void organisation::program::copy(const program &source)
{    
    _width = source._width;
    _height = source._height;
    _depth = source._depth;
    length = source.length;

    caches.copy(source.caches);
    collisions.copy(source.collisions);
    insert.copy(source.insert);    
    links.copy(source.links);
}

bool organisation::program::equals(const program &source)
{
    if(!caches.equals(source.caches)) 
        return false;    
    if(!collisions.equals(source.collisions)) 
        return false;
    if(!insert.equals(source.insert)) 
        return false;
    if(!links.equals(source.links))
        return false;
    
    return true;
}

void organisation::program::cross(program &a, program &b)
{
    clear();

    templates::genetic *ag[] = 
    { 
        &a.caches,        
        &a.collisions,
        &a.insert,
        &a.links
    }; 

    templates::genetic *bg[] = 
    { 
        &b.caches,        
        &b.collisions,
        &b.insert,
        &b.links
    }; 

    templates::genetic *dest[] = 
    { 
        &caches,
        &collisions,
        &insert,
        &links
    }; 

    const int components = sizeof(dest) / sizeof(templates::genetic*);
    for(int i = 0; i < components; ++i)
    {        
        int length1 = ag[i]->size();    
        int sa = 0, ea = 0;

        if(length1 > 0)
        {
            do
            {
                sa = (std::uniform_int_distribution<int>{ 0, length1 })(generator);
                ea = (std::uniform_int_distribution<int>{ 0, length1 })(generator);
            } while(sa == ea);
        }
        
        if(ea < sa) 
        {
            int temp = ea;
            ea = sa;
            sa = temp;
        }

        // ***

        int length2 = bg[i]->size();
        int sb = 0, eb = 0;

        if(length2 > 0)
        {
            do
            {
                sb = (std::uniform_int_distribution<int>{ 0, length2 })(generator);
                eb = (std::uniform_int_distribution<int>{ 0, length2 })(generator);
            } while(sb == eb);
        }
        
        if(eb < sb) 
        {
            int temp = eb;
            eb = sb;
            sb = temp;
        }
        
        // ***

        dest[i]->append(ag[i], 0, sa); 
        dest[i]->append(bg[i], sb, eb); 
        dest[i]->append(ag[i], ea, ag[i]->size()); 
    }
}

std::string organisation::program::serialise()
{
    std::vector<templates::serialiser*> sources = 
    { 
        &caches,
        &collisions,
        &insert,
        &links
    }; 

    std::string result;

    for(auto &it: sources)
    {
        result += it->serialise();    
    }

    return result;
}

void organisation::program::deserialise(std::string source)
{
    std::stringstream ss(source);
    std::string value;

    caches.clear();
    collisions.clear();
    insert.clear();
    links.clear();

    while(std::getline(ss,value))
    {
        std::stringstream stream(value);
		std::string type;
	            
        if(stream >> type)
        {
            if(type == "D") caches.deserialise(value);
            else if(type == "C") collisions.deserialise(value);
            else if(type == "I") insert.deserialise(value);
            else if(type == "L") links.deserialise(value);
        }
    };
}

void organisation::program::save(std::string filename)
{
    std::fstream output(filename, std::fstream::out | std::fstream::binary);

    if(output.is_open())
    {
        std::string data = serialise();
        output.write(data.c_str(), data.size());
    }

    output.close();
}

bool organisation::program::load(std::string filename)
{
    std::ifstream source(filename);
    if(!source.is_open()) return false;
    
    caches.clear();
    collisions.clear();        
    insert.clear();

    for(std::string value; getline(source, value); )
    {
        std::stringstream stream(value);
        std::string type;
            
        if(stream >> type)
        {
            if(type == "D") caches.deserialise(value);                
            else if(type == "C") collisions.deserialise(value);
            else if(type == "I") insert.deserialise(value);
            else if(type == "L") links.deserialise(value);
        }
    }

    source.close();

    return true;
}
    
void organisation::program::makeNull()
{
    
}

void organisation::program::cleanup()
{

}