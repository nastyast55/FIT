#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>


/*----------Jmena semaforu----------*/
#define OUT "/out_xstoik00"
#define AKCE_ID "/akce_id_xstoik00"
#define ADULT_QUEUE "/adultQueue_xstoik00"
#define CHILD_QUEUE "/childQueue_xstoik00"
#define ADULT_SEM "/adult_sem_xstoik00"
#define CHILD_SEM "/child_sem_xstoik00"
#define POM_SEM "/pom_sem_xstoik00"
#define LEAVE_AD "/leave_ad_xstoik00"
#define WAIT_CH "/wait_ch_xstoik00"
#define FINISH_SEM "/finish_sem_xstoik00"
#define CNT_SEM "/count_sem_xstoik00"

typedef enum Error {
    ERROR_OK = 0,    
    ERROR_ENTER = 1,
    ERROR_SYSCALL = 2,       
    ERROR_MALLOC_SH = 3,
    ERROR_MALLOC_SEM = 4,
    ERROR_FILE = 5,
    ERROR_UNDEF = 6,
} Error;

const char *ERR_TABLE[20] = {
    "V poradku.",
    "Chybne zadany vstup.",
    "Chyba v systemovem volani",
    "Chyba alokace sdilene pameti.",
    "Chyba alokace semaphore.",
    "Chyba otevreni souboru.",
    "Unknown error.",
};

void printErr(Error TypeError){
	if(TypeError != 0) // 0 nebo ERROR_OK
    	fprintf(stderr, "ERROR: %s\n", ERR_TABLE[TypeError]);
    	exit(TypeError);
}

void printHelp(){
	printf("Napoveda bla bla.\n");
	return;
}

typedef struct params
{
  int A;	
  int C;	
  int AGT;	
  int CGT;	
  int AWT;
  int CWT;
}sParams;

int get_args(sParams *args, int argc, char *argv[]){
	args->A = 0;
  	args->C = 0;
  	args->AGT = 0;
  	args->CGT = 0;
  	args->AWT = 0;
  	args->CWT = 0;

  	if(argc == 2){		//i kdyz 1 arg -> taky help ?
    	if(strcmp(argv[1], "-h") == 0)
      		printHelp();
    }
	else if (argc == 7){
		char *var = NULL;
		
		args->A = strtol(argv[1],&var,10);
		if (*var != '\0' || errno != 0 || args->A < 1)
			printErr(ERROR_ENTER);
		
		args->C = strtol(argv[2],&var,10);
		if (*var != '\0' || errno != 0 || args->C < 1)
			printErr(ERROR_ENTER);

		args->AGT = strtol(argv[3],&var,10);
		if (*var != '\0' || errno != 0 || args->AGT < 0 || args->AGT > 5000)
			printErr(ERROR_ENTER);
		
		args->CGT = strtol(argv[4],&var,10);
		if (*var != '\0' || errno != 0 || args->CGT < 0 || args->CGT > 5000)
			printErr(ERROR_ENTER);
		
		args->AWT = strtol(argv[5],&var,10);
		if (*var != '\0' || errno != 0 || args->AWT < 0 || args->AWT > 5000)
			printErr(ERROR_ENTER);
		
		args->CWT = strtol(argv[6],&var,10);
		if (*var != '\0' || errno != 0 || args->CWT < 0 || args->CWT > 5000)
			printErr(ERROR_ENTER);

	}
	else{
		printErr(ERROR_ENTER);
	}  

	return ERROR_OK;
}

typedef struct semaphore {
	sem_t *out;				
	sem_t *akce_id_sem; 
	sem_t *adultQueue_sem;
	sem_t *childQueue_sem; 
	sem_t *adult_sem; 
	sem_t *child_sem;
	sem_t *pom_sem;
	sem_t *wait_ch;
	sem_t *leave_ad;  
	sem_t *finish_sem;  
	sem_t *count_sem;      
} semaphores;

int main_adult(int *akce_id, semaphores *sem, int proc_id, sParams *args, int *adults_sh, int *waiting_child, int *children_sh, FILE *OUTP, int *leaving_adult, int *count_process){
	//int ad_id = *adults_sh++;   //fronta adultu ---> id
	//int st = *akce_id++;
	int n;

// Adult process starts
	sem_wait(sem->out);
	sem_wait(sem->akce_id_sem);
	*akce_id += 1;
	fprintf(OUTP, "%d\t: A %d\t: started\n", *akce_id, proc_id);
	fflush(OUTP);
	sem_post(sem->akce_id_sem);
	sem_post(sem->out);

//Adult process enters
	sem_wait(sem->out);
	sem_wait(sem->akce_id_sem);
	sem_wait(sem->adult_sem);
	*adults_sh += 1;
	*akce_id += 1;
	fprintf(OUTP, "%d\t: A %d\t: enter\n", *akce_id, proc_id);
	fflush(OUTP);
	sem_post(sem->adult_sem);
	sem_post(sem->akce_id_sem);
	sem_post(sem->out);

//-----------------------Enter-----------------------
//	sem_wait(sem->pom_sem);

		sem_wait(sem->pom_sem);
		
		//sem_wait(sem->wait_ch);
		if(*waiting_child > 0){
			n = (*waiting_child > 3) ? 3 : *waiting_child;

			sem_wait(sem->wait_ch);
			sem_wait(sem->child_sem);
			*waiting_child -= n;
			sem_post(sem->child_sem);
			sem_post(sem->wait_ch);

			for(int j = 0; j < n; j++)
				sem_post(sem->childQueue_sem); //aby mohli vstoupit child procesy po uvolneni z fronty
		}
	
		sem_post(sem->pom_sem);

	//simuluje cinnost v centru tak ze se uspi na nahodnou dobu
	if (args->AWT != 0) 
		usleep(rand() % args->AWT);

	//po probuzeni se pokusi centrum opustit
	sem_wait(sem->out);
	sem_wait(sem->akce_id_sem);
	*akce_id += 1;
	fprintf(OUTP, "%d\t: A %d\t: trying to leave\n", *akce_id, proc_id);
	fflush(OUTP);
	sem_post(sem->akce_id_sem);
	sem_post(sem->out);

	//Adults leaving
	//sem_wait(sem->pom_sem);
		//zustava-li dostatek dospelych pro deti v centru, po odchodu jednoho
		if(*children_sh <= ((*adults_sh) - 1) * 3){
			sem_wait(sem->adult_sem);
			sem_wait(sem->akce_id_sem);
			sem_wait(sem->out);
			*adults_sh -= 1;
			*akce_id += 1;
			fprintf(OUTP, "%d\t: A %d\t: leave\n", *akce_id, proc_id);
			fflush(OUTP);
			sem_post(sem->out);
			sem_post(sem->akce_id_sem);
			sem_post(sem->adult_sem);

			sem_post(sem->pom_sem);
		}
		//nedostatek dospelych --> 
		else{
			sem_wait(sem->leave_ad);
			sem_wait(sem->akce_id_sem);
			sem_wait(sem->out);
			*leaving_adult += 1;
			*akce_id += 1;
			fprintf(OUTP, "%d\t: A %d\t: waiting : %d : %d\n", *akce_id, proc_id, *adults_sh, *children_sh);
			fflush(OUTP);
			sem_post(sem->out);
			sem_post(sem->akce_id_sem);
			sem_post(sem->leave_ad);

			sem_post(sem->pom_sem);

			//kdyz se otevre adult muze vystoupit
			sem_wait(sem->adultQueue_sem);

			sem_wait(sem->out);
			sem_wait(sem->adult_sem);
			sem_wait(sem->akce_id_sem);
			*akce_id += 1;
			*adults_sh -= 1;
			fprintf(OUTP, "%d\t: A %d\t: leave\n", *akce_id, proc_id);
			fflush(OUTP);
			sem_post(sem->akce_id_sem);
			sem_post(sem->child_sem);
			sem_post(sem->out);
		}

//----------------------------------------FINISH-------------------------------------------
	*count_process += 1;
		if(*count_process == (args->A + args->C))
			sem_post(sem->finish_sem);

		sem_wait(sem->finish_sem);
		sem_wait(sem->akce_id_sem);
		sem_wait(sem->out);
		*akce_id += 1;
		fprintf(OUTP, "%d\t: A %d\t: finished\n", *akce_id, proc_id);
		fflush(OUTP);
		sem_post(sem->out);
		sem_post(sem->akce_id_sem);
		sem_post(sem->finish_sem);

	exit(EXIT_SUCCESS);

}
int main_child(int *akce_id, semaphores *sem, int child_id, sParams *args, int *children_sh, int *adults_sh, int *waiting_child, FILE *OUTP, int *leaving_adult, int *count_process){
	sem_wait(sem->out);
	sem_wait(sem->akce_id_sem);
	*akce_id += 1;
	fprintf(OUTP, "%d\t: C %d\t: started\n", *akce_id, child_id);
	fflush(OUTP);
	sem_post(sem->akce_id_sem);
	sem_post(sem->out);

	//-----------------Child process enters-----------------
	//sem_wait(sem->pom_sem);

		if(*children_sh < ((*adults_sh) * 3)){
			sem_wait(sem->out);
			sem_wait(sem->child_sem);
			sem_wait(sem->akce_id_sem);
			*akce_id += 1;
			*children_sh += 1;
			fprintf(OUTP, "%d\t: C %d\t: enter\n", *akce_id, child_id);
			fflush(OUTP);
			sem_post(sem->akce_id_sem);
			sem_post(sem->child_sem);
			sem_post(sem->out);

			sem_post(sem->pom_sem);

			//Child simuluje aktivitu v centru
			if (args->CWT != 0) 
				usleep(rand() % args->CWT);


		}
		else{
			sem_wait(sem->out);
			sem_wait(sem->akce_id_sem);
			sem_wait(sem->wait_ch);
			*waiting_child += 1;
			*akce_id += 1;
			fprintf(OUTP, "%d\t: C %d\t: waiting : %d : %d\n", *akce_id, child_id, *adults_sh, *children_sh);
			fflush(OUTP);
			sem_post(sem->wait_ch);
			sem_post(sem->akce_id_sem);
			sem_post(sem->out);

			sem_post(sem->pom_sem);
		
		//kdyz se otevre dite muze vstoupit
			sem_wait(sem->childQueue_sem);

			sem_wait(sem->out);
			sem_wait(sem->child_sem);
			sem_wait(sem->akce_id_sem);
			*akce_id += 1;
			*children_sh += 1;
			fprintf(OUTP, "%d\t: C %d\t: enter\n", *akce_id, child_id);
			fflush(OUTP);
			sem_post(sem->akce_id_sem);
			sem_post(sem->child_sem);
			sem_post(sem->out);

			if (args->CWT != 0) 
				usleep(rand() % args->CWT);
		}

	//----------------------CHILD IS LEAVING------------------------
//po probuzeni snazi sa opustit centr

	sem_wait(sem->out);
	sem_wait(sem->akce_id_sem);
	*akce_id += 1;
	fprintf(OUTP, "%d\t: C %d\t: trying to leave\n", *akce_id, child_id);
	fflush(OUTP);
	sem_post(sem->akce_id_sem);
	sem_post(sem->out);	

	sem_wait(sem->child_sem);
	sem_wait(sem->akce_id_sem);
	sem_wait(sem->out);
	*children_sh -= 1;
	*akce_id += 1;
	fprintf(OUTP, "%d\t: C %d\t: leave\n", *akce_id, child_id);
	fflush(OUTP);
	sem_post(sem->out);
	sem_post(sem->akce_id_sem);
	sem_post(sem->child_sem);

		//kdyz odchazi overuje frontu adultov cekajicich na vystup
	if(*leaving_adult > 0){
		if(*children_sh <= 3*(*adults_sh - 1)){
			sem_wait(sem->leave_ad);
			sem_wait(sem->adult_sem);
			*leaving_adult -= 1;	//adult cekajici ve fronte na odchod muze odejit
			//*adults_sh -= 1;		// -> 1 adult muze odejit
			sem_post(sem->adult_sem);
			sem_post(sem->leave_ad);
			//adult muze odejit
			sem_post(sem->adultQueue_sem);
			}
		}

//-----------------------------------------FINISH------------------------------------------------
		*count_process += 1;
		if(*count_process == (args->A + args->C))
			sem_post(sem->finish_sem);

		sem_wait(sem->finish_sem);
		sem_wait(sem->akce_id_sem);
		sem_wait(sem->out);
		*akce_id += 1;
		fprintf(OUTP, "%d\t: C %d\t: finished\n", *akce_id, child_id);
		fflush(OUTP);
		sem_post(sem->out);
		sem_post(sem->akce_id_sem);
		sem_post(sem->finish_sem);

	exit(EXIT_SUCCESS);
}


int generate_adult(sParams *args, int *akce_id, semaphores *sem, int *adults_sh, int *waiting_child, int *children_sh, FILE *OUTP, int *leaving_adult, int *count_process){

	pid_t adults_array[args->A];
		for(int i = 1; i <= args->A; i++)
		{
			pid_t adlt = fork();

			if (adlt == 0){		
				main_adult(akce_id, sem, i, args, adults_sh, waiting_child, children_sh, OUTP, leaving_adult, count_process);
			}
			
			else{ 
				adults_array[i] = adlt;
				
				//Vygenerovat novy proces po dobe AGT, jestli je zadana
				if (args->AGT != 0) 
					usleep(rand() % args->AGT);
			}
		}
		// kill zombies
		for (int i = 1; i <= args->A; i++) 
			waitpid(adults_array[i],NULL,0);

		exit(EXIT_SUCCESS);
}
int generate_child(sParams *args, int *akce_id, semaphores *sem, int *children_sh, int *adults_sh, int *waiting_child, FILE *OUTP, int *leaving_adult, int *count_process){
	
	pid_t children_array[args->C];
		for(int i = 1; i <= args->C ; i++)
		{
			pid_t chld = fork();
			if (chld == 0){	
				main_child(akce_id, sem, i, args, children_sh, adults_sh, waiting_child, OUTP, leaving_adult, count_process);
			}
			else{ 
				children_array[i] = chld;

				if (args->CGT != 0) 
					usleep(rand() % args->CGT);
			}
		}
		// kill zombies
		for (int i = 1; i <= args->C ; i++) 
			waitpid(children_array[i],NULL,0);
		
		exit(EXIT_SUCCESS);
}

int allocation(semaphores *sem, sParams *args, FILE *OUTP){
	int akce, adult_id, child_id, waiting, leaving, cnt_proc;
	int *akce_id, *adults_sh, *children_sh, *waiting_child, *leaving_adult, *count_process;
/*-----------------------Alokace slilene pameti----------------------*/
	if((akce = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((akce_id = (int*)shmat(akce, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }

    if((adult_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((adults_sh = (int*)shmat(adult_id, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }

    if((child_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((children_sh = (int*)shmat(child_id, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }

    if((waiting = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((waiting_child = (int*)shmat(waiting, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }

    if((leaving = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((leaving_adult = (int*)shmat(leaving, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }

    if((cnt_proc = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0){
    	return ERROR_MALLOC_SH;
    }
    if((count_process = (int*)shmat(cnt_proc, NULL, 0)) == (void *) -1){
    	return ERROR_MALLOC_SH;
    }
 
/*--------------------------- Alokace semaforu ---------------------*/
    if ((sem->out = sem_open(OUT, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->akce_id_sem = sem_open(AKCE_ID, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->adultQueue_sem = sem_open(ADULT_QUEUE, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->childQueue_sem = sem_open(CHILD_QUEUE, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->adult_sem = sem_open(ADULT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->child_sem = sem_open(CHILD_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->pom_sem = sem_open(POM_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->wait_ch = sem_open(WAIT_CH, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->leave_ad = sem_open(LEAVE_AD, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->finish_sem = sem_open(FINISH_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	if ((sem->count_sem = sem_open(CNT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED){
		return ERROR_MALLOC_SEM;
	}
	// nastavit "zelene svetlo" pro semafory
	//--potom proces nastaví semafor na „červenou“ (pro ostatní procesy), pracuje se sdílenou pamětí 
	//---a pak jej zase nastaví na „zelenou“
	sem_post(sem->out);
	sem_post(sem->akce_id_sem);
	sem_post(sem->adultQueue_sem);
	sem_post(sem->childQueue_sem);
	sem_post(sem->adult_sem);
	sem_post(sem->child_sem);
	sem_post(sem->pom_sem);
	sem_post(sem->wait_ch);
	sem_post(sem->leave_ad);
	sem_post(sem->finish_sem);
	sem_post(sem->count_sem);

	sem_wait(sem->adultQueue_sem);
	sem_wait(sem->childQueue_sem);
	sem_wait(sem->finish_sem);

/*----------------------------------------------------------------------*/
	/* Intializes random number generator --> na bazi aktualniho data*/
	srand(time(NULL));

	//-------------
	*akce_id = 0;
	*adults_sh = 0;				//number of entered adults
	*children_sh = 0;			//number of entered children
	*waiting_child = 0;
	*leaving_adult = 0;
	*count_process = 0;

	//-------------
	//Dva pomocné procesy pro generování procesů adult a child. 
	//Poté čeká na ukončení všech procesů, které aplikace vytváří

	pid_t adult = fork();	
	pid_t child = 0;
  	if(adult == 0){
        generate_adult(args, akce_id, sem, adults_sh, waiting_child, children_sh, OUTP, leaving_adult, count_process);
    	exit(EXIT_SUCCESS);
  	}

  	child = fork();
    if(child == 0){
        generate_child(args, akce_id, sem, children_sh, adults_sh, waiting_child, OUTP, leaving_adult, count_process);
    	exit(EXIT_SUCCESS);
  	}

  	waitpid(adult, NULL, 0);
  	waitpid(child, NULL, 0);

  	//----------------------Dealokace------------------------------
  	fclose(OUTP);

  	sem_close(sem->out);
  	sem_close(sem->akce_id_sem);
  	sem_close(sem->adultQueue_sem);
  	sem_close(sem->childQueue_sem);
  	sem_close(sem->adult_sem);
  	sem_close(sem->child_sem);
  	sem_close(sem->pom_sem);
  	sem_close(sem->wait_ch);
  	sem_close(sem->leave_ad);
  	sem_close(sem->finish_sem);
  	sem_close(sem->count_sem);

  	sem_unlink(OUT);
  	sem_unlink(AKCE_ID);
  	sem_unlink(ADULT_QUEUE);
  	sem_unlink(CHILD_QUEUE);
  	sem_unlink(ADULT_SEM);
  	sem_unlink(CHILD_SEM);
  	sem_unlink(POM_SEM);
  	sem_unlink(WAIT_CH);
  	sem_unlink(LEAVE_AD);
  	sem_unlink(FINISH_SEM);
  	sem_unlink(CNT_SEM);

  	//Dealokace odkazu
  	shmdt(akce_id);
  	shmdt(adults_sh);
  	shmdt(children_sh);
  	shmdt(waiting_child);
  	shmdt(leaving_adult);
  	shmdt(count_process);

  	//Dealokace sdil. pameti
  	shmctl(akce,IPC_RMID,NULL);
  	shmctl(adult_id,IPC_RMID,NULL);
  	shmctl(child_id,IPC_RMID,NULL);
  	shmctl(waiting,IPC_RMID,NULL);
  	shmctl(leaving,IPC_RMID,NULL);
  	shmctl(cnt_proc,IPC_RMID,NULL);

	return ERROR_OK;
}






int main(int argc, char *argv[])
{
  	sParams args; 
  	semaphores sem;                              
  	int err;
  //Naplnit strukturu args vstupnimy argumenty
  	get_args(&args, argc, argv);

  	FILE *OUTP = fopen("proj2.out", "w");
  	//OUTP = stdout;
  	err = allocation(&sem, &args, OUTP);
  	
  	if(err != ERROR_OK){
  		printErr(err);
  	}
  	
  	return 0;
}

