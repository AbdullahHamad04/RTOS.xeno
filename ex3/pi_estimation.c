#include <alchemy/task.h>
#include <alchemy/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_TASKS 4
#define POINTS_PER_TASK 10000

RT_TASK workers[NUM_TASKS];
RT_QUEUE result_queue;

typedef struct {
    int inside_circle;
    int total;
} Result;

void worker(void *arg) {
    int inside = 0;

    for (int i = 0; i < POINTS_PER_TASK; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    Result r = {inside, POINTS_PER_TASK};

    // allocate memory from queue
    void *msg = rt_queue_alloc(&result_queue, sizeof(Result));
    if (!msg) {
        printf("Failed to allocate message memory\n");
        return;
    }

    *(Result *)msg = r;
    rt_queue_send(&result_queue, msg, sizeof(Result), Q_NORMAL);
}

int main() {
    srand(time(NULL));
    printf("starting Monte Carlo π estimation...\n");

    rt_queue_create(&result_queue, "PiQueue", 100 * sizeof(Result), Q_UNLIMITED, Q_FIFO);

    for (int i = 0; i < NUM_TASKS; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Worker%d", i);
        rt_task_create(&workers[i], name, 0, 50, 0);
        rt_task_start(&workers[i], &worker, NULL);
    }

    int total_inside = 0, total_points = 0;

    for (int i = 0; i < NUM_TASKS; i++) {
        void *msg;
        rt_queue_receive(&result_queue, &msg, TM_INFINITE);
        Result *r = (Result *)msg;
        total_inside += r->inside_circle;
        total_points += r->total;
        rt_queue_free(&result_queue, msg);
    }

    double pi_estimate = 4.0 * total_inside / total_points;
    printf("Estimated π = %.6f using %d points\n", pi_estimate, total_points);

    rt_queue_delete(&result_queue);
    return 0;
}

