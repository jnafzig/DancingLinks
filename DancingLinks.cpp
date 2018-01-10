
#include "DancingLinks.h"

std::random_device seed;
std::mt19937 gen(seed());

int randi(int upperBound) {
  std::uniform_int_distribution<int> dist(0, upperBound-1);
  return dist(gen);
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

std::pair<std::stack<cell*>::size_type, std::stack<node*>::size_type> DeleteStack::stackSize() {
  return std::make_pair(deletedCellStack.size(),deletedNodeStack.size());
}

void DeleteStack::restoreStack(std::pair<std::stack<cell*>::size_type, std::stack<node*>::size_type> checkPoint) {
  while (deletedCellStack.size() > checkPoint.first) {
    deletedCellStack.top()->restore();
    deletedCellStack.pop();
  }
  while (deletedNodeStack.size() > checkPoint.second) {
    deletedNodeStack.top()->restore();
    deletedNodeStack.pop();
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
