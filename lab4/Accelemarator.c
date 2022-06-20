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

int rollingAvgHelper(int *ptrArrNumbers, long *ptrSum, int pos, int len, int nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}




/**
 * Prints a Rolling Average over a value set of 20 
 */
void rollingAvg(char* format, int sample[]){
  // the size of this array represents how many numbers will be used
  // to calculate the average
  int arrNumbers[5] = {0};

  int pos = 0;
  int newAvg = 0;
  long sum = 0;
  int len = sizeof(arrNumbers) / sizeof(int);
  int count = sizeof(sample) / sizeof(int);

  for(int i = 0; i < count; i++){
    newAvg = rollingAvgHelper(arrNumbers, &sum, pos, 5, sample[i]);
    printf(format, newAvg);
    pos++;
    if (pos >= len){
      pos = 0;
    }
  }
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
    //int window[20];
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
            window[windowCount] =  ((myMethod(x_msb, x_lsb) *19.6) / 2048) ;
        if(windowCount < 20){
            windowCount++;
        }
        else{
            rollingAvg("\n The new average for X axis is %d m/s^2 \n", window);
            printf(" Min X %d m/s^2  Max X %d m/s^2 \n", minNum(window, 20), maxNum(window, 20));

            //reset window
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

            rollingAvg("\nThe new average for Y axis is %d m/s^2 \n", window);
            printf(" Min Y %d m/s^2  Max Y %d m/s^2 \n", minNum(window, 20), maxNum(window, 20));

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
            rollingAvg("\nThe new average for Z axis is %d m/s^2 \n", window);
            printf(" Min Z %d m/s^2  Max Z %d m/s^2 \n", minNum(window, 20), maxNum(window, 20));

            //reset window
            windowCount = 0;
            //memset(dataSet, 0, sizeof(dataSet));
        }
    }
    
}





int main (int argc, char *argv[])  
{ 
    pthread_t       tid1,tid2,tid3;


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