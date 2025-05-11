/* mc_pi.c
 * Project 5 – Task 3: Monte Carlo π with Xenomai Message Queue
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alchemy/task.h>
#include <alchemy/queue.h>

#define NUM_WORKERS        4         /* number of parallel Monte Carlo tasks */
#define POINTS_PER_WORKER 100000     /* samples per worker */

typedef struct {
    long inside;   /* points inside the quarter-circle */
    long total;    /* total points generated (always POINTS_PER_WORKER) */
} mc_result_t;

/* Global Xenomai objects */
static RT_TASK  worker_tasks[NUM_WORKERS];
static RT_QUEUE result_queue;

/* 
 * mc_worker()
 *   Each real-time worker generates POINTS_PER_WORKER random (x,y) ∈ [0,1]²,
 *   counts how many fall inside x² + y² ≤ 1, and sends back its mc_result_t.
 */
void mc_worker(void *arg)
{
    int id = (int)(long)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (id * 0x9e3779b9U);
    mc_result_t res = { .inside = 0, .total = POINTS_PER_WORKER };

    for (int i = 0; i < POINTS_PER_WORKER; ++i) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x*x + y*y <= 1.0) {
            res.inside++;
        }
    }

    /* Copy our small struct into the queue’s internal buffer (blocking if full) */
    if (rt_queue_write(&result_queue, &res, sizeof(res), TM_INFINITE) < 0) {
        rt_printf("Worker %d: queue write failed\n", id);
    }
}

int main(void)
{
    int err;
    long total_inside = 0, total_points = 0;
    mc_result_t partial;

    /* 1) Create the message queue: slot size = sizeof(mc_result_t), slots = NUM_WORKERS */
    err = rt_queue_create(&result_queue,
                          "MCQueue",
                          sizeof(mc_result_t),
                          NUM_WORKERS,
                          Q_FIFO);
    if (err < 0) {
        fprintf(stderr, "rt_queue_create failed: %s\n", strerror(-err));
        return EXIT_FAILURE;
    }

    /* 2) Launch NUM_WORKERS real-time tasks */
    for (long i = 0; i < NUM_WORKERS; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "MCWorker%ld", i);

        err = rt_task_create(&worker_tasks[i], name, 0, 50, 0);
        if (err < 0) {
            fprintf(stderr, "rt_task_create(%s) failed: %s\n",
                    name, strerror(-err));
            return EXIT_FAILURE;
        }
        rt_task_start(&worker_tasks[i], mc_worker, (void *)i);
    }

    /* 3) Collect each worker’s result */
    for (int i = 0; i < NUM_WORKERS; ++i) {
        if (rt_queue_read(&result_queue, &partial, sizeof(partial), TM_INFINITE) < 0) {
            fprintf(stderr, "rt_queue_read failed\n");
            return EXIT_FAILURE;
        }
        total_inside += partial.inside;
        total_points += partial.total;
    }

    /* 4) Compute and print π estimate */
    double pi_est = 4.0 * (double)total_inside / total_points;
    printf("Estimated π = %.6f using %ld points\n",
           pi_est, total_points);

    /* 5) Cleanup */
    rt_queue_delete(&result_queue);
    return EXIT_SUCCESS;
}
