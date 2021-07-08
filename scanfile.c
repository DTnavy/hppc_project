#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<omp.h>


int main(int argc,char** argv){

    /*int num  ;
    char array[n];
    FILE *file;

    file = fopen("exam-data2.txt","r");

    if(!file){
        printf("Error");
        exit(1);
    }

    for(num = 0 ; num < n; num++){
        fscanf(file,"%c",&array[num]);
    }

    fclose(file);


    for(num = 0 ; num < n; num++){
        printf("%c",array[num]);
    }*/
    long thread_count=strtol(argv[1],NULL,10);
    printf("%d\n",thread_count);
    FILE *fp=fopen("data.txt","r");
    fseek(fp, 0L, SEEK_END);
    long SIZE = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char *text = (char*) malloc(sizeof(char) * SIZE);
 
    if(!  fp)
    {
    printf("can't open file\n");
    return -1;
    }
    # pragma omp parallel for num_threads(thread_count)
 
    for (int i=0;   i<thread_count;  i++)
    {
    fseek(fp,i*SIZE/thread_count,SEEK_SET);
    
    long local_test=i*(SIZE/thread_count);
 
    for(local_test;  local_test<((i+1)*SIZE/thread_count);  local_test++)
    {
         fscanf(fp,"%c",&text[local_test]);
    }
 }


    
}