
#ifndef DANCINGLINKS_H
#define DANCINGLINKS_H

#include <iostream>
#include <vector>
#include <random>
#include <climits>
#include <stack>

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
  std::pair<std::stack<cell*>::size_type, std::stack<node*>::size_type> stackSize();
  void restoreStack(std::pair<std::stack<cell*>::size_type, std::stack<node*>::size_type>);
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
};

struct grid {
  std::vector<node*> rows;
  std::vector<node*> cols;
  std::vector<cell*> cells;
  node* headerNodePtr;
  DeleteStack deleteStack;
  grid(int numRows, int numCols);
  ~grid();
  void addCell(int row, int col);
  float algorithmX();
};

#endif
