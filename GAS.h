#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>  //for rand and srand
#include <ctime>    //for time()
#include <cmath>
#include <algorithm>    //sort
using namespace std;

//--------------------------------
const int POPULATION = 400;
const int GENERATION = 100000;
const int CHILDREN = 100;
const int puzzleSize = 9;
const float MUTATION_RATE_MIN = 0.01;
const float MUTATION_RATE_MAX = 0.05;
const float MUTATION_RATE_INCREASE = (MUTATION_RATE_MAX - MUTATION_RATE_MIN)/GENERATION;
//--------------------------------
struct Fitness {
    Fitness();

    int index;
    bool isParent;
    float fitness;

    bool operator<(const Fitness& rhs) const;
    bool operator==(const Fitness& rhs) const;
};

class Candidate{
    public :
        void create(int size , int sample[]);
        void print();
        void crossover(Candidate & parentA , Candidate & parentB);
        void mutation(int sample[]);
        float calcFitness();
        bool operator<(const Candidate& rhs) const;
        float fitness;
        int puzzle[81];
        int puzzleSize;

};

class Sudoku{
    public:
        Sudoku(int nChildren, int nParent);
        ~Sudoku();
        void readFromFile(string szFileName); 
        void solveElitair(bool isPrint=true);

    private:
        int sample[81];
        //const int puzzleSize = 9; //9
        vector <Candidate> parents, children;

};





