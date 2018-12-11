#ifndef  BARRIER_H
#define  BARRIER_H
#include <pthread.h>
#include <semaphore.h>

#if defined(__APPLE__) && defined(__MACH__)
#undef POSIX_BARRIER    /* MAC OS X does not implement pthread's barriers so use custom implementation instead */
#endif

#ifdef SEM_BARRIER

	typedef struct{
		sem_t mutex; /* It should be initialized with c=1 (to enforce mutual exclusion) */
		sem_t queue; /* It should be initialized with c=0 (Used as a wait queue) */
		int num_threads_arrived; /* Number of threads that reached the barrier. */
		int max_threads; /* Number of threads that synchronize with the barrier.
						(This value is set up upon barrier creation, and must not
						be modified afterwards) */
	} sys_barrier_t;

#else
/* Synchronization barrier */
typedef struct {
	pthread_mutex_t mutex;		/* Barrier lock */
	pthread_cond_t cond;		/* Condition variable where threads remain blocked */
	int nr_threads_arrived;		/* Number of threads that reached the barrier. */
	int max_threads;			/* Number of threads that rely on the synchronization barrier
    							   (This value is set up upon barrier creation, and must not be modified afterwards) */
} sys_barrier_t;
#endif

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads);
int sys_barrier_destroy(sys_barrier_t* barrier);
int sys_barrier_wait(sys_barrier_t *barrier);

#endif // Barrier


