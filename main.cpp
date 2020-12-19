#include <signal.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

#include "ansi.hpp"

#define TIMED_ITERATION 10000000
bool running = true;

void sigIntHandler(int s){
    running = false;
}

//an implementation of the Java Random() RNG
class JavaRandom {
    public:
    typedef int result_type;
    uint64_t seed=1;
    JavaRandom(uint64_t s){
        seed=s;
    }
    static constexpr int min(){
        return 1;
    }
    static constexpr int max(){
        return 2147483647;
    }
    inline result_type next(int bits){
        seed=((seed)*0x5DEECE66DL+0xBL)&((1L << 48) - 1);
        return seed >> (48 - bits);
    }
    inline result_type nextInt(){
        return next(32);
    }

    result_type operator()() {
        return nextInt();
    }
};

int main(int argc, char * argv[])
{
    srand(time(0));
    bool timer=false;

    if(argc>=2 && string(argv[1]).compare("-t")==0){
        timer=true;
    }
    signal(SIGINT, sigIntHandler);

    std::ofstream file;
    file.open("data.csv", std::ios::out);

    std::cout << ansi::hide_cursor() << ansi::cursor_pos(1, 1) << ansi::erase_in_display();

    std::string iterationsMsg = "Total Iterations:   ";
    std::string pearlAvgMsg   = "Ender Pearl Avg.:   ";
    std::string rodsAvgMsg    = "Blaze Rod Avg.:     ";
    std::string pearlMaxMsg   = "Ender Pearl Max:    ";
    std::string rodsMaxMsg    = "Blaze Rod Max:      ";
    std::string pearlMsg      = "Ender Pearls:       ";
    std::string rodsMsg       = "Blaze Rods:         ";

    std::cout << "+------------------------+" << std::endl << // Line 1
                "|          Total         |" << std::endl <<  // Line 2
                "+------------------------+" << std::endl <<  // Line 3
                iterationsMsg << std::endl <<                 // Line 4
                pearlAvgMsg << std::endl <<                   // Line 5
                rodsAvgMsg << std::endl <<                    // Line 6
                pearlMaxMsg << std::endl <<                   // Line 7
                rodsMaxMsg << std::endl <<                    // Line 8
                "Dream Ender Pearls: 42" << std::endl <<      // Line 9
                "Dream Blaze Rods:   211" << std::endl <<     // Line 10
                "+------------------------+" << std::endl <<  // Line 11
                "|     This Iteration     |" << std::endl <<  // Line 12
                "+------------------------+" << std::endl <<  // Line 13
                pearlMsg << std::endl <<                      // Line 14
                rodsMsg << std::endl <<                       // Line 15
                std::endl;

    int64_t iterations = 0;
    int64_t maxPearls = 0;
    int64_t maxRods = 0;
    int64_t pearlSum = 0;
    int64_t rodsSum = 0;
    double pearlAvg = 0.0;
    double rodsAvg = 0.0;

    int64_t pearlInstances[263];
    int64_t rodInstances[306];
    for (int i = 0; i < 263; i++) {
        pearlInstances[i] = 0;
    }
    for (int i = 0; i < 306; i++) {
        rodInstances[i] = 0;
    }

    JavaRandom j(123);
    JavaRandom j2(123);
    std::binomial_distribution<int> distribution(262,20.0/423);
    std::binomial_distribution<int> distribution2(305,0.5);
    auto start = high_resolution_clock::now();
    while(running) {
        iterations++;
        if(timer==true && iterations==TIMED_ITERATION){
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            int m=duration.count();
            std::cout.clear();
            system("clear");
            std::cout << "Took " << m/1000000.0<< " to perform "<< TIMED_ITERATION<<" iterations"<< std::endl;
            return 0;
        }
        int64_t numPearls = distribution(j);

        pearlInstances[numPearls]++;
        pearlSum += numPearls;
        maxPearls = std::max(maxPearls, numPearls);

        int64_t numRods = distribution2(j2);
        rodInstances[numRods]++;
        rodsSum += numRods;
        maxRods = std::max(maxRods, numRods);
        if(iterations%100000==0){
            // Iterations Message
            std::cout << ansi::cursor_pos(4, iterationsMsg.length() + 1) << ansi::erase_in_line() << iterations << std::endl;
            // Pearl Message
            std::cout << ansi::cursor_pos(14, pearlMsg.length() + 1) << ansi::erase_in_line() << numPearls << std::endl;
            // Rods Message
            std::cout << ansi::cursor_pos(15, rodsMsg.length() + 1) << ansi::erase_in_line() << numRods << std::endl;

            // Pearl Avg Message
            pearlAvg = pearlSum * 1.0 / iterations;
            std::cout << ansi::cursor_pos(5, pearlAvgMsg.length() + 1) << ansi::erase_in_line() << pearlAvg << std::endl;
            // Rods Avg Message
            rodsAvg = rodsSum * 1.0 / iterations;
            std::cout << ansi::cursor_pos(6, rodsAvgMsg.length() + 1) << ansi::erase_in_line() << rodsAvg << std::endl;
            // Pearl Max Message
            std::cout << ansi::cursor_pos(7, pearlMaxMsg.length() + 1) << ansi::erase_in_line() << maxPearls << std::endl;
            // Rods Max Message
            std::cout << ansi::cursor_pos(8, rodsMaxMsg.length() + 1) << ansi::erase_in_line() << maxRods << std::endl;
        }

    }

    //      Iterations                         Pearl Avg          Rods Avg          Pearls Max          Rods Max
    file << "Iterations,Pearl Avg,Rods Avg,Max Pearls,Max Rods\n";
    file << iterations << "," << std::fixed << pearlAvg << "," << rodsAvg << "," << maxPearls << "," << maxRods << "\n";
    file << "Pearl Distribution\n";
    for (int i = 0; i < 263; i++) {
        if (i == 262)
            file << i;
        else
            file << i << ",";
    }
    file << "\n";
    for (int i = 0; i < 263; i++) {
        if (i == 262)
            file << pearlInstances[i];
        else
            file << pearlInstances[i] << ",";
    }
    file << "\nRod Distribution\n";
    for (int i = 0; i < 306; i++) {
        if (i == 305)
            file << i;
        else
            file << i << ",";
    }
    file << "\n";
    for (int i = 0; i < 306; i++) {
        if (i == 305)
            file << rodInstances[i];
        else
            file << rodInstances[i] << ",";
    }

    std::cout << std::endl << "SIGINT Detected" << ansi::show_cursor() << std::endl;
    file.close();

    return 0;
}
