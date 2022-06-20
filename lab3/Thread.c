#include <sys/wait.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <math.h>

/**
 * Moses Mbugua
 * Operating Systems
 * Lab 3
 * Dr. Caley
 */


int pipefd[2];


//Finds average of list of double values
double average(double var1[], int var2){

    double total = 0;
    for(int x = 0; x < var2; x++){
        total += var1[x];
    }


    return total / var2;
}

//Finds largest double of list of numbers
double maxNum(double var1[], int var2) {

    double max = DBL_MIN;
    for(int x = 0; x < var2; x++){
        if(max < var1[x]){
            max = var1[x];
        }
    }

    return max;
}


//Finds the smallest number in a list double values
double minNum(double var1[], int var2){

    double min = DBL_MAX;
    for(int x =0; x < var2; x++){
        if(min > var1[x]){
            min = var1[x];
        }
    }

    return min;
}

//Generates Standard Deviation for a list of double values
double standardDeviation(double var1[], int var2){
    double Variance, Varsum, SD= 0;
    double avg = average(var1, var2);
    for(int i = 0; i < var2; i++){
        double Differ = var1[i] - avg;
        Varsum = Varsum + pow(Differ, 2);

    }
    Variance = Varsum / var2;

    return sqrt(Variance);
}

///Output Average, Min, Max and Standard Deviation of list of random numbers
void outPut(double dataset[], int var1) {

    printf("Average %f \n", average(dataset, var1));
    printf("Max num %f \n", maxNum(dataset, var1));
    printf("Min num %f \n", minNum(dataset, var1));
    printf("Standard Deviation %f \n", standardDeviation(dataset, var1));
}


void *reader() {
     double dataTrial[10];

        double trial;
        //read random numbers generate by child through pipe
        for(int i = 0; i < 10; i++){
            read(pipefd[0], &trial, sizeof(trial));
            dataTrial[i] = trial;

            //check to make sure an actual number is passed through
            printf("%f \n", dataTrial[i]);
        }

        //Print out Avg, Min, Max, Std
        outPut(dataTrial, 10);
    
}

void *writer(){

     for(int i = 0; i < 10; i++){
            //random number
            double trial = (double)rand();
            //write to pipe
            write(pipefd[1], &trial, sizeof(trial));
    }

}

int main (int argc, char *argv[])  
{ 
    // Initialization, should only be called once.
    srand(time(NULL));   

    pid_t cpid;

    pthread_t       tid1,tid2;


    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //Creaate the threads
    pthread_create(&tid1,NULL,reader,NULL);
    pthread_create(&tid2,NULL,writer,NULL);
   

    //wait for thread to exit
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    return 0; 
}