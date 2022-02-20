#pragma once

#include <regex>
#include <cassert>
#include <fstream>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>

#include "Cube.h"
#include "Gui.h"

//"Main" class, this is where all the magic happens

class Parser
{
private:
    std::stringstream            log;
    std::shared_ptr<RC>         cube;
    std::unique_ptr<Gui>          ui;
    std::vector<std::string> history;
public:
    Parser() = default;
    ~Parser();

    void parse();
    void startGui();
    void createCube(size_t n);
    void randomShuffle(int moves);
    void parseMove(std::string move);
    void process(std::vector<std::string>& cmdlist, bool& running);
    void rotateCube(char move, int from, int to, int rotations, bool clockwise);

    bool assertCube();
    bool isNumber(std::string s);

    char chassert(std::istream& is, char expected);
};

