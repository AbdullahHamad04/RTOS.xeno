#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alchemy/task.h>
#include <alchemy/queue.h>

#define NUM_WORKERS        4
#define POINTS_PER_WORKER 100000

typedef struct {
    int inside;
} mc_result_t;

static RT_TASK  worker_tasks[NUM_WORKERS];
static RT_QUEUE result_queue;

/* Worker function */
void mc_worker(void *arg) {
    int id = (int)(intptr_t)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (id * 0x9e3779b9U);

    mc_result_t res = { .inside = 0 };

    for (int i = 0; i < POINTS_PER_WORKER; ++i) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0)
            res.inside++;
    }

    rt_queue_write(&result_queue, &res, sizeof(res), TM_INFINITE);
}

int main(void) {
    int total_inside = 0;
    const int total_points = NUM_WORKERS * POINTS_PER_WORKER;
    mc_result_t res;

    if (rt_queue_create(&result_queue, "MCQueue", sizeof(mc_result_t), NUM_WORKERS, Q_FIFO) < 0) {
        fprintf(stderr, "Queue creation failed\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_WORKERS; ++i) {
        rt_task_create(&worker_tasks[i], NULL, 0, 50, 0);
        rt_task_start(&worker_tasks[i], mc_worker, (void *)(intptr_t)i);
    }

    for (int i = 0; i < NUM_WORKERS; ++i) {
        if (rt_queue_read(&result_queue, &res, sizeof(res), TM_INFINITE) < 0) {
            fprintf(stderr, "Queue read failed\n");
            return EXIT_FAILURE;
        }
        total_inside += res.inside;
    }

    double pi_est = 4.0 * (double)total_inside / total_points;
    printf("Estimated π = %.6f using %d points\n", pi_est, total_points);

    rt_queue_delete(&result_queue);
    return EXIT_SUCCESS;
}
