#include "Cube.h"

RC::RC(size_t sideLength)
{
    this->sideLength = sideLength;

    //new N x 6N array, stored in vector of vectors so i can reference it like cube[i][j]
    this->size = sideLength * sideLength * 6;

    //push N rows of data
    for ( size_t i = 0; i < this->sideLength; i++)
    {
        this->data.push_back(std::vector<CubeColor>());
    }

    //push 6N elements into each row
    for ( int i = 0; i < 6; i++)
    {
        CubeColor actual_color = static_cast<CubeColor>(i);
        for ( size_t j = 0; j < this->sideLength; j++)
        {
            for ( size_t k = 0; k < this->sideLength; k++)
            {
                this->data[j].push_back(actual_color);
            }
        }
    }
}

//used in Parser 'savecube' command
CubeData&  RC::getData()
{
    return this->data;
}

//relic of the past
void RC::print()
{
    int i(1);
    for (auto& side : this->data)
    {
        for (auto& element: side)
        {
            std::cout << static_cast<int>(element);
            if (i % this->sideLength == 0)
            {
                std::cout << " ";
            }
            i++;
        }
        std::cout << std::endl;
    }
    return;
}

//returns the cube to solved state
void RC::restore()
{
    for (auto& e : this->data)
    {
        e.clear();
    }
    this->data.clear();

    for ( size_t i = 0; i < this->sideLength; i++)
    {
        this->data.push_back(std::vector<CubeColor>());
    }

    for ( int i = 0; i < 6; i++)
    {
        CubeColor actual_color = static_cast<CubeColor>(i);
        for ( size_t j = 0; j < this->sideLength; j++)
        {
            for ( size_t k = 0; k < this->sideLength; k++)
            {
                this->data[j].push_back(actual_color);
            }
        }
    }
}

//used in Gui->update()
std::optional<CubeColor> RC::getElement(size_t x, size_t y)
{
    if (x >= this->sideLength  * 6|| y >= this->sideLength)
    {
        return  std::nullopt;
    }

    return this->data[y][x];
}

//used in Parser 'loadc' command
void RC::set(CubeColor c, int x, int y)
{
    this->data[x][y] = c;
}

//affects only this face, not neighbours
void RC::rotateFace(int faceID, bool clockwise)
{
    int startIndex = faceID * this->sideLength;

    if (clockwise)
    {
        for (int i = 0; i < (this->sideLength / 2); i++)
        {
            for (int j = i + startIndex; j < this->sideLength - 1 - i + startIndex; j++)
            {
                //top left corner
                auto temp = this->data[i][j];

                //top left corner = top right corner
                this->data[i][j] = this->data[this->sideLength - 1 - (j - startIndex)][i + startIndex];

                //top right corner = bottom right corner
                this->data[this->sideLength - 1 - (j - startIndex)][i + startIndex]
                    = this->data[this->sideLength - 1 - i][startIndex + this->sideLength - 1 - (j - startIndex)];

                //bottom right corner = bottom left corner
                this->data[this->sideLength - 1 - i][startIndex + this->sideLength - 1 - (j - startIndex)]
                    = this->data[j - startIndex][this->sideLength - 1 - i + startIndex];

                //bottom left corner = temp (top left)	
                this->data[j - startIndex][this->sideLength - 1 - i + startIndex] = temp;
            }
        }
    }

    else
    {
        for (int i = 0; i < (this->sideLength / 2); i++)
        {
            for (int j = i + startIndex; j < this->sideLength - 1 - i + startIndex; j++)
            {
                //top left
                auto temp = this->data[i][j];

                //top left = bottom left
                this->data[i][j] = this->data[j - startIndex][this->sideLength - 1 - i + startIndex];

                //bottom left = bottom right
                this->data[j - startIndex][this->sideLength - 1 - i + startIndex] 
                    = this->data[this->sideLength - 1 - i][startIndex + this->sideLength - 1 - (j - startIndex)];

                //bottom right = top right
                this->data[this->sideLength - 1 - i][startIndex + this->sideLength - 1 - (j - startIndex)]
                    = this->data[this->sideLength - 1 - (j - startIndex)][i + startIndex];

                //top right = temp (top left)
                this->data[this->sideLength - 1 - (j - startIndex)][i + startIndex] = temp;
            }
        }
    }
}

int rotate(int start, int stop, int increment) {
    for (; start < stop; start += increment) {

    }
}


//utility function to rotate values of pointers, like std::rotate but actually rotates values of poiners
//used when storing references to particular rows and rotating them
//in Right and Front rotations
void RC::rotate(std::vector<CubeColor *>& refs, bool clockwise)
{
    if (clockwise)
    {
        for (int i = 0; i < this->sideLength; i++)
        {
            auto last = *refs[refs.size() - 1];
            for (int i = refs.size()-1; i >= 1; i--)
            {
                *refs[i] = *refs[i - 1];
            }
            *refs[0] = last;
        }
    }

    else
    {
        for (int i = 0; i < this->sideLength; i++)
        {
            auto first = *refs[0];
            for (int i = 1; i < refs.size(); i++)
            {
                *refs[i - 1] = *refs[i];
            }
            *refs[refs.size()-1] = first;
        }
    }
}

void RC::performRight(int from, int to, bool clockwise)
{
    from = std::clamp(from, 0, (int)this->sideLength - 1);
    to = std::clamp(to, from, (int)this->sideLength - 1);

    for (int rows = from; rows <= to; rows++)
    {
        std::vector<CubeColor*> refs;

        //selecting the proper squares on the cube
        for (int i = this->sideLength - 1; i >= 0; i--)
        {
            refs.push_back(&this->data[i][this->sideLength -1 - rows]);
        }

        for (int i = this->sideLength - 1; i >= 0; i--)
        {
            refs.push_back(&this->data[i][5 * this->sideLength - 1 - rows]);
        }

        for (int i = 0; i < this->sideLength; i++)
        {
            refs.push_back(&this->data[i][2 * this->sideLength + rows]);
        }

        for (int i = 0; i < this->sideLength; i++)
        {
            refs.push_back(&this->data[i][5 * this->sideLength + rows]);
        }

        //rotating the reference values
        this->rotate(refs, clockwise);

    }

    if (from == 0)
    {
        this->rotateFace(1, clockwise);
    }
    
    if (to == this->sideLength - 1)
    {
        this->rotateFace(3, !clockwise);
    }
}

void RC::performUp(int from, int to, bool clockwise)
{
    from = std::clamp(from, 0, (int)this->sideLength - 1);
    to = std::clamp(to, from, (int)this->sideLength - 1);
    int endIndex = 2 * this->sideLength;
    int rotateIndex = 0;

    //rotateIndex required for std::rotate,
    //based on fact that one anticlockwise move is equal to 3 clockwise moves
    if (clockwise)
    {
        rotateIndex = this->sideLength;
    }
    else if (!clockwise)
    {
        rotateIndex = this->sideLength * 3;
    }

    for (int rows = from; rows <= to; rows++)
    {
        //it's ok to have std::rotate here
        std::rotate(this->data[rows].begin(), this->data[rows].begin() + rotateIndex, this->data[rows].end() - endIndex);
    }
    
    if (from == 0)
    {
        this->rotateFace(4, clockwise);
    }
    
    if (to == this->sideLength - 1)
    {
        this->rotateFace(5, !clockwise);
    }
}

void RC::performFront(int from, int to, bool clockwise)
{
    from = std::clamp(from, 0, (int)this->sideLength - 1);
    to   = std::clamp(to, from, (int)this->sideLength - 1);

    for (int rows = from; rows <= to; rows++)
    {
        std::vector<CubeColor*> refs;

        for (int i = 0; i < this->sideLength; i++)
        {
            refs.push_back(&this->data[i][this->sideLength + rows]);
        }

        for (int i = 0; i < this->sideLength; i++)
        {
            refs.push_back(&this->data[this->sideLength - rows - 1][5 * this->sideLength + i]);
        }

        for (int i = this->sideLength - 1; i >= 0; i--)
        {
            refs.push_back(&this->data[i][4 * this->sideLength - 1 - rows]);
        }

        for (int i = 0; i < this->sideLength; i++)
        {
            refs.push_back(&this->data[this->sideLength - rows - 1][4 * this->sideLength + i]);
        }

        this->rotate(refs, clockwise);
    }
    
    if (from == 0)
    {
        this->rotateFace(0, clockwise);
    }
    
    if (to == this->sideLength - 1)
    {
        this->rotateFace(2, !clockwise);
    }
}

//lil bit hacky but does the job, switch clockwiseness and swap from/to
//plus it gets automatically clamped
//1-2L == lengh - to -- length - from R' -> 1-2L == 2-3R'

void RC::performLeft(int from, int to, bool clockwise)
{
    int newfrom = this->sideLength - 1 - to;
    int newto = this->sideLength - 1 - from;
    this->performRight(newfrom, newto, !clockwise);
}

void RC::performDown(int from, int to, bool clockwise)
{
    int newfrom = this->sideLength - 1 - to;
    int newto = this->sideLength - 1 - from;
    this->performUp(newfrom, newto, !clockwise);
}

void RC::performBack(int from, int to, bool clockwise)
{
    int newfrom = this->sideLength - 1 - to;
    int newto = this->sideLength - 1 - from;
    this->performFront(newfrom, newto, !clockwise);
}

size_t RC::getSize()
{
    return this->sideLength;
}