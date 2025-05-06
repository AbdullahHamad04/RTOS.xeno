#include <alchemy/task.h>
#include <alchemy/queue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

RT_TASK writer_task, reader_task;
RT_QUEUE queue;

#define CHUNK_TEXT   "Hello from writer! "
#define REPEAT_COUNT 5000

void writer(void *arg) {
    char data[2048] = "";
    for (int i = 0; i < REPEAT_COUNT; i++) {
        strcat(data, CHUNK_TEXT);
        if (strlen(data) > 1024) break;
    }
    strcat(data, "\n");

    printf("Writer: sending message…\n");
    fflush(stdout);

    size_t len = strlen(data) + 1;
    void *msg = rt_queue_alloc(&queue, len);
    memcpy(msg, data, len);
    rt_queue_send(&queue, msg, len, Q_NORMAL);

    printf("Writer: message sent (%ld bytes)\n", len - 1);
    fflush(stdout);
}

void reader(void *arg) {
    printf("Reader: waiting for message…\n");
    fflush(stdout);

    void *msg;
    rt_queue_receive(&queue, &msg, TM_INFINITE);

    char *txt = (char *)msg;
    printf("Reader: received (%ld bytes)\n", strlen(txt));
    printf("Reader: sample -> %.60s…\n", txt);
    fflush(stdout);

    rt_queue_free(&queue, msg);
}

int main() {
    printf("starting readers–writers demo using queue…\n");

    // create a queue that holds up to 1 message of up to 2048 bytes
    rt_queue_create(&queue, "RWQueue", 1, 2048, Q_FIFO);

    rt_task_create(&reader_task, "Reader", 0, 50, 0);
    rt_task_create(&writer_task, "Writer", 0, 50, 0);

    rt_task_start(&reader_task, &reader, NULL);
    sleep(1);  // ensure reader is blocked on receive
    rt_task_start(&writer_task, &writer, NULL);

    sleep(2);  // let them finish
    rt_queue_delete(&queue);
    return 0;
}
