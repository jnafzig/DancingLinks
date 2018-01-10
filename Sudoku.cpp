#include "DancingLinks.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>
#include <cassert>

int rowNum(int candidateIndex) {return candidateIndex / 9 / 9;}
int colNum(int candidateIndex) {return candidateIndex / 9 % 9;}
int candNum(int candidateIndex) {return candidateIndex % 9;}
int boxNum(int candidateIndex) {return rowNum(candidateIndex) / 3
                                + (colNum(candidateIndex) / 3) * 3; }
int constraintParameter1(int constraintIndex){return constraintIndex / 4 / 9;}
int constraintParameter2(int constraintIndex){return constraintIndex / 4 % 9;}
int constraintType(int constraintIndex){return constraintIndex % 4;}

int candidateIndex(int row, int col, int num) {return 9*9*row + 9*col + num;}
int constraintIndex(int parameter1, int parameter2, int type) {
  return 9*4*parameter1 + 4*parameter2 + type;
}


int main()
{

    using namespace std::chrono;


    grid sudoku(9*9*9,9*9*4);
    for (int row_i = 0; row_i < 9; row_i++) {
      for (int col_j = 0; col_j < 9; col_j++) {
        for (int num_k = 0; num_k < 9; num_k++) {
          int boxId = row_i / 3 + col_j / 3 * 3;
          int ind = candidateIndex(row_i,col_j,num_k);
          sudoku.addCell(ind, constraintIndex(boxId,num_k,0));
          sudoku.addCell(ind, constraintIndex(row_i,num_k,1));
          sudoku.addCell(ind, constraintIndex(col_j,num_k,2));
          sudoku.addCell(ind, constraintIndex(col_j,row_i,3));
        }
      }
    }

    int N = 1000;
    std::vector<double> results(N);
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
      results[i] = sudoku.algorithmX();
      sudoku.deleteStack.restoreStack();
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    double total = 0;
    for (auto x : results) total+=x;
    double avg = total/N;

    double stddev = 0;
    for (auto x : results) stddev+=pow(x-avg,2);
    stddev = sqrt(stddev/(N-1));

    std::cout << "Average result of " << N << " simulations: " <<  total/N << " standard deviation: " << stddev << std::endl;

    std::cout << "Time Elapsed: " << time_span.count() << std::endl;

}
