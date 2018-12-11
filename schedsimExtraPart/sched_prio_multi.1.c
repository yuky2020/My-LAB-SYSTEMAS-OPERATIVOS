// Priority Algorithm -> Multi-level non-feedback

/* - The algorithm will select to run the next highest priority task in the "ready" state.
 * - EVery task will remain in its associated priority level (prio field in task_t) throughout the execution (fixed-piority scheme)
 * - In each priority level, tasks will be scheduled in a R fashion using the quantum value (-q)
 */

// Use rr_quantum (global variable) -- For quantum slices from Rouond Robin for scheduling

#include "sched.h"

/* Structure to store PRIO_MULTI thread-specific fields */
struct prio_multi_data {
	int priority;
	int remaining_ticks_slice; // Added to combine both RR and PRIO (now with quantum)
};

static int task_new_prio_multi(task_t* t)
{
	struct prio_multi_data* cs_data=malloc(sizeof(struct prio_multi_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum + priority*/
	cs_data->remaining_ticks_slice=rr_quantum; // Assign quantum
	cs_data->priority=t->prio; // Assign priority
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_prio_multi(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_prio_multi(runqueue_t* rq)
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

static void enqueue_task_prio_multi(task_t* t,runqueue_t* rq, int preempted)
{
	struct prio_multi_data* cs_data=(struct prio_multi_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t)) // If in the run queue, or idle, exit
		return;

	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task (sorted for simplicity)
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task (sorted for simplicity)
	
	cs_data->remaining_ticks_slice=rr_quantum; // Reset slice

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

static void task_tick_multi(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct prio_multi_data* cs_data=(struct prio_multi_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; /* Charge tick */

	if (cs_data->remaining_ticks_slice<=0)
		rq->need_resched=TRUE; //Force a resched !!
}

static task_t* steal_task_prio_multi(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}


sched_class_t prio_multi_sched= {
	.task_new=task_new_prio_multi,
	.task_free=task_free_prio_multi,
	.task_tick=task_tick_multi,
	.pick_next_task=pick_next_task_prio_multi,
	.enqueue_task=enqueue_task_prio_multi,
	.steal_task=steal_task_prio_multi
};
