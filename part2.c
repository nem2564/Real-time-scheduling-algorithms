#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <math.h>

struct task{
	float wcet;
	float deadline;
	float period;
	int priority;
	float slake_time;
};
struct task *TASK_details;
 float uti_array[12];
// float period_array[12];
 float deadline_array[12];
 float wcet_array[12];
int number_of_task;
int failed_task=0;
int deadline_flag=0;



int no_of_RM_success_TASK_12_D1[10];
int no_of_DM_success_TASK_12_D1[10];
int no_of_ST_success_TASK_12_D1[10];



int no_of_RM_fail_TASK_12_D1[10];
int no_of_DM_fail_TASK_12_D1[10];
int no_of_ST_fail_TASK_12_D1[10];



int task_ok=0;
int task_failed =0;

//////////////////////////////SETTING UTILIZATION /////////////////////////////////////////
int unifast(int n, float utilization){
	float sumU = utilization;
	float k;
	memset(uti_array, 0, 12);
	int i,M = 3;
	float nextSumU;
	float sum=0;

	for(i=0; i< n-1; i++){
		
		//k = 1/(float)(n-i);
		nextSumU = sumU * (float)(pow((((float)(rand()%100))/100.0), 1/(float)(n-i)));
//		printf("NEXT SUM U value is  %f \n",nextSumU);
		uti_array[i] = sumU - nextSumU;
		sumU = nextSumU;
//		printf("U value is  %f \n",uti_array[i]);
	}
	uti_array[i] = sumU;
	for(i=0; i<n; i++){
		sum += uti_array[i];
	}
//	printf("Value of sum is %f\n",sum);
	return 0;
}

//////////////////////////////////////SETTING PERIOD FOR THE SYNTHETIC TASK?//////////////////////

void period_funct(int no_of_task){
	int n= no_of_task;
	n = n/3;
	float period;
	int i;
	for(i=1; i<=no_of_task;i++){
		if(i<=n){
			do{									// for getting period between 10- 100
			period = rand()/100.00;
			}
			while(period > 100 || period < 10);
			TASK_details[i-1].period = period;
//			printf("Period value is in i<n %f and %d \n", TASK_details[i-1].period, i);
		}
		else if (i>n && i<=2*n){			// for getting period between 100- 1000
			do{
			period = rand()/1000.00;
			}
			while(period >1000 || period <100);
			TASK_details[i-1].period = period;
//			printf("Period value is in i< 2n %f and %d\n", TASK_details[i-1].period, i);
		}
		else if(i > 2*n && i<= 3*n){				// for getting period between 1000- 10000
			do{
			period = rand()/10000.00;
			}
			while(period >10000 || period < 1000);
			TASK_details[i-1].period = period;
//			printf("Period value is in 3n %f and %d \n", TASK_details[i-1].period, i);
		}
	}
}

///////////////////////////SETTING EXECUTION TIME ///////////////////////////
void wcet_funct(int n){
	int i;
	for(i=0; i< n; i++){
		TASK_details[i].wcet = (float)(uti_array[i] * TASK_details[i].period);   /// wort cast execution time = utilization * period
//		printf("Value of wcet in array is %f and %d \n",TASK_details[i].wcet, i);
	}
}

////////////////////////////SETTTING DEADLINE////////////////////////////////

void deadline_funct(int n){
	float deadline;
	int i;
	for(i=0; i<n; i++){
		float period = TASK_details[i].period;
		float wcet = TASK_details[i].wcet;
		if(deadline_flag ==0){				//////////flag = 0 deadline between [Ci, Ti]
			do{
				deadline = rand()/period;
			}
			while(deadline > period || deadline < wcet);
//				printf("Value of deadline is %f \n", deadline);
			TASK_details[i].deadline = deadline;
		}
		else if(deadline_flag ==1){				////////Conditin flag = 1 deadline between [Ci+(Ci+Ti)/ 2, Ti]
			do{
				deadline = rand()/period;
				wcet = TASK_details[i].wcet +((TASK_details[i].period - TASK_details[i].wcet)/ 2);
			}
			while(deadline > period || deadline < wcet);
//			printf("Value of deadline is %f \n", deadline);
			TASK_details[i].deadline = deadline;			
		}
	}
}



/////////////////////UTILIZATION TEST /////////////////////////////////////////////////
int utilization_test(int total,int set_val){
	float uti[20];
	float utilization_sum =0;
	float utilization_power = 0;// Utilization factor =====n(2^1/n - 1).
	int i, j;
	int set = set_val;
	int n = total - set;
	float sum_one = 0;
	float sum_two=0;
	float sum_three = 0;
	float total_sum = 0;
	int flag = 0;
//printf("\n\n_________UTILIZATION TIME ANALYSIS__________\n\n");
	i = 0;
	set = set_val;							//total number of tasks to be schedule
	if((TASK_details[0].wcet / TASK_details[0].period )< 1)						// utilization test for first elemetn of the task
//		printf("Utilization test paseed for subtask 1 which is < 1\n");
	for(i = 1; i< set; i++){

		sum_three=0.0;sum_one=0.0;sum_two=0.0; 					//utilizatino for next all elements in tasksets
		for(j=i; j> 0;j--){
			if (TASK_details[j].period > TASK_details[j-1].period){
//				printf("VALUE OF TASK DETAILS IS %f  and %f \n", TASK_details[j-1].wcet , TASK_details[j-1].deadline);
				sum_one = sum_one + (TASK_details[j-1].wcet / TASK_details[j-1].deadline);
				      																				// sum of ei/pi for higher priority tasks.
//				printf("SUm one value is %f\n",sum_one);
			}

		}

	// summing up execution time

		for(j=i; j>0; j--){	
			 if (TASK_details[j].period < TASK_details[j-1].period){
					sum_two += TASK_details[j-1].wcet;
		}
	}

	// compare period and deadline and which of them is lesser than use that value to get sun three. i.e sum(execution)* (1/ min(d,p)

		if(TASK_details[i].period < TASK_details[i].deadline) {

			sum_two = sum_two/TASK_details[i].period;
			sum_three = TASK_details[i].wcet/ TASK_details[i].period;
		}
		else {
			sum_two = sum_two/TASK_details[i].deadline; 
			sum_three = TASK_details[i].wcet/ TASK_details[i].deadline;
//			printf("Sum three values is %f %f  %f \n", sum_three, TASK_details[i].wcet, TASK_details[i].deadline);
		}
		total_sum = sum_two + sum_one + sum_three;
//		printf("Summation wth ei/pi  %f AND ", total_sum);
		utilization_power = (i+1) * (pow(2, 1/(float)(i+1)) - 1);
//		printf("  utilization_power is %f and %f\n", utilization_power, total_sum);
		if(total_sum <= utilization_power) {
//			printf("Utilization test passed for subtask %d------\n", i+1);
			task_ok++;
		}
		else{ 
		flag = 1;
		failed_task = i;
//	 	printf("TASK scheduling  using utilization test is inconclusive RT needed for %d \n",i +1);
	 	response_time(total,set_val, i);
	 	}

}
//	if(!flag)printf("ALL TASKS ARE SCHEDULABLE BY UT\n\n");
	return 0;
}	


/////////////////////////////////////??RESPONSE TIME ANALYASIS////////////////////////////////////////

int response_time(int total,int set_val, int task_number){
//	printf("\n\n _________RESPONSE TIME ANALYSIS__________\n\n");
//	printf("Called responsetime function and set value is %d failed task value is %d \n", set_val, task_number);
	int i=0, j;
	int set = set_val;
	int n = task_number;
	int flag_for_uti = 0;
	float a0 = 0.0;
	float rt_task_wcet;
	float responsetime_A[50];
	int flag=  0;

	memset(responsetime_A, 0, 50);	
	while(set !=0){
		responsetime_A[0] += TASK_details[i].wcet;				//summation of all execution time 
//		printf("%f and %f\n",responsetime_A[0],TASK_details[i].wcet );
		i++;
		set--;
	}
//	printf("RESPONSE TIME IS %f \n",responsetime_A[0]);



// outer for loop is for number of tries it is going to do before declaring task to be non schedulable.


	for(i= total - set_val +1; i<= 10;i++){
		for(j = 0; j<= n-1; j++){
			a0 += ceil(responsetime_A[i-1]/ TASK_details[j].period) * TASK_details[j].wcet; 
//			printf("Value of %d A0 in iteration %f\n",j, a0);
		}
		responsetime_A[i] = TASK_details[task_number].wcet + a0;
		if(responsetime_A[i] == responsetime_A[i-1]){						// comdition : if response time for n is == to response time for n-1.
//		 printf("TASK IS SCHEDULABLE \n");
		task_ok++;
		flag = 1;
		break; 			// break and go out of loop
		}
		else if(responsetime_A[i] > TASK_details[n].deadline){			// if deadline over runs then break and declare task to be non schedulable
		 flag=0;
		 break;
		}
//		printf("Value of A[%d] is %f\n",i, responsetime_A[i]);
		a0=0;

	}


if(flag ==0){
// printf("TASK IS NOT SCHEDULABLE BY RT\n");
	task_failed++;
}	
failed_task =0;
	return 0;
}


/// sorting algorithm by period/////////////////////I used simple swap algorithm

void sorting_period(void *task, int set){
	int i = 0, j;
	struct task *task_add = task;
	for(i=0; i<set; i++){
		for(j= i+1; j<set; j++){
			if(task_add[i].period > task_add[j].period){
				struct task temp;
				temp.wcet = task_add[i].wcet;
				temp.period = task_add[i].period;
				temp.deadline = task_add[i].deadline;
				task_add[i].wcet = task_add[j].wcet;
				task_add[i].period = task_add[j].period;
				task_add[i].deadline = task_add[j].deadline;
				task_add[j].wcet = temp.wcet;
				task_add[j].period = temp.period;
				task_add[j].deadline = temp.deadline;					
				}
			}
		
//	printf("Sorted list according to period is %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period);
	}

}


///////// sorting algorithm by deadline////////////////I used simple swap algorithm

void sorting_deadline(void *task, int set){
	int i = 0, j;
	struct task *task_add = task;
	for(i=0; i<set; i++){
		for(j= i+1; j<set; j++){
			if(task_add[i].deadline > task_add[j].deadline){
				struct task temp;
				temp.wcet = task_add[i].wcet;
				temp.period = task_add[i].period;
				temp.deadline = task_add[i].deadline;
				task_add[i].wcet = task_add[j].wcet;
				task_add[i].period = task_add[j].period;
				task_add[i].deadline = task_add[j].deadline;
				task_add[j].wcet = temp.wcet;
				task_add[j].period = temp.period;
				task_add[j].deadline = temp.deadline;					
				}
			}
		
//	printf("Sorted list according to deadline is %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period);
	}

}


void sorting_slaketime(void *task, int set){
	int i = 0, j;
	struct task *task_add = task;
	for(i=0; i<set; i++){
		for(j= i+1; j<set; j++){
			if(task_add[i].slake_time > task_add[j].slake_time){
				struct task temp;
				temp.wcet = task_add[i].wcet;
				temp.period = task_add[i].period;
				temp.deadline = task_add[i].deadline;
				temp.slake_time = task_add[i].slake_time;
				task_add[i].wcet = task_add[j].wcet;
				task_add[i].period = task_add[j].period;
				task_add[i].deadline = task_add[j].deadline;
				task_add[i].slake_time = task_add[j].slake_time;
				task_add[j].wcet = temp.wcet;
				task_add[j].period = temp.period;
				task_add[j].deadline = temp.deadline;
				task_add[j].slake_time = temp.slake_time;					
				}
			}
		
//	printf("Sorted list according to slaketime is %f and %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period, task_add[i].slake_time);
	}

}

void lst_funct(void *task, int n){
int i;	
struct task *task_add = task;
for(i=0; i<n; i++){
	task_add[i].slake_time = task_add[i].deadline - task_add[i].wcet;
	}
}

void main(){
	float util = 0.05;
	number_of_task = 12;
	int set = number_of_task;
	TASK_details = (struct task*)malloc(number_of_task * sizeof(struct task));
	int i, j, k;



srand(time(NULL));
memset(no_of_RM_success_TASK_12_D1, 0, 10);
memset(no_of_DM_success_TASK_12_D1, 0, 10);
memset(no_of_ST_success_TASK_12_D1, 0, 10);
// memset(no_of_RM_success_TASK_24_D1, 0, 10);
// memset(no_of_RM_success_TASK_48_D1, 0, 10);
memset(no_of_RM_fail_TASK_12_D1, 0, 10);
memset(no_of_DM_fail_TASK_12_D1, 0, 10);
memset(no_of_ST_fail_TASK_12_D1, 0, 10);


	for(i=0; i<10; i++){			

		for(j=0; j<10000; j++){
			unifast(number_of_task, util);
			period_funct(number_of_task);
			wcet_funct(number_of_task);
			deadline_funct(number_of_task);
			lst_funct(TASK_details, number_of_task);

					for(k=0; k<number_of_task; k++){
						TASK_details[k].priority = k +1;
					}
					set = number_of_task;
			 		sorting_period(TASK_details, set);
//			 		printf("\nRATE MONOTONIC SCHEDULING WITH SORTING ACCORDING TO PERIOD \n");
			 		utilization_test(number_of_task,set);
			 		no_of_RM_success_TASK_12_D1[i] += task_ok;
			 		no_of_RM_fail_TASK_12_D1[i] += task_failed;
			 		task_ok=0;
			 		task_failed =0;

			 		sorting_deadline(TASK_details, set);
//			 		printf("\nDADEADLINE MONOTONIC SCHEDULING WITH SORTING ACCORDING TO PERIOD");
			 		utilization_test(number_of_task,set);
			 		no_of_DM_success_TASK_12_D1[i] += task_ok;
			 		no_of_DM_fail_TASK_12_D1[i] += task_failed;
			 		task_ok=0;
			 		task_failed =0;
			 		
			 		sorting_slaketime(TASK_details, set);
//			 		printf("\nSLAKETIME MONOTONIC SCHEDULING WITH SORTING ACCORDING TO SLACKTIME\n");
			 		utilization_test(number_of_task,set);
			 		no_of_ST_success_TASK_12_D1[i] += task_ok;
			 		no_of_ST_fail_TASK_12_D1[i] += task_failed;
			 		task_ok=0;
			 		task_failed =0;

		}
		printf("SUCCESSFUL RM is                 %d for %d ", no_of_RM_success_TASK_12_D1[i],i);
		printf("FAILED  RM is %d for %d \n", no_of_RM_fail_TASK_12_D1[i], i);
		printf("SUCCESSFUL  DM is         %d for %d ", no_of_DM_success_TASK_12_D1[i], i);
		printf("FAILED  DM is %d  for %d\n", no_of_DM_fail_TASK_12_D1[i],i);
		printf("SUCCESSFUL  ST is   %d  for %d ", no_of_ST_success_TASK_12_D1[i],i);
		printf("FAILED  ST is %d for %d  \n", no_of_DM_fail_TASK_12_D1[i],i);	
				util += 0.10;
	}	 		


}