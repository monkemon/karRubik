#include "Cube.h"
#include "Parser.h"
#include "Gui.h"

Parser::~Parser()
{
    std::ofstream log_out;
    log_out.open("log.txt", std::ios::out);
    log_out << this->log.str();
    log_out.close();

    if (this->ui)
    {
        this->ui->die();
    }
}

//running as long as the 'running' bool is true, the bool is passed
//to the process function, and from process function set to 0 when needed
void Parser::parse()
{
    bool running = true;
    std::string command;
    std::vector<std::string> buffer;

    std::cout << "Welcome to KarRubik, type 'help' for a list of commands, 'exit' to quit" << std::endl;

    while(running)
    {
        std::cout << "\u27a4  ";  //unicode arrow, not sure if works on every system :D
        std::getline(std::cin, command);

        std::string delimiter = " ";
        std::string token;
        size_t pos = 0;

        //split the 'command' by space char, push all parts into vector
        while ((pos = command.find(delimiter)) != std::string::npos)
        {
            token = command.substr(0, pos);
            buffer.push_back(token);
            command.erase(0, pos + 1); // +1 for the length of delimiter " "
        }
        buffer.push_back(command);

        //the huge function down under
        this->process(buffer, running);

        buffer.clear();
    }
}

static std::regex REGEX_QUIT("exit|q");

void Parser::process(std::vector<std::string>& cmdlist, bool& running)
{
    if (std::regex_match(cmdlist[0], REGEX_QUIT))
    {
        running = false;
        return;
    }

    //ugh...
    else if (std::regex_match(cmdlist[0], std::regex("help|h")))
    {
        std::cout << "help/h               - opens this help menu" << std::endl;
        std::cout << "exit/q               - quits the program" << std::endl;
        std::cout << "loadh/lh             - loads cube from history file" << std::endl;
        std::cout << "loadc/lc             - loads cube from cube file" << std::endl;
        std::cout << "newcube/nc/n <int>   - creates new n x n x n cube" << std::endl;
        std::cout << "ls                   - list history of moves" << std::endl;
        std::cout << "goto <int>           - goes back to a particular step in history" << std::endl;
        std::cout << "gui                  - displays the cube graphically" << std::endl;
        std::cout << "cls                  - closes the gui" << std::endl;
        std::cout << "printcube/pc/p       - prints cube to console" << std::endl;
        std::cout << "saveh/sh <name>      - saves the cube in history format into current directory" << std::endl;
        std::cout << "savec/sc <name>      - saves the cube as is into current directory" << std::endl;
        std::cout << "savepng/png <name>   - saves the cube as png to current directory" << std::endl;
        std::cout << "random/shuffle <int> - randomly performs n moves" << std::endl << std::endl;
        std::cout << "NOTE: creating a new cube when one is already loaded discards cube and history" << std::endl;
        std::cout << "Please exit the gui via 'cls' command, otherwise the app will crash" << std::endl << std::endl;
        std::cout << "Cube move: [from-to][move][movecount][']" << std::endl;
        std::cout << "Valid cube moves: F, R, B, L, U, D. Letters stand for Front, Right, Back, Left, Up, Down" << std::endl;
        std::cout << "You can only perform moves when the cube had been created, example moves:" << std::endl;
        std::cout << "1-1R2'\trotates the first face of the right side twice, counterclockwise" <<std::endl;
        std::cout << "1-2B\trotates first and second layer of B face, once, clockwise" <<std::endl << std::endl;
    }

    //new cube
    else if (std::regex_match(cmdlist[0], std::regex("newcube|nc|n")))
    {
        //no number provided
        if (cmdlist.size() == 1 || cmdlist[1].empty())
        {
            std::cout << "You must specify the size of the cube" << std::endl;
            std::cout << "Example usage: newcube 4, nc 6, newcube 3" << std::endl;
            return;
        }

        //second arg must be number
        if (this->isNumber(cmdlist[1]) == false)
        {
            std::cout << "You must provide a valid integer as the size" << std::endl;
            this->log << "invalid cube size provided" << std::endl;
            return;
        }

        //second argument is the size of a new cub
        size_t cube_size = std::stoi(cmdlist[1]);
        if (cube_size == 0)
        {
            std::cout << "Cube size can't be zero" << std::endl;
            this->log << "attempted to create a zero-sized cube" << std::endl;
            return;
        }
        std::cout << "Creating a new" << cube_size << "x" << cube_size << "x" << cube_size << " cube..." << std::endl; // :D

        //reset smart pointer -> calls destructors, clear history, reload(update) the gui
        this->cube.reset();
        this->history.clear();
        this->createCube(cube_size);
        if (this->ui) 
        {
            this->ui->clear();
            this->ui->setdata(this->cube);
            this->ui->update();
        }

        std::cout << "You can now type moves" << std::endl;
    }

    //a move command to further parsing
    else if (std::regex_match(cmdlist[0], std::regex("[1-9][0-9]*-[1-9][0-9]*[FRULDB][0-9]*[']?")))
    {
        if (this->assertCube())
        {
            this->parseMove(cmdlist[0]);
        }
        return;
    }

    //simplified command
    else if (std::regex_match(cmdlist[0], std::regex("[FRBLUD][0-9]*[']?")))
    {
        if (this->assertCube())
        {
            std::stringstream placeholder;
            placeholder << "1-1" << cmdlist[0];

            this->parseMove(placeholder.str());
        }
        return;
    }

    //simplified command
    else if (std::regex_match(cmdlist[0], std::regex("[0-9]+[FRBLUD][0-9]*[']?")))
    {
        if (this->assertCube())
        {
            std::stringstream placeholder;
            placeholder << cmdlist[0];
            size_t layer;
            placeholder >> layer;

            placeholder.str(std::string());
            placeholder << layer << "-" << cmdlist[0];


            this->parseMove(placeholder.str());
        }
        return;
    }

    else if (std::regex_match(cmdlist[0], std::regex("random|shuffle")))
    {
        if (cmdlist.size() == 2 && this->isNumber(cmdlist[1]) && !cmdlist[1].empty())
        {
            if (this->assertCube())
            {
                int moves = std::stoi(cmdlist[1]);

                this->randomShuffle(moves);
            }
        }
        else
        {
            std::cout << "Please provide the number of random moves. Usage: random 5" << std::endl;
            this->log << "attempted to random shuffle without providing valid number of moves" << std::endl;
        }
        return;
    }

    //print cube to console
    else if (std::regex_match(cmdlist[0], std::regex("print|pc|p|printcube")))
    {
        if (this->assertCube())
        {
            this->cube->print();
        }        
        return;
    }

    //start gui
    else if (std::regex_match(cmdlist[0], std::regex("gui|g|display|show")))
    {
        if (this->assertCube())
        {
            this->startGui();
        }
        return;
    }

    //list commands
    else if (std::regex_match(cmdlist[0], std::regex("ls")))
    {
        if (this->assertCube())
        {
            int i(0);
            std::cout << "History:" << std::endl;
            for (auto& s : this->history)
            {
                std::cout << i << ": " << s << std::endl;
                i++;
            }
            std::cout << std::endl;
        }
        return;
    }
    
    else if (std::regex_match(cmdlist[0], std::regex("savepng|png")))
    {
        std::string filename;
            
        if (this->assertCube() && this->ui)
        {
            if (cmdlist.size() > 1 && !cmdlist[1].empty())
            {
                filename = cmdlist[1];
            }
            else
            {
                std::cout << "File will be written as \'default_cube.png\'" << std::endl;
                filename = "default_cube.png";
            }
            
            this->ui->savePNG(filename);
        }
        else
        {
            std::cout << "You must have gui open in order to save images" << std::endl;
            this->log << "attemt to save image without having gui open" << std::endl;
        }
        return;
    }

    //goes back to some command in history
    else if (std::regex_match(cmdlist[0], std::regex("goto")))
    {
        if (this->assertCube())
        {
            if (this->history.empty())
            {
                std::cout << "The history is empty" << std::endl;
                this->log << "attempted to go to a non-existing point in history" << std::endl;
                return;
            }
            if (cmdlist.size() == 2 && this->isNumber(cmdlist[1]))
            {
                size_t step = std::stoi(cmdlist[1]);
                std::clamp(step, (size_t)0, cmdlist.size());

                this->cube->restore();
                this->ui->update();

                std::vector<std::string> tempHistory(this->history);
                this->history.clear();

                for (size_t i = 0; i <= step; i++)
                {
                    this->parseMove(tempHistory[i]);
                }
            }
        }
        return;
    }

    //loads cube from history file
    else if (std::regex_match(cmdlist[0], std::regex("loadh|lh")))
    {
        if (cmdlist.size() == 1 || cmdlist[1].empty())
        {
            std::cout << "You must specify a filename. Usage: > lh history.txt" << std::endl;
        }

        std::ifstream in;
        in.open(cmdlist[1], std::ios::in);

        if (!in.is_open())
        {
            std::cout << "File " << cmdlist[1] << " could not be found" << std::endl;
            this->log << "attempted to load history from non-existing file" << std::endl;
            in.close();
            return;
        }

        size_t cubsize(0);
        std::string buffer;

        this->chassert(in, 'N');
        this->chassert(in, ' ');

        in >> cubsize;

        this->cube.reset();
        this->history.clear();
        this->createCube(cubsize);

        while (in >> buffer)
        {
            this->parseMove(buffer);
        }
        
        in.close();
        if (this->ui)
        {
            this->ui->clear();
            this->ui->setdata(this->cube);
            this->ui->update();
        }
        return;
    }

    //saving the cube as history
    else if (std::regex_match(cmdlist[0], std::regex("saveh|sh")))
    {
        std::string filename;
        if (this->assertCube())
        {
            if (cmdlist.size() > 1 && !cmdlist[1].empty())
            {
                filename = cmdlist[1];
            }
            else
            {
                std::cout << "File will be written as \'history.txt\'" << std::endl;
                filename = "history.txt";
            }

            std::ofstream file;
            file.open(filename, std::ios::out);
            if (!file.is_open())
            {
                std::cout << "File " << filename << "could not be created" << std::endl;
                this->log << "file creation failed" << std::endl;
                file.close();
                return;
            }

            file << "N " << this->cube->getSize() << std::endl;

            for (auto& e : this->history)
            {
                file << e << std::endl;
            }

            file.close();
        }
        return;
    }

    //loads cube from as-is file
    else if (std::regex_match(cmdlist[0], std::regex("loadc|lc")))
    {
        if (cmdlist.size() == 1 || cmdlist[1].empty())
        {
            std::cout << "You must specify a filename. Usage: > lc cube.txt" << std::endl;
        }

        std::ifstream in;
        in.open(cmdlist[1], std::ios::in);

        if (!in.is_open())
        {
            std::cout << "File " << cmdlist[1] << " could not be found" << std::endl;
            this->log << "attempted to load a non-existing file" << std::endl;
            in.close();
            return;
        }

        size_t cubsize(0);
        std::string buffer;

        this->chassert(in, 'N');
        this->chassert(in, ' ');

        in >> cubsize;

        this->cube.reset();
        this->history.clear();
        this->createCube(cubsize);

        char color(0);
        for (int i = 0; i < (int)cubsize ; i++)
        {
            for (int j = 0; j < (int)cubsize * 6; j++)
            {
                in >> color;
                int intcolor = color - 48;
                if (in.eof() || intcolor < 0 || intcolor > 5)
                {
                    this->cube->restore();
                    std::cout << "Bad/ corrupted file format" << std::endl;
                    this->log << "reading from a corrupted file" << std::endl;
                    return;
                }
                this->cube->set(static_cast<CubeColor>(intcolor), i, j);
            }
        }
        
        in.close();
        if (this->ui)
        {
            this->ui->clear();
            this->ui->setdata(this->cube);
            this->ui->update();
        }
        return;
    }

    //saving the cube as is
    else if (std::regex_match(cmdlist[0], std::regex("savecube|sc")))
    {
        std::string filename;
        if (this->assertCube())
        {
            if (cmdlist.size() > 1 && !cmdlist[1].empty())
            {
                filename = cmdlist[1];
            }
            else
            {
                std::cout << "File will be written as \'cube_default.txt\'" << std::endl;
                filename = "cube_default.txt";
            }

            std::ofstream file;
            file.open(filename, std::ios::out);
            if (!file.is_open())
            {
                std::cout << "File " << filename << "could not be created" << std::endl;
                this->log << "writing to a file failure" << std::endl;
                file.close();
                return;
            }

            file << "N " << this->cube->getSize() << std::endl;

            for (auto& row : this->cube->getData())
            {
                for (auto& col : row)
                {
                    file << static_cast<int>(col);
                }
                file << std::endl;
            }
            file.close();
        }
        return;
    }

    //closes the gui
    else if (std::regex_match(cmdlist[0], std::regex("cls")))
    {
        if (this->assertCube())
        {
            this->ui->die();
        }
        return;
    }

    else
    {
        std::cout << "\'" << cmdlist[0] << "\' : unknown command" << std::endl;
    }
}

bool Parser::isNumber(std::string s)
{
    for (auto& ch : s)
    {
        if (std::isdigit(ch) == 0) return false;
    }
    return true;
}

bool Parser::assertCube()
{
    if (!this->cube)
    {
        std::cout << "You must first create a cube with 'newcube' command" << std::endl;
        this->log << "attempted to perform operation on an empty cube" << std::endl;
        return false;
    }
    return true;
}

void Parser::createCube(size_t n)
{
    this->cube = std::make_shared<RC>(n);
}

void Parser::parseMove(std::string move)
{
    //save to history
    this->history.push_back(move);
    std::stringstream ss;
    ss << move;

    int from;
    int to;
    int rotations = 1;
    char moveLetter;
    bool clockwise = true;

    ss >> from;
    this->chassert(ss, '-');
    ss >> to;
    ss >> moveLetter;
    
    //no move count specified
    if (!(ss.peek() == '\''))
    {
        ss >> rotations;
    }
    clockwise = ss.get() == '\'' ? false : true;

    this->rotateCube(moveLetter, from, to, rotations, clockwise);
    
    //we can rotate without gui
    if (this->ui)
    {
        this->ui->update();
    }
}

//skip position in a stream, used in reading history from file
char Parser::chassert(std::istream& is, char expected)
{
    char c = is.get();
    assert(c == expected);
    return c;
}

// "Vidlácký legatus"
void Parser::rotateCube(char move, int from, int to, int rotations, bool clockwise)
{
    //pointer to a function within RC class
    void (RC::*function)(int, int, bool) = nullptr;

    switch (move)
    {
        case 'R':
            function = &RC::performRight;
            break;
        
        case 'U':
            function = &RC::performUp;
            break;
        
        case 'F':
            function = &RC::performFront;
            break;

        case 'L':
            function = &RC::performLeft;
            break;

        case 'D':
            function = &RC::performDown;
            break;

        case 'B': 
            function = &RC::performBack;
            break;
        
        default:
            break;
    }

    for (int i = 0; i < rotations; i++)
    {
        //cursed
        //call the function, from-1 and to-1 because we can't move 0th face of a cube
        (*(this->cube).*function)(from - 1, to - 1, clockwise);
    }
}

//randomly shuffle n moves
void Parser::randomShuffle(int moves)
{
    std::mt19937 generator(time(nullptr));
    std::string moveLetter("FRBLUD");
    int from(0);
    int to(0);

    for (int i = 0; i < moves; i++)
    {
        std::string move;
        int count = generator() % 3 + 1;  //rotating faces more than 3x doesn't make sense
        int index = generator() % 6;
        from = generator() % this->cube->getSize() + 1;
        to = generator() % this->cube->getSize() + 1;

        if (from > to)
        {
            int temp = from;
            from = to;
            to = temp;
        }

        move += std::to_string(from);
        move += '-';
        move += std::to_string(to);
        move += moveLetter[index];
        move += std::to_string(count);

        if (generator() % 2 == 0)
            move += '\'';

        //little delay so the moves' randomeness is more diverse
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000000));
        this->parseMove(move);
    }
}

void Parser::startGui()
{
    //if new gui started, old one is deleted
    this->ui = std::make_unique<Gui>();
    this->ui->createWindow();
    this->ui->setdata(this->cube);
    this->ui->update();
}
