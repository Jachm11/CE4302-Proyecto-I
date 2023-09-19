#include <string>
#include <queue>
#include <iostream>
#include "../headers/pe.hpp"
#include "../headers/cache.hpp"
#include "../headers/instruction.hpp"


class Core : public PE 
{

    int id;

    Cache cache;

    std::queue<instruction> instructions;
    

    public:
        Core(int id, std::queue<instruction>& instructions, Cache& cache){
            this->id = id;
            this->instructions = instructions;
            this->cache = cache;
        }


        void run() override {

            // Here is when Core execution starts


            while(instructions.size() != 0){

                instruction instruction_to_execute = instructions.front();
                if(instruction_to_execute.opcode.compare("write")){
                    //cache.write();
                    std::cout << "write instruction from PE" << id <<"\n";
                }
                if(instruction_to_execute.opcode.compare("read")){
                    std::cout << "read instruction from PE" << id <<"\n";
                }
                if(instruction_to_execute.opcode.compare("inc")){
                    //cache.read();
                    //cache.write();
                    std::cout << "inc instruction from PE" << id <<"\n";
                }
                instructions.pop();
                    


            }


    }

};