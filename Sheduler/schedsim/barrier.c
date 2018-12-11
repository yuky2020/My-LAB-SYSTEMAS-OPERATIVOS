#include "barrier.h"
#include <errno.h>


#ifdef SEM_BARRIER // Optional Part

/* P1 a b c... -> Name of the Task / Task priority / Arrival time / CPU bursts...
 * P2 d e f...
 * */

int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{
	// Assign number of threads to the
	barrier->max_threads = nr_threads; // Assign max number of threads (CPUs)
	barrier->num_threads_arrived = 0; // Initialize to 0
	sem_init(&barrier->mutex,0,1); // Initialize the semaphore
	sem_init(&barrier->queue,0,0); // Initialize the run queue
	return 0;
}

/* Destroy barrier resources */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
	sem_destroy(&barrier->queue); // Destroy queue
	sem_destroy(&barrier->mutex); // Destroy mutex
	return 0;
}

/* Main synchronization operation */
int sys_barrier_wait(sys_barrier_t *barrier)
{
	sem_wait(&barrier->mutex); // Wait until posted
	barrier->num_threads_arrived += 1; // Another thread has reached this point
	sem_post(&barrier->mutex); // Post
	if (barrier->num_threads_arrived < barrier->max_threads) { // If more threads can arrive
		sem_wait(&barrier->queue); // Wait until posted
	}
	barrier->num_threads_arrived = 0; // Reset amount of threads arrived
	sem_post(&barrier->queue); // Post run queue
	return 0;
}


#else // Mandatory part


/* Barrier initialization function */
int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{
	barrier->max_threads = nr_threads; // Assign max number of threads (CPUs)
	barrier->nr_threads_arrived = 0; // Initialize to 0
	pthread_mutex_init(&barrier->mutex, NULL); // Initialize mutex
	pthread_cond_init(&barrier->cond, NULL); // Initialize condition
	return 0;
}

/* Destroy barrier resources */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
	pthread_mutex_destroy(&barrier->mutex); // Destroy mutex
	pthread_cond_destroy(&barrier->cond); // Destroy condition
	return 0;
}

/* Main synchronization operation */
int sys_barrier_wait(sys_barrier_t *barrier)
{

	/* Implementation outline:
	   - Every thread arriving at the barrier acquires the lock and increments the nr_threads_arrived
	    counter atomically
	     * In the event this is not the last thread to arrive at the barrier, the thread
	       must block in the condition variable
	     * Otherwise...
	        1. Reset the barrier state in preparation for the next invocation of sys_barrier_wait() and
	        2. Wake up all threads blocked in the barrier
	   - Don't forget to release the lock before returning from the function

	    ... To be completed ....
	*/

	pthread_mutex_lock(&barrier->mutex); // Lock the mutex
	barrier->nr_threads_arrived += 1; // Number of threads arrives increments

	if (barrier->nr_threads_arrived < barrier->max_threads) { // If the amount of threads arrived to the barrier is less than the maximum permitted
		pthread_cond_wait(&barrier->cond, &barrier->mutex); // Wait until signal
	} else{
		barrier->nr_threads_arrived = 0; // Reset amount of threads arrived
		pthread_cond_broadcast(&barrier->cond); // Broadcast condition to every thread
	}

	pthread_mutex_unlock(&barrier->mutex); // Unlock mutex

	return 0;
}

#endif /* POSIX_BARRIER */
