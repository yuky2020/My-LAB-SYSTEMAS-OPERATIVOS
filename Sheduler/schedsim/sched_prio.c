// Main structure of this .c file are taken from the Round Robin (RR) Scheduler

// Priority Algorithm

#include "sched.h"

/* Structure to store RR thread-specific fields */
struct prio_data {
	int priority;
};

static int task_new_prio(task_t* t)
{
	struct prio_data* cs_data=malloc(sizeof(struct prio_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */
	cs_data->priority=t->prio;
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_prio(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_prio(runqueue_t* rq)
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

static void enqueue_task_prio(task_t* t,runqueue_t* rq, int preempted)
{

	if (t->on_rq || is_idle_task(t)) // If in the run queue, or idle, exit
		return;

	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		sorted_insert_slist_front(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task
	} else
		sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_cpu_burst);  // Push task

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

static task_t* steal_task_prio(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t)
		remove_slist(&rq->tasks,t);

	return t;
}


sched_class_t prio_sched= {
	.task_new=task_new_prio,
	.task_free=task_free_prio,
	.pick_next_task=pick_next_task_prio,
	.enqueue_task=enqueue_task_prio,
	.steal_task=steal_task_prio
};
