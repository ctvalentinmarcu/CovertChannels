#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<cstdlib>
#include<iostream>
#include<sys/time.h>
#include<fstream>
#include<vector>
#include <unistd.h>
#include<math.h>

#define BLOCKSIZE 16
#define FCS 4
#define ACK 1

using namespace std;

//ofstream fout;

void cpu_intensive(unsigned long jmax){
unsigned long i=0, j=0;
unsigned long max = 1000*1000;

double sum = 0;
for(j=0; j<jmax; j++)
   for(i=0; i<max; i++)
        if(i%2==0) sum += i%1000;
        else sum -= i%1000;

}

int get_time(){
   struct timeval current_time;
   int time = 0;

   gettimeofday(&current_time, NULL);
   //cout<<current_time.tv_sec<<" "<<current_time.tv_usec<<endl;
   time = (int) (current_time.tv_sec) % 10000;
   time = 10*time + (int) (current_time.tv_usec / 100000);

   return time;
}


void send_one(int index, int t0, unsigned long jmax){
//   cout<<"1";

   struct timeval current_time, tv1, tv2;
   int time, t;
   int t0up = 1 + t0 / 100; //t0 rounded up towards the next multiple of 100 ms
   int transactions = BLOCKSIZE + FCS + ACK;
   int transactions_reserved = transactions * (2 * t0up);
   int remainder;
   int target_id = index;
   
   do{
        //usleep(100*1000); // sleep for 100 ms to avoid busy waiting
        time = get_time();
        remainder = (time % transactions_reserved) / (2 * t0up);
   }while(remainder != target_id);

   cpu_intensive(jmax);
   //usleep(t0*1000);
}

void send_zero(int index, int t0, unsigned long jmax){
//   cout<<"0";

   struct timeval current_time, tv1, tv2;
   int time, t;
   int t0up = 1 + t0 / 100; //t0 rounded upwards to the next multiple of 100 ms
   int transactions = BLOCKSIZE + FCS + ACK;
   int transactions_reserved = transactions * (2 * t0up);
   int remainder;
   int target_id = index;

   usleep(t0*1000); // this step lasts at least t0 ms   
   do{
        usleep(100*1000); // sleep for 100 ms to avoid busy waiting
        time = get_time();
        remainder = (time % transactions_reserved) / (2 * t0up);
   }while(remainder != target_id);

   //usleep(t0*1000);
}

int receive(int index, int t0, unsigned long jmax){
   struct timeval current_time, tv1, tv2;
   int time, t;
   int t0up = 1 + t0 / 100; //t0 rounded upwards to the next multiple of 100 ms
   int transactions = BLOCKSIZE + FCS + ACK;
   int transactions_reserved = transactions * (2 * t0up);
   int remainder;
   int target_id = index;
   
   //usleep(t0*1000); // this step lasts at least t0 ms
   do{
        usleep(100*1000); // sleep for 100 ms to avoid busy waiting
        time = get_time();
        remainder = (time % transactions_reserved) / (2 * t0up);
   }while(remainder != target_id);
//cout<<"test"<<endl;
   // start measuring execution time
   gettimeofday(&tv1, NULL);
   // call the cpu-intensive function
   cpu_intensive(jmax);
   // end measuring execution time
   gettimeofday(&tv2, NULL);
   t = (int) ( (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) /
1000 );

   if(t-t0 > t0/2) return 1; // if the receiver sends ACK=1
   return 0;
}

#if 0
bool receive_ACK(int t0, unsigned long jmax){
   bool received = false;
   struct timeval current_time, tv1, tv2;
   int time, t;
   int t0up = 1 + t0 / 100; //t0 rounded upwards to the next multiple of 100 ms
   int transactions = BLOCKSIZE + FCS + ACK;
   int transactions_reserved = transactions * (2 * t0up);
   int remainder;
   int id = transactions-1;

   do{
        usleep(100*1000); // sleep for 100 ms to avoid busy waiting
        time = get_time();
        remainder = (time % transactions_reserved) / (2 * t0up);
   }while(remainder != id);

   // start measuring execution time
   gettimeofday(&tv1, NULL);
   // call the cpu-intensive function
   cpu_intensive(jmax);
   // end measuring execution time
   gettimeofday(&tv2, NULL);
   t = (int) ( (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) /
1000 );

   if(t-t0 > t0/2) return true; // if the receiver sends ACK=1
   return false;
}
#endif

void receive_block(vector<int> output, const char* out_filename, int t0, unsigned long jmax)
{
   struct timeval current_time, tv1, tv2;
   //cout<<t0<<" "<<jmax<<endl;
   //int temp[BLOCKSIZE];
   int i, j;
   int time;
   int t0up = 1 + t0 / 100; //t0 rounded upwards to the next multiple of 100 ms
   int transactions = BLOCKSIZE + FCS + ACK;
   int transactions_reserved = transactions * (2 * t0up);
   //cpu_intensive(jmax);
   bool valid = false;
   while(!valid) {
     do
     {
        //usleep(t0/4*1000); // sleep for 100ms;
        gettimeofday(&current_time, NULL);
        //cout<<current_time.tv_sec<<" "<<current_time.tv_usec<<endl;
        time = (int) (current_time.tv_sec) % 10000;
        time = 10*time + (int) (current_time.tv_usec / 100000);
        //cout<<"time with .1s precision: "<<time<<endl;
        //cpu_intensive(jmax);
     }
     while(time % transactions_reserved > t0/4 &&
        time % transactions_reserved < transactions_reserved - t0/4);

//return;
     //cpu_intensive(jmax);
     int sum = 0;   
     // receive BLOCKSIZE bits of data from sender
     for(i = 0; i<BLOCKSIZE; i++){
                output[i] = receive(i,t0,jmax);
                sum += output[i];
                cout<<output[i];
       // }
     }
     // receive FCS bits representing the value of sum
     cout<<" FCS: ("<<sum<<"): ";
     int sum_bits[FCS];
     int sum2=0;
     for(i=0; i<FCS; i++){
        sum_bits[i] = receive(BLOCKSIZE+i,t0,jmax);
        cout<<sum_bits[i];
        sum2 += sum_bits[i] * ((int) pow(2,FCS-1-i) );
     }
     
        /*
     for(i=1; i<=FCS; i++){
        int bit = sum_bits[i-1]; //(sum % aux == 0) ? 1 : 0;
        if(bit==0) send_zero(i-1 + BLOCKSIZE, t0, jmax);
        else send_one(i-1 + BLOCKSIZE, t0, jmax);
     }
     valid = receive_ACK(t0, jmax);
        */
cout<<" sum received = "<<sum<<" sum computed = "<<sum2; 
     // signal ACK to sender
     if(sum==sum2){
        valid = true;
        send_one(transactions-1, t0, jmax);
     }
     else{
        valid = false;
        send_zero(transactions-1, t0, jmax);
     }

     //valid = true;
     if(valid == true){
        ofstream fout;
        fout.open(out_filename, ios::out | ios::app);
        for(i=0;i<BLOCKSIZE;i++)
                fout << output[i];
        fout.close();
     }
     cout<<endl;
   }
}


int main(int argc, char** argv)
{

using namespace std;

struct timeval current_time;
unsigned long jmax = 115;
int t0 = 50;
int t = 0; // t is the execution time for a CPU-intensive function

int i=0, j=0;

clock_t t1, t2;

vector<int> output;

if(argc >= 1){
   ifstream fin1(argv[1]);
   //ofstream fout2(argv[2]);
   
   fin1>>t0;
   fin1>>jmax;


   fin1.close();
   //fin2.close();
}

if(argc < 2) return -1;

//ofstream fout2(argv[2]);
//fout.open(argv[2]);

int output_dim = output.size();

// initialize output with BLOCKSIZE zeros
for(i=0; i<BLOCKSIZE; i++)
        output.push_back(0);
gettimeofday(&current_time, NULL);
//cout<<current_time.tv_sec<<" "<<current_time.tv_usec
//    <<" t0="<<t0<<" jmax="<<jmax<<endl;

struct timeval tv1, tv2;

int block=0;

//for(block=0; block<nblocks; block++)
while(1)
{
   // start measuring execution time
   //t1 = clock();
   gettimeofday(&tv1, NULL);
   // call the cpu-intensive function
   //cpu_intensive(jmax);
   receive_block(output, argv[2], t0, jmax);   
   // end measuring execution time
   //t2 = clock();
   gettimeofday(&tv2, NULL);
   //t = (int) ((((float)t2 - (float)t1) / 1000000.0F ) * 1000);
   t = (int) ( (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000 );

  // cout<<"live time for block = "<<t<<endl;
   //printf("intermediate t=%f; j = %d\n", t, jmax);
}

//cout<<"Done."<<endl;

//fout2.close();

//std::cout<<"t = "<<t<<" at jmax = "<<jmax;
//printf("t = %f at jmax = %d \n", t, jmax);

return 0;
}

