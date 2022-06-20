#include <sys/wait.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>


#define DEVICE_ID 0x1d


#define 	INT16_MAX   0x7fff
#define 	INT16_MIN   (-INT16_MAX - 1)


//control register for setting standby or measurement mode for the chip
#define REG_POWER_CTL   0x2a


pthread_mutex_t mutex;


/**
 *Creates a file or writes into an existing file
 *STD, MIN, MAX, MEAN for the X, Y, Z direction
 *
 * 
 */
void writeStats(char axis, float std, int min, int max, float mean){

    //acquire the mutex lock
    pthread_mutex_lock(&mutex);
    

    //critical section
    FILE *fp;


    if((fp = fopen("output4.txt", "a+")) == NULL) {
        printf("Error! opening file");

       // Program exits if the file pointer returns NULL.
       exit(1);
    }

    fprintf(fp, "%c direction: \n", axis);
    fprintf(fp, "Mean: %f \n", mean);
    fprintf(fp, "STD: %f \n", std);
    fprintf(fp, "Min: %d \n", min);
    fprintf(fp, "Max: %d \n", max);
    fclose(fp);

    //release the mutex lock
    pthread_mutex_unlock(&mutex);

}



/**
 *int data[], the window of values
 *int size, the size of the window
 *Returns the largest Value in the window
 */

int maxNum(int data[], int size){
    int max = INT16_MIN;

    for(int i = 0; i < size; i++){
        if(max < data[i]){
            max = data[i];
        }
    }

    return max;
}

/**
 *int data[], the window of values
 *int size, the size of the window
 *Returns the smallest Value in the window
 */
int minNum(int data[], int size){
    int min = INT16_MAX;

    for(int i = 0; i < size; i++){
        if(min > data[i]){
            min = data[i];
        }
    }

    return min;
}


//Finds average of list of double values
float average(int var1[], int var2){

    float total = 0;
    for(int x = 0; x < var2; x++){
        total += (float) var1[x];
    }

    float size = (float) var2;
    return total / size;
}

//Generates Standard Deviation for a list of double values
float standardDeviation(int var1[], int var2){
    float Variance, Varsum, SD= 0;
    float avg = average(var1, var2);
    for(int i = 0; i < var2; i++){
        float Differ = ((float) var1[i] ) - avg;
        Varsum = Varsum + pow(Differ, 2);

    }
    Variance = Varsum / var2;

    return sqrt(Variance);
}

/**
 * Converts Most Significant Bits and Least Significant Bits of two's complement into a 32 bit integer
 */ 
int myMethod(int x_msb, int x_lsb){

    //combine msb and lsb into an 12 bit integer
    int dataX = ((x_msb << 8 | x_lsb)) >> 4;

    //add 20 zeros to make it a 32 bit integer
    int x_sign = dataX << 20;
    
    //
    x_sign = x_sign >> 20;



    dataX = dataX | x_sign;


    return x_sign;

}

/**
 * Computes the X axis values of a running average value, 
 * min and max in meters per second squared
 */

void *computeX(){
    int windowCount = 0;

    int window[20];

    //Setup I2C communication
    int fd = wiringPiI2CSetup(DEVICE_ID);
    if (fd == -1) {
        printf("Failed to init I2C communication.\n");
        return -1;
    }
    //Set I2C chip to measurement mode
    wiringPiI2CWriteReg8(fd, REG_POWER_CTL, 0x01);


    while(1){    
            //Read from register   
            int x_msb = wiringPiI2CReadReg8(fd, 1);
            int x_lsb = wiringPiI2CReadReg8(fd, 2);

            //store converted two's completement into 32 bit integers
            window[windowCount] =  ((myMethod(x_msb, x_lsb) *19.6) / 2048);

        if(windowCount < 20){
            windowCount++;
        }
        else{
            
            int x_min = minNum(window, 20);
            int x_max = maxNum(window, 20);
            float avg = average(window, 20);
            float std = standardDeviation(window, 20);

            //The values being written into a file
            printf("The Average X is %f m/s^2 \n", avg);
            printf(" Min X %d m/s^2  Max X %d m/s^2 \n", x_min, x_max);
            printf("The standard deviation of Y is %f m/s^2 \n", std);
            //reset window

            //Save the calculated values in a file
            writeStats('X', std, x_min, x_max, avg);
            windowCount = 0;
            //memset(dataSet, 0, sizeof(dataSet));
        }
    }

}




/**
 * Computes the Y axis values of a running average value, 
 * min and max in meters per second squared
 */
void *computeY(){
    int window[20];
    int windowCount = 0;

    //setup I2C communication
    int fd = wiringPiI2CSetup(DEVICE_ID);
    if (fd == -1) {
        printf("Failed to init I2C communication.\n");
        return -1;
    }
     //Set I2C chip to measurement mode
    wiringPiI2CWriteReg8(fd, REG_POWER_CTL, 0x01);

    while(1){    
            //Read from register   
            int y_msb = wiringPiI2CReadReg8(fd, 3);
            int y_lsb = wiringPiI2CReadReg8(fd, 4);

            //store converted two's completement into 32 bit integers
            window[windowCount] =  ((myMethod(y_msb, y_lsb) *19.6) / 2048) ;
        if(windowCount < 20){
            windowCount++;
        }
        else{
            
            int y_min = minNum(window, 20);
            int y_max = maxNum(window, 20);
            float avg = average(window, 20);
            float std = standardDeviation(window, 20);

            //The values being written into a file
            printf("The Average Y is %f m/s^2 \n", avg);
            printf(" Min Y %d m/s^2  Max Y %d m/s^2 \n", y_min, y_max);
            printf("The standard deviation of Y is %f m/s^2 \n", std);

            //Save the calculated values in a file
            writeStats('Y', std, y_min, y_max, avg);

            //reset window
            windowCount = 0;
            //memset(dataSet, 0, sizeof(dataSet));
        }
    }

}

/**
 * Computes the Z axis values of a running average value, 
 * min and max in meters per second squared
 */
void *computeZ(){
  int window[20];
    int windowCount = 0;

    //setup I2C communication
    int fd = wiringPiI2CSetup(DEVICE_ID);
    if (fd == -1) {
        printf("Failed to init I2C communication.\n");
        return -1;
    }

    //Set I2C chip to measurement mode
    wiringPiI2CWriteReg8(fd, REG_POWER_CTL, 0x01);

    while(1){    
            //Read from register   
            int z_msb = wiringPiI2CReadReg8(fd, 5);
            int z_lsb = wiringPiI2CReadReg8(fd, 6);

            //store converted two's completement into integers
            window[windowCount] =  ((myMethod(z_msb, z_lsb) *19.6) / 2048) ;
        if(windowCount < 20){
            windowCount++;
        }
        else{
            int z_min = minNum(window, 20);
            int z_max = maxNum(window, 20);
            float avg = average(window, 20);
            float std = standardDeviation(window, 20);

            //The values being written into a file
            printf("The Average Z is %f m/s^2 \n", avg);
            printf(" Min Z %d m/s^2  Max Z %d m/s^2 \n", z_min, z_max);
            printf("The standard deviation of Z is %f m/s^2 \n", std);
            //reset window

             //Save the calculated values in a file
            writeStats('Z', std, z_min, z_max, avg);


            //reset window
            windowCount = 0;
            //memset(dataSet, 0, sizeof(dataSet));
        }
    }
    
}





int main (int argc, char *argv[])  
{ 
    pthread_t       tid1,tid2,tid3;

    /* create the mutex lock */
    pthread_mutex_init(&mutex,NULL);

    //Each X, Y, Z axis values are calculated on different threads
    pthread_create(&tid1,NULL, computeX,NULL);
    pthread_create(&tid2,NULL, computeY,NULL);
    pthread_create(&tid3,NULL, computeZ,NULL);



    //wait for thread to exit
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);


    return 0; 
}