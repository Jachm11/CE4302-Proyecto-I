#include "core.cpp"
#include "../headers/instruction.hpp"
#include <queue>
#include <thread>


int main(){


    instruction instruction1;
    instruction1.opcode = "write";

    instruction instruction2;
    instruction1.opcode = "read";

    instruction instruction3;
    instruction1.opcode = "inc";

    instruction instruction4;
    instruction1.opcode = "write";
    
    
    std::queue<instruction> instructions; 
    instructions.push(instruction3);
    instructions.push(instruction2);
    instructions.push(instruction1);
    instructions.push(instruction4);

    std::queue<instruction> instructions1; 
    instructions1.push(instruction2);
    instructions1.push(instruction4);
    instructions1.push(instruction3);
    instructions1.push(instruction1);


    Cache cache; 
    Cache cache1;

    Core core(0,instructions, cache);
    Core core1(1, instructions1, cache1);


    std::thread thread1([&core]{
        core.run();
    });

    std::thread thread2([&core1]{
        core1.run();
    });


    thread1.join();
    thread2.join();









}