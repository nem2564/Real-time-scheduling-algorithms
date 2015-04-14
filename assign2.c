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

//////////////////////////STRUCTURE FOR TASKS????????????????????????????????????????????

struct task{
	float wcet;
	float deadline;
	float period;
	int priority;
	float slake_time;
};

struct task *TASK_details; //pointer to dynamically allocated structures.
int failed_task=0;			// glloabl variable for failing tasks


/////////////////////UTILIZATION TEST /////////////////////////////////////////////////

int utilization_test(int total,int set_val){
	float uti[20];
	float utilization_sum =0;		//utilization factor by summatoion ei/ pi
	float utilization_power = 0;  // Utilization factor =====n(2^1/n - 1).
	int i, j;
	int set = set_val;
	int n = total - set;
	float sum_one = 0;
	float sum_two=0;
	float sum_three = 0;
	float total_sum = 0;
	int flag = 0;


	i = 0;
	set = set_val;												//total number of tasks to be schedule
	if((TASK_details[0].wcet / TASK_details[0].period )< 1)
		printf("Utilization test paseed for subtask 1 which is < 1\n"); // utilization test for first elemetn of the task
	for(i = 1; i< set; i++){

		sum_three=0;sum_one=0;sum_two=0;					//utilizatino for next all elements in tasksets
		for(j=i; j>0;j--){
			if (TASK_details[j].period > TASK_details[j-1].period){
				sum_one = sum_one + (TASK_details[j-1].wcet / TASK_details[j-1].deadline);  // sum of ei/pi for higher priority tasks.
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
		total_sum = sum_two + sum_one + sum_three; // total addition of all elements of equation
//		printf("Summation wth ei/pi  %f AND ", total_sum);
		utilization_power = (i+1) * (pow(2, 1/(float)(i+1)) - 1);
//		printf("utilization_power is %f\n", utilization_power);
		if(total_sum <= utilization_power) printf("Utilization test paseed for subtask %d------\n", i+1);
		else{ 
		flag = 1;
		failed_task = i;
	 	printf("TASK scheduling  using utilization test is inconclusive RT needed \n");
	 	response_time(total,set_val);
	 	}

}
	if(!flag)printf("ALL TASKS ARE SCHEDULABLE BY UT\n"); // Print result
	return 0;
}	

/////////////////////////////////////??RESPONSE TIME ANALYASIS////////////////////////////////////////

int response_time(int total,int set_val){
//	printf("Called responsetime function and set value is %d failed task value is %d \n", set_val, failed_task);
	int i=0, j;
	int set = set_val;
	int n = failed_task;
	int flag_for_uti = 0;
	float a0 = 0.0;
	float rt_task_wcet;
	float responsetime_A[50]; //array to store individual element of equation of summation to find response time 
	int flag=  0;

	memset(responsetime_A, 0, 50);	// set all array elements to zero
	while(set !=0){
		responsetime_A[0] += TASK_details[i].wcet;  //summation of all execution time 
//		printf("%f and %f\n",responsetime_A[0],TASK_details[i].wcet );
		i++;
		set--;
	}
	printf("TOTAL EXCECUTION TIME IS %f \n",responsetime_A[0]); // print execution time for 


// outer for loop is for number of tries it is going to do before declaring task to be non schedulable.

	for(i= total - set_val +1; i<= 10;i++){
		for(j = 0; j<= n-1; j++){
			a0 += ceil(responsetime_A[i-1]/ TASK_details[j].period) * TASK_details[j].wcet; // ceil function which is ei/pi with upper bound
//			printf("Value of %d A0 in iteration %f\n",j, a0);
		}
		responsetime_A[i] = TASK_details[failed_task].wcet + a0;   //execuation time + summatino we found from for loop just above
		if(responsetime_A[i] == responsetime_A[i-1]){  // comdition : if response time for n is == to response time for n-1.
		 printf("TASK IS SCHEDULABLE \n");
//		task_ok++;
		flag = 1;
		break;// break and go out of loop
		}
		else if(responsetime_A[i] > TASK_details[n].deadline){    // if deadline over runs then break and declare task to be non schedulable
		 flag=0;		
		 break;
		}
//		printf("Value of A[%d] is %f\n",i, responsetime_A[i]);
		a0=0;

	}


if(flag ==0){
 printf("TASK IS NOT SCHEDULABLE BY RT\n");
//	task_failed++;
}	
failed_task =0;  // resetting glabal value
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
		
	printf("Sorted list according to period is %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period);
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
		
	printf("Sorted list according to deadline is %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period);
	}

}
//////////////////////////sorting algorithm by lST/////////////////////I used simple swap algorithm
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
		
	printf("Sorted list according to slaketime is %f and %f and %f and %f\n", task_add[i].wcet, task_add[i].deadline, task_add[i].period, task_add[i].slake_time);
	}

}

int main(){
	FILE *fr;
	int line_count = 0;
	char line[80], test[80];
 	char *token;
 	int set;
 	int no_of_task, task_counter, task_set_counter;
 	int total_task_sets = 0;
 	int check_for_PD = 0;


	fr = fopen ("/home/esp/Desktop/input.txt", "rt");   /// file reading 
	
	if (fgets(line,80, fr) != NULL)
   	{
   		token = strtok(line, " ");  // read value by token
   		no_of_task = atoi(token);
   		task_counter = no_of_task;    // total number of tasks
   		printf("Number of tasksets are %d\n", task_counter);
	}
 	
 	while(task_counter != 0){
	 	fgets(line, 80, fr);
	   		token = strtok(line, " ");
	   		task_set_counter = atoi(token);
	   		TASK_details = (struct task*)malloc(task_set_counter * sizeof(struct task));
	   		set = task_set_counter;
	   		total_task_sets =  set;
	   		printf("Number of tasks are %d\n", task_set_counter);
	   		
//////////////////////reads value by lines//////////////////////////
	   		while(task_set_counter != 0){
		   		fgets(line, 80, fr);
		   			token = strtok(line, " ");
		   			TASK_details[line_count].wcet = atof(token);    // Execution time 
		   			printf("WCET %f for task %d...", TASK_details[line_count].wcet, task_set_counter);
		   			token = strtok(NULL, " ");
		   			TASK_details[line_count].deadline = atof(token);   // deadline 
		   			printf("deadline is %f && ...", TASK_details[line_count].deadline);
		   			token = strtok(NULL, " ");
		   			TASK_details[line_count].period = atof(token);  // period
		   			printf("period is %f && ", TASK_details[line_count].period);
		   			if(TASK_details[line_count].deadline < TASK_details[line_count].period) check_for_PD = 1;
		   			TASK_details[line_count].priority = line_count +1;  // Priority
		   			printf("priority is %d && ", TASK_details[line_count].priority);
		   			TASK_details[line_count].slake_time = TASK_details[line_count].deadline - TASK_details[line_count].wcet; // set slack time
		   			printf("SLAKETIME is %f \n", TASK_details[line_count].slake_time);
		   			line_count ++;
		   			task_set_counter--;

		   	}

////////////////////RATE MONOTONIC SCHEDULING??/////////////////

	 		sorting_period(TASK_details, set);
	 		printf("RATE MONOTONIC SCHEDULING WITH SORTING ACCORDING TO PERIOD AND CHECK_for_PD is  %d\n", check_for_PD);
	 		utilization_test(total_task_sets,set);
//	 		if(check_for_PD != 1) utilization_test(total_task_sets,set);
//	 		else response_time(total_task_sets,set);
	 		line_count = 0;
////////////////////DEADLINE MONOTONIC SCHEDULING??/////////////////	 		
	 		sorting_deadline(TASK_details, set);
	 		printf("\nDADEADLINE MONOTONIC SCHEDULING WITH SORTING ACCORDING TO PERIOD AND CHECK_for_PD is  %d\n", check_for_PD);
	 		utilization_test(total_task_sets,set);
	 		
////////////////////SLACK TIME MONOTONIC SCHEDULING??/////////////////	 		
	 		sorting_slaketime(TASK_details, set);
	 		printf("\nSLAKETIME MONOTONIC SCHEDULING WITH SORTING ACCORDING TO PERIOD AND CHECK_for_PD is  %d\n", check_for_PD);
	 		utilization_test(total_task_sets,set);
	 			 		
// Free task structures///////////
	 		free(TASK_details);
	 		check_for_PD = 0;
//	 		responce_time(total_task_sets,set);	
 		task_counter--;
 	}
   
                                                      // free alocated memory space to task array
   fclose(fr);                                                                   // close file.

   return 0;
}