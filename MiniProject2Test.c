// Lab2Test.c
// Runs on LM4F120/TM4C123
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// Modified by Sile Shu 10/4/17, ss5de@virginia.edu
// Modified by Mustafa Hotaki 8/1/2018, mkh3cf@virginia.edu

#include <stdint.h>
#include "OS.h"
#include "tm4c123gh6pm.h"
#include <string.h> 
#include "PLL.h"
#include "PORTE.h"

unsigned long NumCreated;   // Number of foreground threads created
int CS_Start, CS_Stop, CS_Time; // Variables to calculate context switch (time bw end of one thread and start of second thread)
int TS_Start, TS_End, TS_Time; // Variables to calculate thread switch (end to start of one thread)
int TP_S, TP_F; // Variables to calculate thread frequency
int start, stop, Slice; 
int first_time = 0;

//******************* Measurement of context switch time **********
// Run this to measure the time it takes to perform a task switch
int TS_Start=0, TS_End=0, TS_Time, TS_Time_Gap; // Variables to calculate thread switch (end to start of one thread)

void Thread8(void){       // only thread running
	 
  while(1){
		TS_Start = OS_Time();
    PE0 ^= 0x01;      // debugging profile  
		TS_Time = OS_TimeDifference(TS_End, TS_Start);
		if(TS_Time >0x16){
			TS_Time_Gap=TS_Time;
		}
		TS_End = OS_Time();
	}
	
	
}

int main(void){       // Testmain0
  PortE_Init();
  OS_Init();           // initialize, disable interrupts
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread8, 128, 2); 
  OS_Launch(TIME_1MS/10); // 100us, doesn't return, interrupts enabled in here
  return 0;             // this never executes
}

//******************* Initial TEST **********
// Cooperative thread scheduler
unsigned long Count1;   // number of times thread1 loops
unsigned long Count2;   // number of times thread2 loops
unsigned long Count3;   // number of times thread3 loops
unsigned long Count4;   // number of times thread4 loops
unsigned long Count5;   // number of times thread5 loops

void Thread1(void){
  Count1 = 0;
  for(;;){
		start = OS_Time();
    PE0 ^= 0x01;       // heartbeat
    Count1++;
		CS_Start = OS_Time(); // start for context time; end for slice time
	  OS_Suspend();      // cooperative multitasking
  }
}

int tester = 0;

void Thread2(void){
	Count2 = 0;          
  for(;;){
    CS_Stop = OS_Time(); // context end
		Slice = OS_TimeDifference (start,CS_Start); // Slice time difference
		CS_Time = OS_TimeDifference (CS_Start, CS_Stop); // context time difference
		//	CS time =1.3875 microseconds for context switch	
		PE1 ^= 0x02;       // heartbeat
    Count2++;
    OS_Suspend();      // cooperative multitasking
  }
}
void Thread3(void){
  Count3 = 0;          
  for(;;){
    PE2 ^= 0x04;       // heartbeat
    Count3++;
    OS_Suspend();      // cooperative multitasking
  }
}

int Testmain1(void){  // Testmain1
  OS_Init();          // initialize, disable interrupts
  PortE_Init();       // profile user threads
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread1, 128, 1); 
  NumCreated += OS_AddThread(&Thread2, 128, 2); 
  NumCreated += OS_AddThread(&Thread3, 128, 3); 
  // Count1 Count2 Count3 should be equal or off by one at all times
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}

//*******************Second TEST**********
// Preemptive thread scheduler
void Thread1b(void){
  Count1 = 0;
	start = OS_Time(); // Start for time slice	
  for(;;){
    PE0 ^= 0x01;       // heartbeat
    Count1++;
		CS_Start = OS_Time(); // for context time
  }
}
void Thread2b(void){
//	start =  OS_Time(); //Time slice start
//	CS_Stop = OS_Time(); // context end
//	CS_Time = OS_TimeDifference(CS_Start, CS_Stop); // context time difference
	// CS_Start = OS_Time(); // for context time
	CS_Stop = OS_Time(); // context end
	Slice = OS_TimeDifference(start,CS_Start); // Slice time difference
  
	Count2 = 0;          
  CS_Stop = OS_Time(); // context end
	CS_Time = OS_TimeDifference (CS_Start, CS_Stop); // context time difference
	//	CS time =1.3875 microseconds for context switch	
	for(;;){
		
    PE1 ^= 0x02;       // heartbeat
    Count2++;
  }
	
	
}
void Thread3b(void){
  Count3 = 0;          
  for(;;){
    PE2 ^= 0x04;       // heartbeat
    Count3++;
  }
}
int Testmain2(void){    // Testmain2
  OS_Init();            // initialize, disable interrupts
  PortE_Init();         // profile user threads

  NumCreated = 0;
  NumCreated += OS_AddThread(&Thread1b, 128, 1); 
  NumCreated += OS_AddThread(&Thread2b, 128, 2); 
  NumCreated += OS_AddThread(&Thread3b, 128, 3); 

  OS_Launch(TIME_2MS);  // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}
