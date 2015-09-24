#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<cstdlib>
#include<iostream>

#define MAX_DURATION 500

using namespace std;

int main(int argc, char** argv)
{

using namespace std;

unsigned long i=0, j=0;
unsigned long max = 1000*1000;
unsigned long jmax = 1024;
double sum = 0;

float t = 0; // t is the execution time for a CPU-intensive loop
clock_t t1, t2;
do{
//start measuring execution time
t1 = clock();
for(j=0; j<jmax; j++)
for(i=0; i<max; i++) 
        if(i%2==0) sum += i%1000;
        else sum -= i%1000;
// end measuring execution time
t2 = clock();
t = (((float)t2 - (float)t1) / 1000000.0F ) * 1000;

if(t >= MAX_DURATION) jmax /= 2;
//printf("intermediate t=%f; j = %d\n", t, jmax);
}
while(t>=MAX_DURATION && jmax>1);

//cout<<"t = "<<t<<" at jmax = "<<jmax<<endl;
//printf("t = %f at jmax = %d \n", t, jmax);

cout<<t<<" "<<jmax<<endl;

return 0;
}

