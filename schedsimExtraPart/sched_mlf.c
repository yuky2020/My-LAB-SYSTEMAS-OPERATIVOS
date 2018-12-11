// Priority Algorithm -> Multi-level Feedback queue (MLF)

/* - 3 Priority Levels (0, 1, and 2)-
 */

// Use rr_quantum (global variable) -- For quantum slices from Rouond Robin for scheduling

#include "sched.h"

// 

/* Structure to store MLF thread-specific fields */
struct mlf_data {
	int remaining_ticks_slice; // Added to combine both RR and PRIO (now with quantum)
};

static int task_new_mlf(task_t* t)
{
	struct mlf_data* cs_data=malloc(sizeof(struct mlf_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum depending on the priority*/
	cs_data->remaining_ticks_slice= 2 * (t->prio + 1); // Assign quantum depending on the priority
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_mlf(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_mlf(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static int compare_tasks_cpu_burst(void *t1,void *t2)
{
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->prio-tsk2->prio; // Grabs the lowest
}

/*

- If the task goes to sleep because of an I/O operation then: t->runnable_ticks_left == 1 / Before uring its quantum or when in just expired, increase priority


*/

static void enqueue_task_mlf(task_t* t,runqueue_t* rq, int preempted)
{


	struct mlf_data* cs_data=(struct mlf_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t)) // If in the run queue, or idle, exit
		return;

	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task (sorted for simplicity)
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task (sorted for simplicity)
	
	cs_data->remaining_ticks_slice=2 * (t->prio + 1); // Reset slice

    	/* If the task was not using the current CPU, check whether a preemption is in order or not */
	if (!preempted) {
		task_t* current=rq->cur_task;

		/* Trigger a preemption if this task has a shorter CPU burst than current */
		if (preemptive_scheduler && t->prio < current->prio) {
			rq->need_resched=TRUE;
			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
                                                another task with the same length wakes up as well*/
		}
	}

}

static task_t* steal_task_mlf(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}

static void task_tick_multi(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct mlf_data* cs_data=(struct mlf_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
	
	if (current->runnable_ticks_left==1) {
		if (cs_data->remaining_ticks_slice <= 0) {
			current->prio--;
			if (current->prio < 0) current->prio = 0;
		}
	} else if (current->runnable_ticks_left<= 0) {
		current->prio++;
		if (current->prio > 2) current->prio = 2;
	}
	
}


sched_class_t mlf_sched= {
	.task_new=task_new_mlf,
	.task_free=task_free_mlf,
	.task_tick=task_tick_multi,
	.pick_next_task=pick_next_task_mlf,
	.enqueue_task=enqueue_task_mlf,
	.steal_task=steal_task_mlf
};

