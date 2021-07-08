#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<mpi.h>
#include<omp.h>

int fileLen=20;

int main(int argc,char** argv)
{
	int rank,size,tid;
	int req_complete=0;
	int SquareLeft[fileLen][2];
	int SquareRight[fileLen][2];
	int SquareNo =0;
	int counter=0;
	int temp[2];
	int width[fileLen];
	int height[fileLen];
	int width_1,height_1,x_ax,y_ax,total;
	MPI_Status status;
	MPI_Request sendrq,recvrq;
	
	char *fileLine = (char*)malloc(fileLen*(fileLen+2)*sizeof(int));
	
	MPI_Init(NULL,NULL);
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	if(rank==0)
	{
		char recvLine[fileLen];
		int count =0;
		while (count !=(fileLen*2)-1)
		{
			MPI_Irecv(&recvLine,fileLen,MPI_CHAR,1,0,MPI_COMM_WORLD,&recvrq);
		 	MPI_Test(&recvrq,&req_complete,&status);
		 	
		 	if(!req_complete)
		 	{
		 		MPI_Wait(&recvrq,&status);
		 	}
		 	*strcpy((fileLine + count*(fileLen+2)),recvLine);
		}
	}
	
	if (rank==1)
	{
		int count2=0;
		char const* const fileName="exam-data2.txt";
		FILE* file= fopen(fileName,"r");
		char line[fileLen+3];
		
		while(!feof(file))
		{
			fgets(line,fileLen+2,file);
			
			MPI_Isend(&line, fileLen,MPI_CHAR,0,0,MPI_COMM_WORLD,&sendrq);
			MPI_Wait(&sendrq,&status);
			
			count2++;
		}
		
		fclose(file);
	}
	
	for(int k=0;k<fileLen*2;k+=2)
	{
		int data = fileLen/size;
		char inbuff[fileLen];
		char recvbuff[data];
		
		*strcpy(inbuff,(fileLine + counter*(fileLen+2)));
		
		MPI_Scatter(inbuff,data,MPI_CHAR,recvbuff,data,MPI_CHAR,0,MPI_COMM_WORLD);
		total=0;
		
		for(int i=0;i<data;i++)
		{
			total +=(int)(recvbuff[i]);
		}
		if(rank==0)
		{
			int ssum=total;
			int recv2;
			
			for (int i=1;i<size;i++)
			{
				MPI_Irecv(&recv2,1,MPI_INT,i,MPI_ANY_TAG,MPI_COMM_WORLD,&recvrq);
				MPI_Test(&recvrq,&req_complete,&status);
				
				if(!req_complete)
				{
					MPI_Wait(&recvrq,&status);
				}
				ssum+=recv2;
			}
			if(rank!=0)
			{
				MPI_Isend(&total,1,MPI_INT,0,0,MPI_COMM_WORLD,&sendrq);
				MPI_Wait(&sendrq,&status);
					
				if(total>0)
				{
					for(int h=0;h<fileLen;h++)
					{
						if(*(fileLine + k*(fileLen+2) + h ) == '1')
						{
							int already=0;
							for(int count=0;count<SquareNo;count++)
							{
								if((k/2)+1 >= SquareLeft[count][0]
									&& (k/2)+1 <= SquareRight[count][0]
									&& h+1 >= SquareLeft[count][1]
									&& h+1 <= SquareRight[count][1])
									{
										already=1;
									}
							}
							if(already==1)
							{
								continue;
							}
							width_1=0;
							height_1 =0;
										
							y_ax=k;
							x_ax =h;
										
							SquareLeft[SquareNo][0] = (k/2) + 1;
							SquareLeft[SquareNo][1] = h+1;
							#pragma omp parallel num_threads(20) private(tid)
							{
								tid =omp_get_thread_num();
										
								if(tid==0)
								{
									while(*(fileLine + k*(fileLen+2) + x_ax) == '1')
									{
										width_1++;
										x_ax++;
									}
								}
								if (tid ==1)
								{
									while (*(fileLine + y_ax*(fileLen+2) + h) == '1')
									{
										height_1++;
										y_ax=y_ax+2;
									}
								}
							}
							
							height[SquareNo] = height_1;
							width[SquareNo] = width_1;
									
							SquareRight[SquareNo][0] =k/2 + height_1;
							SquareRight[SquareNo][1] = h + width_1;
										
							SquareNo++;
						}
						}
				
				}
			}
			

		}//
		if(rank!=0)
		{
			MPI_Isend(&total,1,MPI_INT,0,0,MPI_COMM_WORLD,&sendrq);
			MPI_Wait(&sendrq,&status);
		}
		
	}
	
	
		
	
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	for(int p=0;p<SquareNo;++p)
	{
		printf("Square No. : %d\n",p+1);
		printf("Square coordinate [top-left corner] \t: %d,%d\n",SquareLeft[p][1],SquareLeft[p][0]);
		printf("Square coordinate [bottom-right corner] \t: %d,%d\n",SquareRight[p][1],SquareRight[p][0]);
		printf("\n");
	}
	
	MPI_Finalize();	
	return 0;	
	
}
