#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define n 5000*4950+4949

int main(){

    int num  ;
    char array[n];
    FILE *file;

    file = fopen("exam-data2.txt","r");

    if(!file){
        printf("Error");
        exit(1);
    }

    /*for(num = 0 ; num < n; num++){
        fscanf(file,"%c",&array[num]);
    }

    fclose(file);


    for(num = 0 ; num < n; num++){
        printf("%c",array[num]);
    }*/


    
}