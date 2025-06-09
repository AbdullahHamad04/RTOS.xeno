#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <stdio.h>
#include <unistd.h>
RT_TASK demo_task;

void demo(void *arg) {
    rt_task_set_periodic(NULL, TM_NOW, 1e9); // 1 second period
    while (1) {
        rt_task_wait_period(NULL);
        printf("Hello from periodic task!\n");
    }
}
int main(int argc, char* argv[]) {
    printf("Starting periodic task...\n");

    rt_task_create(&demo_task, "DemoTask", 0, 50, 0);
    rt_task_start(&demo_task, &demo, NULL);

    pause(); // wait forever
    return 0;
}
