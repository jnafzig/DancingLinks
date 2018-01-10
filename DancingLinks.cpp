
#include <string>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <array>
#include <list>
#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include <climits>
#include <stack>
#include <math.h>

#include <cassert>

std::random_device seed;
std::mt19937 gen(seed());

int randi(int upperBound) {
  std::uniform_int_distribution<int> dist(0, upperBound-1);
  return dist(gen);
}

struct node;
struct cell;
struct grid;
struct DeleteStack;

struct node {
  node *prev = this;
  node *next = this;
  cell *start = nullptr;
  int numValid = 0;
  void addColCell(cell *cellPtr);
  void addRowCell(cell *cellPtr);
  void attach(node *nodePtr);
  node* chooseNode();
  cell* chooseCandidate();
  void remove();
  void restore();
};

struct DeleteStack {
  std::stack<cell*> deletedCellStack;
  std::stack<node*> deletedNodeStack;
  void push(cell *cellPtr);
  void push(node *nodePtr);
  std::pair<int, int> stackSize();
  void restoreStack(std::pair<int, int>);
  void restoreStack();
};

struct cell{
  cell *up = this;
  cell *down = this;
  cell *left = this;
  cell *right = this;
  node *col = nullptr;
  node *row = nullptr;
  void attachUD(cell *cellPtr);
  void attachLR(cell *cellPtr);
  void remove();
  void restore();
  int selectRowAndRemove(DeleteStack &deleteAndStack, node* &headerNodePtr);
  int verify();
  int countUD();
};

struct grid {
  std::vector<node*> cols;
  std::vector<node*> rows;
  std::vector<cell*> cells;
  node* headerNodePtr;
  DeleteStack deleteStack;
  grid(int numRows, int numCols);
  ~grid();
  void addCell(int row, int col);
  float algorithmX();
};

int cell::countUD() {
  cell* cellPtr = this->down;
  int count = 1;
  while (cellPtr != this) {
    count++;
    if (cellPtr->down->up != cellPtr) {
      std::cout << "Down up broken" << std::endl;
      return count;
    }
    if (cellPtr->up->down != cellPtr) {
      std::cout << "Up down broken" << std::endl;
      return count;
    }
    if (cellPtr->left->right != cellPtr) {
      std::cout << "Left right broken" << std::endl;
      return count;
    }
    if (cellPtr->right->left != cellPtr) {
      std::cout << "Right left broken" << std::endl;
      return count;
    }
    cellPtr = cellPtr->down;
  }
  return count;
}

int cell::verify() {
  cell* cellPtr1 = this;
  cell* cellPtr2 = this->down;
  int count = 0;
  while (cellPtr1 != cellPtr2) {
    count++;
    if (cellPtr1->down->up != cellPtr1) {
      std::cout << "Down up broken" << std::endl;
      return count;
    }
    if (cellPtr1->up->down != cellPtr1) {
      std::cout << "Up down broken" << std::endl;
      return count;
    }
    if (cellPtr1->left->right != cellPtr1) {
      std::cout << "Left right broken" << std::endl;
      return count;
    }
    if (cellPtr1->right->left != cellPtr1) {
      std::cout << "Right left broken" << std::endl;
      return count;
    }
    cellPtr1 = cellPtr1->down;
    cellPtr2 = cellPtr2->down->down;
  }
  std::cout << "Loop found after " << count << std::endl;
  return count;
}

int cell::selectRowAndRemove(DeleteStack &deleteAndStack, node* &headerNodePtr){
  // Loop over all cells in selected row
  cell* cellPtr1 = this;
  do {
    cellPtr1 = cellPtr1->right;

    // Loop over all cells in current column except for selected row
    cell *cellPtr2 = cellPtr1->down;
    while (cellPtr2 != cellPtr1) {

      // Loop over and remove all cells in current row
      cell *cellPtr3 = cellPtr2;
      do {
        cellPtr3 = cellPtr3->right;

        if (cellPtr3->col->start == cellPtr3) {
          if (cellPtr3->down == cellPtr3) {
            return 0;
          } else {
            cellPtr3->col->start = cellPtr3->down;
          }
        }
        deleteAndStack.push(cellPtr3);
      } while (cellPtr3 != cellPtr2);

      cellPtr2 = cellPtr2->down;
    }
    if (cellPtr1->col->next == cellPtr1->col) return 1;
    if (cellPtr1->col == headerNodePtr) headerNodePtr = headerNodePtr->next;
    deleteAndStack.push(cellPtr1->col);

  } while (cellPtr1 != this);
  return -1;
}

void cell::remove() {
  this->right->left = this->left;
  this->left->right = this->right;
  this->down->up = this->up;
  this->up->down = this->down;
  this->col->numValid--;
}

void cell::restore() {
  this->right->left = this;
  this->left->right = this;
  this->down->up = this;
  this->up->down = this;
  this->col->numValid++;
}

void cell::attachLR(cell *cellPtr) {
  cellPtr->left = this;
  cellPtr->right = this->right;
  this->right->left = cellPtr;
  this->right = cellPtr;
}

void cell::attachUD(cell *cellPtr) {
  cellPtr->up = this;
  cellPtr->down = this->down;
  this->down->up = cellPtr;
  this->down = cellPtr;
}

node* node::chooseNode() {
  int min = this->numValid;
  node *minPtr = this;
  for (node *nodePtr = this->next; nodePtr != this; nodePtr = nodePtr->next) {
    if (nodePtr->numValid < min) {
      min = nodePtr->numValid;
      minPtr = nodePtr;
    }
  }
  return minPtr;
}

cell* node::chooseCandidate() {
  int randIndex = randi(numValid);
  for (int i = 0; i<randIndex; i++) start = start->down;
  return start;
}

void node::addColCell(cell *cellPtr) {
  if (start == nullptr) {
    start = cellPtr;
  } else {
    start->attachUD(cellPtr);
  }
  cellPtr->col = this;
  numValid++;
}

void node::addRowCell(cell *cellPtr) {
  if (start == nullptr) {
    start = cellPtr;
  } else {
    start->attachLR(cellPtr);
  }
  cellPtr->row = this;
  numValid++;
}

void node::attach(node *nodePtr) {
  nodePtr->prev = this;
  nodePtr->next = this->next;
  this->next->prev = nodePtr;
  this->next = nodePtr;
}

void node::remove() {
  this->next->prev = this->prev;
  this->prev->next = this->next;
}

void node::restore() {
  this->next->prev = this;
  this->prev->next = this;
}


grid::grid(int numRows, int numCols) : rows(numRows), cols(numCols) {
  for (auto &nodePtr : rows) nodePtr = new node;
  for (int i = 1; i < numRows; i++) {
    rows[i-1]->attach(rows[i]);
  }
  for (auto &nodePtr : cols) nodePtr = new node;
  for (int i = 1; i < numCols; i++) {
    cols[i-1]->attach(cols[i]);
  }
  headerNodePtr = cols[0];
}

grid::~grid() {
  for (node* nodePtr : rows) delete nodePtr;
  for (node* nodePtr : cols) delete nodePtr;
  for (cell* cellPtr : cells) delete cellPtr;
}

void grid::addCell(int row_i, int col_j) {
  cells.push_back(new cell);
  rows[row_i]->addRowCell(cells.back());
  cols[col_j]->addColCell(cells.back());
}

float grid::algorithmX() {
  node* selectedConstraintPtr = headerNodePtr->chooseNode();
  int numValid = selectedConstraintPtr->numValid;
  cell* selectedCandidatePtr = selectedConstraintPtr->chooseCandidate();
  //auto checkPoint = deleteStack.stackSize();
  int result = selectedCandidatePtr->selectRowAndRemove(deleteStack, headerNodePtr);
  if (result == -1) {
    return numValid * (this->algorithmX());
  } else {
    return result;
  }
}

void DeleteStack::push(cell *cellPtr) {
  cellPtr->remove();
  deletedCellStack.push(cellPtr);
}

void DeleteStack::push(node *nodePtr) {
  nodePtr->remove();
  deletedNodeStack.push(nodePtr);
}

std::pair<int, int> DeleteStack::stackSize() {
  return std::make_pair(deletedNodeStack.size(),deletedCellStack.size());
}

void DeleteStack::restoreStack(std::pair<int, int> checkPoint) {
  while (deletedNodeStack.size() > checkPoint.first) {
    deletedNodeStack.top()->restore();
    deletedNodeStack.pop();
  }
  while (deletedCellStack.size() > checkPoint.second) {
    deletedCellStack.top()->restore();
    deletedCellStack.pop();
  }
}

void DeleteStack::restoreStack() {
  while (!deletedNodeStack.empty()) {
    deletedNodeStack.top()->restore();
    deletedNodeStack.pop();
  }
  while (!deletedCellStack.empty()) {
    deletedCellStack.top()->restore();
    deletedCellStack.pop();
  }
}

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

void sudokuTest(grid &sudoku) {
  for (auto nodePtr: sudoku.cols) {
    assert(nodePtr->numValid == nodePtr->start->countUD());
  }
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

    int N = 4000;
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

    std::cout << "result: " <<  total/N << " standard deviation: " << stddev << std::endl;

    std::cout << "Time Elapsed: " << time_span.count();

}
