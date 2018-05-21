#include <stdio.h>
#include <stdlib.h>
#include <stdbool>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "sudoku.h"
#include "array.h"

const int DIM = 9;

static const int EMPTY = 0;
static const char BLANK = '-';
static const int MIN = 1;
static const int MAX = 9;

static const int SUCCESS = 0;
static const int ERROR = -1;
static const int ERASE_EMPTY_CELL = -2;
static const int ERASE_FILLED_CELL = -3;
static const int ERROR_NEXT_CELL = -4;

static bool cell_empty(const int board[], int row, int col);

struct sudoku {
  int puzzle[DIM * DIM];
  int sol[DIM * DIM];
};

bool valid(const int a[], int row, int col, int num) {
  if ((num > MAX) || (num < MIN)) return false;
  int cor = row * DIM + col;
  for (int i = 0; i < DIM; ++i) {
    if (num == a[row * DIM + i] && (i != col)) {
      return false;
    }
  }

  for (int j = 0; j < DIM; ++j) {
    if (num == a[col + DIM * j] && (j != row)) {
      return false;
    }
  }

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      int index = ((col/3) * 3 + j) + 
        ((row/3) * 3 * DIM + (DIM * i));
      if (num == a[index] && (index != cor)) {
        return false;
      }
    }
  }
  return true;
}

int fill_cell(struct sudoku *s, int row, int col, int num) {
  assert(s);
  if (!cell_empty(s->puzzle, row, col)) {
    return ERROR;
  }
  else if (valid(s->sol, row, col, num)) {
    s->sol[row * MAX + col] = num;
    return SUCCESS;
  }
  return ERROR;
}

void choices_cell(const struct sudoku *s, int row, int col, 
                  int choices[], int *num_choices) {
  assert(s);
  int index = 0;
  for (int i = MIN; i <= MAX; ++i) {
    if (cell_empty(s->sol, row, col) 
        && (valid(s->sol, row, col, i))) {
      choices[index] = i;
      ++index;
    }
  }
  *num_choices = index;
}

bool solved_puzzle(const struct sudoku *s) {
  int index = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (cell_empty(s->sol, i, j) 
          || !valid(s->sol, i, j, s->sol[index])) {
        return false;
      }
      ++index;
      //trace_int(index);
    }
  }
  return true;
}


bool solve(struct sudoku *s) {
  if (solved_puzzle(s)) {
    return true;
  }
  int col = 0;
  int row = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (cell_empty(s->sol, i, j)) {
        row = i;
        col = j;
        break;
      } 
    }
  }
  int ar[9] = {0};
  int len = 0;
  choices_cell(s, row, col, ar, &len);
  for (int k = 0; k < len; ++k) {
    fill_cell(s, row, col, ar[k]);
    if (solve(s)) {
      return true;
    }
  }
  s->sol[row* MAX + col] = EMPTY;
  return false;
}


struct sudoku *read_sudoku(void) {
  struct sudoku *s = malloc(sizeof(struct sudoku));
  char c = 0;
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      scanf(" %c", &c);
      if (c == BLANK) {
        s->puzzle[row * DIM + col] = 0;
      } else {
        s->puzzle[row * DIM + col] = c - '0';
      }
    }
  }
  reset_sol(s);

  return s;
}

void sudoku_destroy(struct sudoku *s) {
  assert(s);
  free(s);
}

void print_sol(const struct sudoku *s) {
  assert(s);

  printf("\n");
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      int num = s->sol[row * DIM + col];
      if (num == EMPTY) {
        printf("%c", BLANK);
      } else {
        printf("%d", num);
      }  
    }
    printf("\n");
  }
  printf("\n");
}

void reset_sol(struct sudoku *s) {
  assert(s);

  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      s->sol[row * DIM + col] = s->puzzle[row * DIM + col];
    }
  }  
}

static bool cell_empty(const int board[], int row, int col) {
  assert(board);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  return board[row * DIM + col] == EMPTY;
}

int erase_cell(struct sudoku *s, int row, int col) {
  assert(s);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (cell_empty(s->sol, row, col)) {
    return ERASE_EMPTY_CELL;
  }
  if (!cell_empty(s->puzzle, row, col)) {
    return ERASE_FILLED_CELL;
  }
  s->sol[row * DIM + col] = EMPTY;
  return SUCCESS;
}


int next_cell(const struct sudoku *s, int *row, int *col) {
  assert(s);
  assert(row);
  assert(col);

  int choices[DIM];
  int num_choices = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (!cell_empty(s->sol, i, j)) continue;
      choices_cell(s, i, j, choices, &num_choices);
      if (num_choices == 1) {
        *row = i;
        *col = j;
        return SUCCESS;
      }
    }
  }
  return ERROR_NEXT_CELL;
}
