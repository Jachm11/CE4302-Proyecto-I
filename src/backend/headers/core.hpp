#ifndef CORE_H // Include guards to prevent multiple inclusions
#define CORE_H


#include "instruction.hpp"
#include "pe.hpp"
#include <queue>

class Core : public PE {
    public:
        Core(int id, std::queue<instruction>& instructions);

        void run();

};
#endif // CORE_H
