#include <stdio.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>

RT_TASK t1, t2;
RT_SEM  mutex;

void sem_task(void *arg)
{
    int id = (int)(long)arg;
    for (int i = 0; i < 3; ++i) {
        //rt_sem_p(&mutex, TM_INFINITE); 

        rt_printf("Task %d entering critical section\n", id);
        rt_task_sleep(1000000000);        /* 1 s */
        rt_printf("Task %d leaving critical section\n", id);

        //rt_sem_v(&mutex);   
        rt_task_sleep(500000000);         /* 0.5 s */
    }
}

int main(void)
{
    /* 1) Create a mutex semaphore (initial count 1) */
    rt_sem_create(&mutex, "mutex", 1, S_PRIO);

    /* 2) Two tasks at same priority to “fight” for the semaphore */
    rt_task_create(&t1, "sem1", 0, 40, 0);
    rt_task_create(&t2, "sem2", 0, 40, 0);

    /* 3) Start them */
    rt_task_start(&t1, &sem_task, (void*)(long)1);
    rt_task_start(&t2, &sem_task, (void*)(long)2);

    pause();
    return 0;
}
