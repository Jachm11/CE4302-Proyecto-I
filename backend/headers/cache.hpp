#ifndef CACHE_H // Include guards to prevent multiple inclusions
#define CACHE_H

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>
#include <string>

struct cache_line {
    int tag;
    int value;
    std::string state;


};

class Cache  {

    public:

    std::vector<cache_line> cache_lines;

    Cache();

    void write();

    void read(int address);


    private:

    bool updated;

    bool hasValue(int address)
    {
        for(auto &cache_line: this->cache_lines){
            if(cache_line.tag == address){
                return true;
            }
        }
        return false;
    }

    cache_line getValue(int address){
        for(auto &cache_line: this->cache_lines){
            if(cache_line.tag == address){
                return cache_line;
            }
        }
        throw std::runtime_error("Address not found");
    }

};
#endif // CACHE_H
