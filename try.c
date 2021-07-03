#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

//--some helper functions and structs--
typedef struct Line_s {
    long x0;
    long nx;
    long y0;
} Line;

typedef struct LineVect_s {
  Line *lines;
  long size;
  long capacity;
} LineVect;

LineVect* newLineVect(void) {
    LineVect *vect = malloc(sizeof(LineVect));
    vect->lines = malloc(sizeof(Line));
    vect->size = 0;
    vect->capacity = 1;
    return vect;
}

void deleteLineVect(LineVect* self) {
    free(self->lines);
    free(self);
}

void LineVect_pushBack(LineVect* self, Line line) {
    self->size++;
    if (self->size > self->capacity) {
        self->capacity *= 2;
        self->lines = realloc(self->lines, self->capacity * sizeof(Line));
    }
    self->lines[self->size - 1] = line;
}

typedef struct Block_s {
    long x0, y0;
    long nx, ny;
    char val;
} Block;

typedef struct BlockVect_s {
   Block *blocks;
   long size;
   long capacity;
} BlockVect;

BlockVect* newBlockVect(void) {
    BlockVect *vect = malloc(sizeof(BlockVect));
    vect->blocks = malloc(sizeof(Block));
    vect->size = 0;
    vect->capacity = 1;
    return vect;
}

void deleteBlockVect(BlockVect* self) {
    free(self->blocks);
    free(self);
}

void BlockVect_pushBack(BlockVect* self, Block block) {
    self->size++;
    if (self->size > self->capacity) {
        self->capacity *= 2;
        self->blocks = realloc(self->blocks, self->capacity * sizeof(Block));
    }
    self->blocks[self->size - 1] = block;
}


LineVect** find_lines(char* data, long nx, long ny) {
    LineVect **slices = malloc(ny * sizeof(LineVect*));
    //essentially each y-slice is independent here
#pragma omp parallel for
    for (long y = 0; y < ny; y++) {
        slices[y] = newLineVect();
        char prev_val = 0;
        long xstart = 0;
        for (long x = 0; x < nx; x++) {
            char val = data[nx*y + x];
            if (val == 1 && prev_val == 0) {
                xstart = x;
            } else if (val == 0 && prev_val == 1) {
                Line line;
                line.y0 = -1;
                line.x0 = xstart;
                line.nx = x - xstart;
//              printf("Found line at y=%d from x=%d to %d\n", y, xstart, x-1); 
                LineVect_pushBack(slices[y], line);
            }
            
            if (val == 1 && x == nx-1) {
                Line line;
                line.y0 = -1;
                line.x0 = xstart;
                line.nx = x - xstart + 1;
//              printf("Found line at y=%d from x=%d to %d\n", y, xstart, x);
                LineVect_pushBack(slices[y], line);
            }
            prev_val = val;
        }
    }
    return slices;
}

BlockVect* find_blocks(LineVect** slices, long ny) {
    BlockVect* blocks = newBlockVect();
    //for each line
    for (long y = 0; y < ny; y++) {
        LineVect* slice = slices[y];
        long l2 = 0;
        for (long l = 0; l < slice->size; l++) {
            Line *line = slice->lines + l;
            if (line->y0 == -1) {
                line->y0 = y;
            }
            char match = 0;
            //check next slice if there is any
            if (y != ny-1) {
                //check all the remaining lines in the next slice
                if (l2 < slices[y+1]->size) {
                    Line *line2 = slices[y+1]->lines + l2;
                    //note that we exploit the sorted nature of the lines (x0 must increase with l2)
                    for (; l2 < slices[y+1]->size && line2->x0 < line->x0; l2++) {
                        line2 = slices[y+1]->lines + l2;
                    }
                    //matching line2 found -> mark it as same block (y0 cascades)
                    if (line->x0 == line2->x0 && line->nx == line2->nx) {
                        match = 1;
                        line2->y0 = line->y0;
                    }
                }
            }
            //current line didn't find a matching line hence store it (and all the previously collected lines) as a block
            if (match == 0) {
                Block block;
                block.x0 = line->x0;
                block.nx = line->nx;
                block.y0 = line->y0;
                block.ny = y - line->y0 + 1;
                BlockVect_pushBack(blocks, block);
            }
        }
    }
    return blocks;
}

#define Nx 30000
#define Ny 30000

char * write_blocks(const BlockVect* blocks) {
    char * data = calloc(Ny, Nx*sizeof(char));
    for (long i = 0; i < blocks->size; i++) {
        Block *block = blocks->blocks + i;
        for (long y = block->y0; y < block->y0 + block->ny; y++) {
            for (long x = block->x0; x < block->x0 + block->nx; x++) {
                data[Nx*y + x] = 1;
            }
        }
    }
    return data;
}

int main() {
    struct timeval t0;
    gettimeofday(&t0, NULL);
    
 // char data[Ny][Nx] = {
 //     {0, 0, 0, 0, 1, 1, 1, 0},
 //    {0, 0, 0, 0, 1, 1, 1, 0},
 //     {0, 1, 1, 1, 0, 0, 0, 1},
 //     {0, 1, 1, 1, 0, 0, 0, 1},
 //     {0, 1, 1, 1, 0, 1, 1, 0},
 //     {0, 0, 0, 0, 0, 1, 1, 0},
 //     {0, 1, 0, 1, 1, 1, 1, 1},
//    {0, 0, 0, 1, 1, 1, 1, 1}
 // };
    
    srand(t0.tv_usec);
    char * input = calloc(Ny, Nx*sizeof(char));
    printf("Filling...\n");
    for (long y = 0; y < Ny; y++) {
        for (long x = 0; x < Nx; x++) {
            input[Nx*y + x] = rand() % 10 != 0; //data[y][x];
        }
    }
    printf("Computing...\n");
    struct timeval t;
    struct timeval t_new;
    gettimeofday(&t, NULL);
    
    LineVect** slices = find_lines(input, Nx, Ny);
    
    gettimeofday(&t_new, NULL);
    printf("Finding lines took %lu milliseconds\n", (t_new.tv_sec - t.tv_sec)*1000 + (t_new.tv_usec -t.tv_usec) / 1000);
    
    gettimeofday(&t, NULL);
    
    BlockVect* blocks = find_blocks(slices, Ny);
    
    gettimeofday(&t_new, NULL);
    printf("Finding blocks took %lu milliseconds\n", (t_new.tv_sec - t.tv_sec)*1000 + (t_new.tv_usec -t.tv_usec) / 1000);
    
    long n_lines = 0;
    for (long y = 0; y < Ny; y++) {
        n_lines += slices[y]->size;
        deleteLineVect(slices[y]);
    }
    free(slices);
    
    printf("Done computation of %ld lines and %ld blocks\n", n_lines, blocks->size);
    
    char * output = write_blocks(blocks);
    deleteBlockVect(blocks);
    
    char pass = 1;
    for (long y = 0; y < Ny; y++) {
        for (long x = 0; x < Nx; x++) {
            if (input[Nx*y + x] != output[Nx*y + x]) {
                printf("ERROR at x=%ld and y=%ld!\n", x, y);
                pass = 0;
            }
        }
    }
    printf("Plausibility check %s\n", pass ? "PASSED" : "FAILED");
    
    //free all the rest
    free(input);
    free(output);
    gettimeofday(&t_new, NULL);
    printf("Whole run took %.3lf seconds\n", (t_new.tv_sec - t0.tv_sec) + (t_new.tv_usec -t0.tv_usec) * 1e-6);
}