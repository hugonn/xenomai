#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <rtdk.h>

#define ITER1 15
#define ITER2 30
#define ITER3 15

#define HIGH 90 /* high priority */
#define MID 30 /* medium priority */
#define LOW 10  /* low priority */


static RT_TASK t1;
static RT_TASK t2;
static RT_TASK t3;
int global = 0;
int global2 = 0;

RT_SEM semGlobal;
RT_SEM semGlobal2;


void taskOne(void *arg)
{
	int i;
	rt_task_suspend(&t2);
	rt_task_suspend(&t3);

	RT_TASK_INFO curtaskinfo;
	RT_TASK *curtask;
	curtask=rt_task_self();

	rt_task_inquire(curtask,&curtaskinfo);
	rt_printf("Tarefa Baixa Prioridade Executando \n "); //%s \n", curtaskinfo.name);

	rt_printf("Tarefa Baixa Prioridade Tentando Travar semáforo 1 \n "); //%s \n", curtaskinfo.name);
	rt_sem_p(&semGlobal,TM_INFINITE );
	rt_printf("Tarefa Baixa Prioridade Travou semáforo 1\n");
	for(i=0; i<ITER1; i++){
		
		rt_printf("Tarefa de baixa Prioridade alterando");
		rt_printf(" global = %d................\n",
		++global);
		
		if(i>10)
			rt_task_resume(&t2); 
		
	};
	rt_printf("Tarefa Baixa Prioridade Liberou semáforo 1\n");
	rt_sem_v(&semGlobal);
	
	
}

void taskTwo(void *arg)
{
	
	
	int i;
	RT_TASK_INFO curtaskinfo;
	RT_TASK *curtask;
	curtask=rt_task_self();
	rt_task_inquire(curtask,&curtaskinfo);
	rt_printf("Tarefa Média Prioridade Chegou \n ");
	rt_printf("Tarefa Média Prioridade Executando \n ");
	for(i=0; i<ITER2; i++){

		rt_printf("Tarefa de Média Prioridade alterando");
		global2= global2 + 4;
		rt_printf(" global2 = %d................\n",
		global2);
		if(i > 10)
			rt_task_resume(&t3);		
	}
	
}

void taskThree(void *arg)
{
	int i;
	//rt_task_suspend(&t3);
	
	RT_TASK_INFO curtaskinfo;
	RT_TASK *curtask;
	curtask=rt_task_self();
	rt_task_inquire(curtask,&curtaskinfo);

	rt_printf("Tarefa Média Prioridade Chegou \n ");
	rt_printf("Tarefa Alta Prioridade Executando \n ");
	rt_printf("Tarefa Alta Prioridade Tentando Travar semáforo 1 \n "); 

	rt_sem_p(&semGlobal,TM_INFINITE );
	rt_printf("Tarefa Alta Prioridade Travou semáforo 1\n");

	for(i=0; i<ITER3; i++){
		rt_printf("Tarefa de Alta Prioridade alterando");
		rt_printf(" global = %d................\n",
		++global);
	}

	rt_printf("Tarefa Alta Prioridade Liberou semáforo 1\n");
	rt_sem_v(&semGlobal);	
	//rt_task_wait_period(NULL);
	
	
}


int main(int argc, char* argv[])
{
	

	rt_sem_create(&semGlobal,"semaforo", 0, S_PRIO );
	rt_sem_create(&semGlobal2,"semaforo2", 0, S_PRIO );
	/* Perform auto-init of rt_print buffers if the task doesn't */
	rt_print_auto_init(1);

	rt_timer_set_mode(TM_ONESHOT);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	/* Cria 3 tarefas e suas respectivas prioridades */
	rt_task_create(&t1, "task low", 0, 1, 0);
	rt_task_create(&t2, "task mid", 0, 5, 0);
	rt_task_create(&t3, "task high", 0, 10, 0);
	/*
	rt_task_set_priority(&t1,LOW);
  	rt_task_set_priority(&t2,MID);
	rt_task_set_priority(&t3,HIGH);
	*/

	rt_task_set_periodic(&t1, TM_NOW,  rt_timer_ns2ticks(100e8));
	rt_task_set_periodic(&t2, 160e8, rt_timer_ns2ticks( 200e8));
	rt_task_set_periodic(&t3, 200e8, rt_timer_ns2ticks( 100e8 ));

	
	/* start the two tasks */
	rt_task_start(&t1, &taskOne, 0);
	rt_task_start(&t2, &taskTwo, 0);
	rt_task_start(&t3, &taskThree, 0);
	
	
	rt_sem_broadcast(&semGlobal);
	rt_sem_broadcast(&semGlobal2);	
			
	return 0;
}
