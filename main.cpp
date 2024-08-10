#include <iostream>
#include "GAS.h"
using namespace std;

//************************************************************************************
//FITNESS
//************************************************************************************
bool Fitness::operator<(const Fitness& rhs) const {
    return (fitness< rhs.fitness); //Sort Fitness
}
bool Fitness::operator==(const Fitness& rhs) const{
    return (fitness == rhs.fitness); //Unique budan
}
Fitness::Fitness(){
    fitness= 0.0;
    isParent = false;
    index = 0;
}

//************************************************************************************
//CANDIDATE
//************************************************************************************
void Candidate::create(int size , int sample[]){
    int value , block , square , position;
    bool valueFound;
    puzzleSize = size;//9

    for(position = 0 ; position < puzzleSize*puzzleSize; position++){
        puzzle[position]=sample[position]; 
    }

    for(block = 0; block < puzzleSize*puzzleSize; block+= puzzleSize){
        for(value = 1 ; value<= puzzleSize; value++){
            valueFound = false;
            for(square=0; square<puzzleSize; square++){
                if(value==puzzle[block+square]){
                    valueFound = true;
                }
            }
            if(!valueFound){
                    position= rand()%puzzleSize;
                    while(puzzle[block+position]!=0){
                        position = (position+1)%puzzleSize;
                    }
                    puzzle[block+position] = value;
            }
        }
    }
}

float Candidate::calcFitness(){
    int sumFitness = 0 , complFitness = 0; //numberhaye motefvet
    int i , j , k , l;
    int nrt = sqrt(puzzleSize); //3  n root
    int sum , prod , completeness;
    int seen[9];
    int optprod = 362880; // ==9!
    int optsum = 45;
    float prodFitness = 0.0;

    //per row
    for(i= 0; i< puzzleSize*puzzleSize; i+= nrt*puzzleSize){
        for(j=0; j<puzzleSize; j+=nrt){
            sum = 0 ;
            prod = 1;
            completeness = 0;

            for(k=0; k< puzzleSize*nrt; k+=puzzleSize){
                for(l=0 ; l<nrt; l++){
                    sum += puzzle[i+j+k+l];
                    prod*=puzzle[i+j+k+l];
                    seen[puzzle[i+j+k+l]-1] = true;
                }
            }

            for(k=0; k<puzzleSize;k++){
                if(seen[k]){
                    completeness++;
                }
            }

            sumFitness += abs(optsum-sum);
            prodFitness += sqrt(abs(optprod-prod));
            complFitness += puzzleSize-completeness;
        }
    }

    //per column
    for(k=0; k<puzzleSize*nrt; k+=puzzleSize){
        for(l=0; l<nrt; l++){
            sum = 0;
            prod =1 ;
            completeness = 0;

            for(i=0; i<puzzleSize*puzzleSize; i+= nrt*puzzleSize){
                for(j=0; j<puzzleSize; j+=nrt){
                    sum += puzzle[i+j+k+l];
                    prod *= puzzle[i+j+k+l];
                    seen[puzzle[i+j+k+l]-1] = true;
                }
            }
            for(i=0 ; i<puzzleSize; i++){
                if(seen[i]){
                    completeness++;
                }
            }
            sumFitness += abs(optsum-sum);
            prodFitness += sqrt(abs(optprod-prod));
            complFitness += puzzleSize-completeness;
        }
    }

    fitness = 10.0*sumFitness + prodFitness + 50.0*complFitness;
    return fitness;
}
//Swap the value of two random position in a random block
void Candidate::mutation(int sample[]){
    int block = rand()%puzzleSize;
    int positionA = rand()%puzzleSize;
    int positionB = rand()%puzzleSize;
    int temp;
    int  nAttempts = 0;
    while((positionA==positionB || sample[block*puzzleSize+positionA] || sample[block*puzzleSize+positionB]) && nAttempts<puzzleSize*puzzleSize){
        block = rand()%puzzleSize;
        positionA = rand()%puzzleSize;
        positionB = rand()%puzzleSize;
        nAttempts++;
    }

    if(nAttempts>=puzzleSize*puzzleSize){
        //finish
        return;
    }
    temp = puzzle[block*puzzleSize+positionA];
    puzzle[block*puzzleSize+positionA] = puzzle[block*puzzleSize+positionB];
    puzzle[block*puzzleSize+positionB] = temp;
}

void Candidate::crossover(Candidate & parentA, Candidate & parentB){
    int i , j , nrt;
    int method = rand()%3;
    int offset;
    bool parent;

    puzzleSize = parentA.puzzleSize;
    nrt = sqrt(puzzleSize);

    switch(method){
        case 0 : //Random
            for(i=0; i<puzzleSize*puzzleSize; i+=puzzleSize){
                parent = rand()%2;
                for(j=0 ; j< puzzleSize; j++){
                    if(parent){
                        puzzle[i+j] = parentA.puzzle[i+j];
                    }
                    else{
                        puzzle[i+j] = parentB.puzzle[i+j];
                    }
                }
            }
        break;
        
        case 1: //per Row
            offset = 1+rand()%(nrt-1);
            for(i=0; i< offset*puzzleSize*nrt; i++){
                puzzle[i] = parentA.puzzle[i];
            }
        break;

        case 2: //per Column
            offset = 1+rand()%(nrt-1);
            for(i=0; i<puzzleSize*puzzleSize; i+=puzzleSize){
                for(j=0; j<offset*puzzleSize; j++){
                    puzzle[i+j] = parentB.puzzle[i+j];
                }
            }
        break;

        default:
            cout<< "No Crossover. \n";
        break;
    }
}


void Candidate::print(){
    int i, j , k , l, nrt = sqrt(puzzleSize);

    for(i=0; i<puzzleSize*puzzleSize; i+=nrt*puzzleSize){
        for(j=0 ; j<puzzleSize; j+=nrt){
            for(k=0; k<puzzleSize*nrt; k+=puzzleSize){
                for(l=0; l<nrt; l++){
                    cout << puzzle[i+j+k+l] << " ";
                }
                if(k<(puzzleSize*nrt)-puzzleSize){
                    cout << "| ";
                }
            }
            cout << endl;
        }
        if(i< (puzzleSize*puzzleSize)-(nrt*puzzleSize)){
            for(j=0; j<puzzleSize+nrt-1;j++){
                cout << "- ";
            }
            cout << endl;
        }
    }
    cout << endl;
}

//************************************************************************************
//GA
//************************************************************************************

//Instructor
//push some randome candidates in "children & parents" vectors
Sudoku::Sudoku(int nChildren, int nParents){
    int i;
    for(i=0; i< nChildren; i++){
        children.push_back(Candidate());
    }
    for(i=0; i<nParents; i++){
        parents.push_back(Candidate());
    }
}

Sudoku::~Sudoku(){
    children.clear();
    parents.clear();
}

//read sudoku from Text
void Sudoku::readFromFile(string szFileName){
    fstream file(szFileName.c_str(), ios::in);
    int i , j , k , l;
    int nrt;
    if (!file.is_open()){
        cout << "File not found!\n";
        exit(0);
    }
    nrt = sqrt(puzzleSize);  //3
    //puzzleSize : 9
    //nrt*puzzleSize => 27
    //for(i) => each 3 column or we can say each 3×3 sub puzzle(Vertical)
    //for(j) => each 3 block per row or we can say each 3×3 sub puzzle(Horizontal)
    //for(k) => each row
    //for(l)=> each column
    for(i=0 ; i< puzzleSize*puzzleSize; i+=nrt*puzzleSize){
        for(j=0; j<puzzleSize; j+=nrt){
            for(k=0 ; k< puzzleSize*nrt; k+=puzzleSize){
                for(l= 0 ; l < nrt; l++){
                    file >> sample[i+j+k+l];
                }
            }
        }
    }

    file.close();
}

void Sudoku::solveElitair(bool isPrint){
    int i,j , parentA , parentB , generation =0 , nParentsNextGen = 0.25*POPULATION;
    float mutate , MUTATION_RATE = MUTATION_RATE_MIN;

    Candidate parentCopy[POPULATION];
    bool isDone = false;

    //iterator ha pointerhayi baraye gardesh dar array hastand
    vector <Fitness> ::iterator endUnique;
    vector <Fitness>::iterator it;
    vector <Fitness> fitParents;
    vector <Fitness> fitChildren;

    //initialise parents; n random parents, givens at the right place

    for(i=0; i<POPULATION; i++){
        parents[i].create(puzzleSize, sample);
        parents[i].calcFitness();
    } 

    //initialise fitness place holder
    // be andazeye population be fitParentmemory midahad
    fitParents.reserve(POPULATION);
    for(i=0; i<POPULATION; i++){
        fitParents.push_back(Fitness());
    }

    fitChildren.reserve(POPULATION);
    for(i=0; i<CHILDREN; i++){
        fitChildren.push_back(Fitness());
    }

    while(generation<GENERATION && !isDone){
        generation++;

        //copy parent fitness to vector

        for(i=0; i<POPULATION; i++){
            fitParents[i].fitness = parents[i].fitness;
            fitParents[i].index = i;
        }

        //Create Children
        for(i=0; i<CHILDREN; i++){
            do{
                parentA = rand()%POPULATION;
                parentB = rand()%POPULATION;
            }while(parentA==parentB);
            mutate = float(rand()%5) /100.0; //will we mutate?

            children[i].crossover(parents[parentA] , parents[parentB]);

            if(mutate<=MUTATION_RATE){
                    children[i].mutation(sample);
            }

            fitChildren[i].fitness = children[i].calcFitness();
            fitChildren[i].index = i;
        }

        sort(fitParents.begin(), fitParents.end());
        sort(fitChildren.begin(), fitChildren.end());

        //copying the original parents
        for(i=0; i<POPULATION; i++){
            parentCopy[i] = parents[i];
        }

        //copy n best parents
        //az ebteda ta ntahaye array ra migardim va fitParents haye gheire takrary ra da endUnique save mikonim
        endUnique = unique(fitParents.begin() , fitParents.end());
        it = fitParents.begin();
        for(i=0; i<nParentsNextGen; i++){
            parents[i] = parentCopy[it->index];
            if(it != endUnique){
                ++it;
            }
            else{
                it=fitParents.begin();
            }
        }

        //copy N best children
        endUnique = unique(fitChildren.begin() , fitChildren.end());
        it = fitChildren.begin();
        for(i=0; i<POPULATION - nParentsNextGen; i++){
            parents[nParentsNextGen+i] = children[it -> index];
            if(it != endUnique){
                ++it;
            }
            else{
                it = fitChildren.begin();
            }
        }

        sort(parents.begin() , parents.end());

        if(isPrint){
            cout << "generation" << generation << ": ";
            for(i=0; i<5 ; i++){
                cout<< parents[i].fitness << " ";
            }
            cout << endl;
        }

        //Stop if optimall solution is reached
        if(parents[0].fitness ==0){
            isDone = true;
        }
        
        //print boards while running
        if(isPrint && generation%1000==0){
            for(i=0; i<3; i++){
                parents[i].print();
            }
        }
        MUTATION_RATE += MUTATION_RATE_INCREASE;
    }

    if(isPrint){
        parents[0].print();
    }
    cout << parents[0].calcFitness() << endl << generation << endl;
}

//estefade baraye Sort kardan:
bool Candidate::operator<(const Candidate& rhs) const {
    return fitness < rhs.fitness;
}

int main(){
    Sudoku sudoko(CHILDREN, POPULATION);
    srand(time(NULL));
        sudoko.readFromFile("sample.txt");
        cout << "done!\n";
        sudoko.solveElitair(true);
    return 0;
}