#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>

RT_TASK demo_task;

void demo(void *arg) {
    RT_TASK *curtask;
    RT_TASK_INFO curtaskinfo;

    printf("Hello World!\n");

    curtask = rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    printf("Task name: %s\n", curtaskinfo.name);
}

int main(int argc, char* argv[]) {
    printf("start task\n");

    // 1. create the task
    rt_task_create(&demo_task, "HelloTask", 0, 50, 0);

    // 2. start the task
    rt_task_start(&demo_task, &demo, NULL);

    pause(); // wait forever
    return 0;
}

