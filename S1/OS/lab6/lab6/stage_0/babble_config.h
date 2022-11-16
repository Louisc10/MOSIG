#ifndef __BABBLE_CONFIG_H__
#define __BABBLE_CONFIG_H__

#define BABBLE_BACKLOG 100

#define BABBLE_PORT 5656
#define MAX_CLIENT 1000
#define MAX_FOLLOW MAX_CLIENT

#define BABBLE_BUFFER_SIZE 256
#define BABBLE_PUB_SIZE 64
#define BABBLE_ID_SIZE 32

#define BABBLE_TIMELINE_MAX 5

#define BABBLE_EXECUTOR_THREADS 1

/* constant to be used starting from Stage 2 */
#define BABBLE_COMMUNICATION_THREADS 8

/* size of the prodcons buffers to be implemented */
#define BABBLE_PRODCONS_SIZE 4

/* expressed in micro-seconds */
#define MAX_DELAY 10000

#endif
