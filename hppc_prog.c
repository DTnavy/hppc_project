#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mpi.h>
#include<omp.h>

int fileLen = 20;

int main(int argc,char** argv){
    int rank,size,tid;
    int req_complete = 0;
    int recLeft[][2];
    int recRight[][2];
    int recNo = 0;
    int counter = 0;
    int temp[2];
    int width[];
    int height[];
    int width_1, height_1,x_ax,y_ax,total;
    MPI_Status status;
    MPI_Request sendrq,recvrq;

    char *fileLine = (char *)malloc(fileLen*(fileLen+2)*sizeof(int));

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD.&size);

    if(rank == 0){
        char recvLine[fileLen];
        int count = 0;

        while(count !=(fileLen*2)-1){
            MPI_Irecv(&recvLine,fileLen,MPI_CHAR,1,0,MPI_COMM_WORLD,&recvrq);
            MPI_Test(&recvrq,&req_complete,&status);
        }
    }
}