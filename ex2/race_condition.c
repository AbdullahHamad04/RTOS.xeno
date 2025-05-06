#include <alchemy/task.h>
#include <alchemy/mutex.h>
#include <stdio.h>
#include <unistd.h>

RT_TASK task1, task2;
RT_MUTEX lock;
int shared_counter = 0;

void task_func(void *arg) {
    char *name = (char *)arg;

    rt_mutex_acquire(&lock, TM_INFINITE); // lock

    for (int i = 0; i < 10000; i++) {
        shared_counter++;
    }

    rt_mutex_release(&lock); // unlock

    printf("%s done. shared_counter = %d\n", name, shared_counter);
}

int main() {
    printf("starting mutex demo...\n");

    rt_mutex_create(&lock, "SharedLock");

    rt_task_create(&task1, "Task1", 0, 50, 0);
    rt_task_create(&task2, "Task2", 0, 50, 0);

    rt_task_start(&task1, &task_func, "Task1");
    rt_task_start(&task2, &task_func, "Task2");

    pause();
    return 0;
}

