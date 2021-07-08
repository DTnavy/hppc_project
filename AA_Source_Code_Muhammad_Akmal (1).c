#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mpi.h>
#include<omp.h>

int lengthFile = 20;	//declare the length of file (each line length)

int main(int argc, char** argv)
{
	int rank, size, tid;
	MPI_Request send_req, recv_req;
  	MPI_Status status;
  	int request_complete = 0;

  	char *line2 = (char *)malloc(lengthFile * (lengthFile+2) * sizeof(int));	//Dynamic Array store the lines

	MPI_Init(NULL,NULL);	//Initialize the MPI
	MPI_Comm_size(MPI_COMM_WORLD,&size);	//Determines the number of process in communicator
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);	//Determines the rank of the calling process in the communicator

	if(rank == 0){	//rank 0 does
		char recvLine[lengthFile];
		int counter=0;

		while(counter != (lengthFile*2)-1)
		{
			MPI_Irecv(&recvLine,lengthFile,MPI_CHAR,1,0,MPI_COMM_WORLD,&recv_req);	//non blocking recieve
			MPI_Test(&recv_req, &request_complete, &status);	//test the recv req (check already recv)
			
			if(!request_complete){	//if not yet recv
				MPI_Wait(&recv_req,&status);	//wait until recv
			}
			*strcpy((line2 + counter*(lengthFile+2)),recvLine);
			counter++;
		}
	}
	if (rank == 1)
	{
		int count=0;

	    char const* const fileName = "Data.txt"; 
    	FILE* file = fopen(fileName, "r");
    	char line[lengthFile+3];

		while (!feof(file)) { 
    	   	fgets(line, lengthFile+2,file);
    	   	MPI_Isend(&line,lengthFile,MPI_CHAR,0,0,MPI_COMM_WORLD,&send_req);	//begins nonblocking send
    	   	MPI_Wait(&send_req,&status);	//wait until already send
    	   	count++;
	    }
    	fclose(file);
	}

	int left_coor[lengthFile][2]; //[0] = y-axis(height) , [1] = x-axis(width)
    int right_coor[lengthFile][2]; //[0] = y-axis(height) , [1] = x-axis(width)
    int left = 0;
    int num_Rect = 0;
    int temp[2];
    int width[lengthFile];
    int hight[lengthFile];
    int tempwidth;
    int tempheight;
    int xaxis;
    int yaxis;
    int sum;
    int counter = 0;

	for (int y = 0; y < lengthFile*2; y+=2)
	{
		char inputBuffer[lengthFile];
		int numData = lengthFile/size;
		char recieveBufffer[numData];
		*strcpy(inputBuffer,(line2 + counter*(lengthFile+2)));
		
		MPI_Scatter(inputBuffer,numData,MPI_CHAR,recieveBufffer,numData,MPI_CHAR,0,MPI_COMM_WORLD);		//Scatter the data (decomposition)

		sum=0;

		for(int i=0; i<numData;i++){
			sum += (int)(recieveBufffer[i]);
		}

		if(rank == 0){
			int masterSum = sum;
			int recieve;	//variable recieving

			for(int i=1; i<size; i++){
				MPI_Irecv(&recieve, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_req);	//non blocking recieve
				MPI_Test(&recv_req, &request_complete, &status);	//test the recv req (check already recv)
				
				if(!request_complete){	//if not yet recv
					MPI_Wait(&recv_req,&status);	//wait until recv
				}
				
				masterSum+=recieve;
			}

			if(sum > 0){

            	for (int x = 0; x < lengthFile; x++)
            	{
                	if (*(line2 + y*(lengthFile+2) + x) == '1')
                	{
                    	int already = 0;
                    	for (int count = 0; count < num_Rect; count++)
                    	{
                        	if( x+1 >= left_coor[count][1] && x+1 <= right_coor[count][1] && (y/2)+1 >= left_coor[count][0] && (y/2)+1 <= right_coor[count][0]){
                            	already = 1;
                        	}
                    	}

                    	if(already == 1){
                        	continue;
                    	}

                    	left_coor[num_Rect][0] = (y/2)+1;
                    	left_coor[num_Rect][1] = x+1;
                    	tempwidth = 0;
                    	tempheight = 0;
                    	xaxis = x;
                    	yaxis = y;

        				#pragma omp parallel private(tid) num_threads(2) // 2 threads created which will have each pid and both threads will execute simultaneously
        				{
        					tid = omp_get_thread_num();	//returns a unique thread number of each thread
                    		if(omp_get_thread_num() == 0){
                    			while(*(line2 + y*(lengthFile+2) + xaxis) == '1'){
	                        		tempwidth++;
	                        		xaxis++;
	                    		}
                    		}
                    		if(omp_get_thread_num() == 1){
                    			while(*(line2 + yaxis*(lengthFile+2) + x) == '1'){
	                        		tempheight++;
	                        		yaxis+=2;
	                    		}
                    		}
                    	}
        
                    	hight[num_Rect] = tempheight;
                    	width[num_Rect] = tempwidth;
                    	right_coor[num_Rect][0] = y/2+tempheight;
                    	right_coor[num_Rect][1] = x+tempwidth;
                    	num_Rect++;
                	}
            	}     
        	}
		}
		if(rank != 0){
			MPI_Isend(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,&send_req);	//begins nonblocking send
			MPI_Wait(&send_req, &status);	//wait until send req successful
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);	//Blocks the caller until all processes in the communicator have called it
	
	for (int i = 0; i < num_Rect; ++i)
	{
	    printf("Number Rect => %d\n",i+1);
	    printf("Left coordindate : (%d,%d)\n",left_coor[i][1],left_coor[i][0]); 
	    printf("Right coordindate : (%d,%d)\n",right_coor[i][1],right_coor[i][0]);
	    printf("\n");
	}
	MPI_Finalize();
	return 0;
}