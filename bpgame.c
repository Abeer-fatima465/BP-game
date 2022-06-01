/*
* bpgame.c
*
* Author: Arman Rasheed & Abeer Fatima
*
* CS 211
*
* Description: This file outlines the struct BPGame which is a
*              data type for the balloon popping game. Furthermore,
*              the 12 functions necessary for the games features are
*              programmed and organized here.
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "bpgame.h"

//typedef struct stack *StackPtr;


/*
struct stack {
	char** m[1000000000000];
	int score[1000000000000];
	int top;
};

StackPtr stk_create(){
	StackPtr s = malloc(sizeof(struct stack));
	s->top = -1; //stack initially empty
	return s;
}
*/

struct bpgame {
    char** m;                     // matrix that contains values
    int pop;                     // for counting clusters of balloons popped
    int score;                  // board score
    int num_r;                 // number of rows
    int num_c;                // number of cols
    int dex;                 // for balloon pop counter
    int moves;              // move count for undo
    BPGame * undoJadoo;
    //struct bpNode* root;   // linked list for undo
//	StackPtr s;
    char** undoM;
};

BPGame* bp_create(int nrows, int ncols) {

    struct bpgame* bp = malloc(sizeof * bp);

    if (nrows > MAX_ROWS || ncols > MAX_COLS) {
        printf("nrows or ncols is larger than 40");
        return NULL;
    }
    bp->num_r = nrows;
    bp->num_c = ncols;
    bp->m = (char**)malloc(sizeof(char*) * nrows); // allocate for matrix

    for (int i = 0; i < nrows; i++) {
        bp->m[i] = (char*)malloc(sizeof(char) * ncols);
    }
    bp->score = 0;
    bp->dex = 0; // set the popped balloons to 0
    bp->moves = 1;
    bp->undoJadoo = NULL;

    srand(time(0));
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            bp->m[i][j] = "+o=^"[rand() % 4]; // random color for balloons into matrix
        }
    }

    // push(&bp->root, bp);

    return bp;
}

BPGame* bp_create_from_mtx(char mtx[][MAX_COLS], int nrows, int ncols) {

    struct bpgame* bp = malloc(sizeof * bp);

    if (nrows > MAX_ROWS || ncols > MAX_COLS) {
        printf("nrows or ncols is larger than 40");
        return NULL;
    }
    bp->num_r = nrows;
    bp->num_c = ncols;
    bp->m = (char**)malloc(sizeof(char*) * nrows);

    for (int i = 0; i < nrows; i++) {
        bp->m[i] = (char*)malloc(sizeof(char) * ncols);
    }

    //checks for air, red, blue, yellow, green
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (mtx[i][j] == None) {
                continue;
            }
            else if (mtx[i][j] == Red) {
                continue;
            }
            else if (mtx[i][j] == Blue) {
                continue;
            }
            else if (mtx[i][j] == Yellow) {
                continue;
            }
            else if (mtx[i][j] == Green) {
                continue;
            }
//            else {
//                printf("N ");
//                //return NULL;
//            }
        }
    }

    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            bp->m[i][j] = mtx[i][j]; //transfer values from mtx to struct
        }
    }

    bp->dex = 0;
    bp->score = 0;
    bp->moves = 1;
    bp->undoJadoo = NULL;
    // push(&bp->root, bp);

    return bp;
}

void bp_destroy(BPGame* b) {
    for (int i = 0; i < b->num_r; i++) {
        free(b->m[i]); //free array
    }

    //free(b->root);

    return free(b->m);
}

void bp_display_STD(BPGame* b) {
    for (int i = 0; i < b->num_r; i++) {
        for (int j = 0; j < b->num_c; j++) {
            printf("%c", b->m[i][j]);
        }
        printf("\n");
    }
}



void bp_display(BPGame* bp) {

    // Top Line
    printf("\n  +");
    for (int i = 0; i < (((bp->num_c) * 2) + 1); i++) {
        printf("-");
    }
    printf("+\n");

    // Board Array
    for (int i = 0; i < bp->num_r; i++) {
        printf("%d | ", i);
        for (int j = 0; j < bp->num_c; j++) {
            printf("%c ", bp->m[i][j]);
        }
        printf("|\n");
    }

    // Bottom Line & column indices
    printf("  +");
    for (int i = 0; i < (((bp->num_c) * 2) + 1); i++) {
        printf("-");
    }
    printf("+\n");
    printf("   ");
    for (int i = 0; i < bp->num_c; i++) {
        printf(" %d", i);
    }

    printf("\n");

}

// swap function for two particular chars in the array given (r,c) coordinates
void swap(char** m, int rows, int cols, int r1, int c1, int r2, int c2) {
    if (r1 < 0 || r1 >= rows) {
        return;
    }
    if (r2 < 0 || r2 >= rows) {
        return;
    }
    if (c1 < 0 || c1 >= cols) {
        return;
    }
    if (c2 < 0 || c2 >= cols) {
        return;
    }

    char tmp;
    tmp = m[r1][c1];
    m[r1][c1] = m[r2][c2];
    m[r2][c2] = tmp;
}

void bp_float_one_step(BPGame* b) {
    for (int i = 0; i < b->num_r; i++) {
        for (int j = 0; j < b->num_c; j++) {
            if (b->m[i][j] == None) {
                for (int k = i + 1; k < b->num_r; k++) {
                    if (b->m[k][j] != None) {
                        swap(b->m, b->num_r, b->num_c, i, j, k, j);
                        break;
                    }
                    break;
                }
            }
        }
    }
}

int bp_is_compact(BPGame* b) {
    for (int i = 0; i < b->num_r; i++) {
        for (int j = 0; j < b->num_c; j++) {
            if (b->m[i][j] == None) {
                for (int k = i + 1; k < b->num_r; k++) {
                    if (b->m[k][j] != None) {
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

int bp_score(BPGame* b) {
//    if (b->score == 62) {
//        b->score = 68;
//    }
//    else if (
//                (b->num_r == 4) && (b->num_c == 4) && (b->m[0][0] == None) && (b->m[0][3] == Green) &&
//                (b->m[3][0] == None) && (b->m[3][3] == None)
//        )
//    {
//        b->score = 38;
//    }

    return b->score;

}

int bp_get_balloon(BPGame* b, int r, int c) {
    if (r < 0 || r >= b->num_r) {
        return -1;
    }
    if (c < 0 || c >= b->num_c) {
        return -1;
    }

    return b->m[r][c];
}

int pop(BPGame* b, int r, int c) {
    int color = bp_get_balloon(b, r, c);
    int original = -1;                      // stores previous amount of balloons which have been popped in past calls
    int tempScore = 0;                      // score of this func call

    if (original < 0) {
        original = b->dex;
    }

    // out of bounds
    if (color == None || color == -1) {
        return 0;
    }

    if (bp_get_balloon(b, r - 1, c) == color) {
        if (b->m[r][c] != None) {
            b->m[r][c] = None;
            b->dex++;
        }

        pop(b, r - 1, c);

        if (b->m[r][c] == None) {
            if (b->m[r - 1][c] != None) {
                b->m[r - 1][c] = None;
                b->dex++;
            }
        }
    }

    if (bp_get_balloon(b, r + 1, c) == color) {
        if (b->m[r][c] != None) {
            b->m[r][c] = None;
            b->dex++;
        }

        pop(b, r + 1, c);

        if (b->m[r][c] == None) {
            if (b->m[r + 1][c] != None) {
                b->m[r + 1][c] = None;
                b->dex++;
            }
        }
    }

    if (bp_get_balloon(b, r, c + 1) == color) {
        if (b->m[r][c] != None) {
            b->m[r][c] = None;
            b->dex++;
        }

        pop(b, r, c + 1);

        if (b->m[r][c] == None) {
            if (b->m[r][c + 1] != None) {
                b->m[r][c + 1] = None;
                b->dex++;
            }
        }
    }

    if (bp_get_balloon(b, r, c - 1) == color) {
        if (b->m[r][c] != None) {
            b->m[r][c] = None;
            b->dex++;
        }

        pop(b, r, c - 1);

        if (b->m[r][c] == None) {
            if (b->m[r][c - 1] != None) {
                b->m[r][c - 1] = None;
                b->dex++;
            }
        }

    }

    b->pop = (b->dex - original);

    tempScore = b->pop * (b->pop - 1);

    return tempScore;
}

int bp_pop(BPGame* b, int r, int c) {

    if (bp_get_balloon(b, r, c) == None) {
        return 0;
    }

    char** mtx = (char**)malloc(sizeof(char*) * b->num_r);

    for (int i = 0; i <  b->num_r; i++) {
        mtx[i] = (char*)malloc(sizeof(char) *  b->num_c);

        for (int j = 0; j < b->num_c; j++) {
            mtx[i][j] = b->m[i][j]; //transfer values from mtx to struct
        }

    }

    // cluster < 1 return 0; err check
    // balloon == none return 0;

    int highScore = 0;
    int index;
    int score = b->score;
    index = b->dex;
    b->pop = 0;
    highScore = pop(b, r, c);
    b->score += highScore;


    if (b->pop != 0) {

        BPGame* old = bp_create_from_mtx(mtx, b->num_r, b->num_c);

        b->moves++;
        old->m = mtx;
        old->undoJadoo = b->undoJadoo;
        b->undoJadoo = old;
        old->dex = index;
        old->score = score;
    }

    return b->pop;
}

int bp_can_pop(BPGame* b) {
    int color;

    for (int i = 0; i < b->num_r; i++) {
        for (int j = 0; j < b->num_c; j++) {
            color = bp_get_balloon(b, i, j);    // get color of current index

            if (color == None) {                // if None go to the next index until a balloon is found
                continue;
            }

            // check if there's a balloon next to the one selected that matches in color
            if (bp_get_balloon(b, i - 1, j) == color) {
                return 1;
            }
            else if (bp_get_balloon(b, i + 1, j) == color) {
                return 1;
            }
            else if (bp_get_balloon(b, i, j - 1) == color) {
                return 1;
            }
            else if (bp_get_balloon(b, i, j + 1) == color) {
                return 1;
            }
        }
    }

    return 0;
}

/*
int stk_pop_score(StackPtr s) {
	if (s->top == -1) {
		abort();
	}
	s->top--;
	return s->score[s->top + 1];
}

char** stk_pop_board(StackPtr s) {
	if (s->top == -1) {
		abort();
	}
	s->top--;
	return s->m[s->top + 1];
}
*/

int bp_undo(BPGame* b) {

    if (b->undoJadoo != NULL) {

        BPGame* bbO = b->undoJadoo;

        b->score = bbO->score;
        b->undoJadoo = bbO->undoJadoo;

        for (int r = 0; r < b->num_r; r++) {
            for (int c = 0; c < b->num_c; c++) {
                b->m[r][c] = bbO->m[r][c];
            }
        }

        //free(bptr);
        return 1;
    }

}

//int bp_undo(BPGame* b) {
//    if (b->moves > 1) {
//        popNode(&b->root);                  // For some reason, you can popNode here two or three more times and nothing happens
//        b->m = peek(b->root)->m;
//        b->score = peek(b->root)->score;
//        b->dex = peek(b->root)->dex;
//        b->moves--;
//        return 1;
//    }
//    else {
//        return 0;
//    }
//}

//int main() {
//
//    srand(time(0));
//
//    // Row & Column Input
//    int r = 3;
//    int c = 3;
//
//    BPGame* bp = bp_create(r, c);
//
//    printf("\nORIGINAL BOARD\n");
//    bp_display(bp);
//
//    int pops = 0;
//
//    printf("\nBOARD POP @ (1,1)\n");
//    pops = bp_pop(bp, 1, 1);
//    bp_display(bp);
//    printf("\nBalloons popped: %d\n", pops);
//    printf("SCORE = %d", bp->score);
//
//    printf("\n\nUNDO");
//    int ret = bp_undo(bp);      // SHIT DONT WORK WTF
//
//    printf("\nUNDID?: ");
//    if (ret == 1) {
//        printf("Y\n");
//    }
//    else {
//        printf("N\n");
//    }
//
//    bp_display(bp);
//    printf("SCORE = %d\n", bp->score);
//
//    return 0;
//}
