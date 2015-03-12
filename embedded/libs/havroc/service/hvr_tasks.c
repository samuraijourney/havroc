/* XDCtools Header files */
#include "havroc/service/hvr_tasks.h"
#include "havroc/service/hvr_tasks_list.h"

#define DEFAULT_TASK_PRIORITY	5

const Task_Params DefaultTaskParams;

typedef struct _hvr_task
{
	Task_Handle handle;
	Task_FuncPtr fxn;
	int priority;
} hvr_task;

hvr_task def = {
		NULL,
		task1Fxn,
		DEFAULT_TASK_PRIORITY
};

hvr_task hvrTasks[] = {
		{NULL, task1Fxn, DEFAULT_TASK_PRIORITY}
};

void hvr_tasks_init()
{
	int i;
	//iterate and initialize tasks
	for (i = 0; i < TASK_IDX_COUNT; i++)
	{
		Task_Params params;
		Task_Params_init(&params);
		params.priority = hvrTasks[i].priority;
		hvrTasks[i].handle = Task_create((Task_FuncPtr)(hvrTasks[i].fxn), &params, NULL);

		if (hvrTasks[i].handle == NULL) {
			System_printf("Task #%d create failed.\n", i);
			System_flush();
		}
	}
}

void hvr_tasks_changePriority(hvr_task_Idx taskId, int priority)
{
	if (taskId < TASK_IDX_COUNT)
	{
		Task_setPri(hvrTasks[taskId].handle, priority);
		hvrTasks[taskId].priority = priority;
	}
}

int hvr_tasks_getPriority(hvr_task_Idx taskId)
{
	if (taskId < TASK_IDX_COUNT)
	{
		return Task_getPri(hvrTasks[taskId].handle);
	}
	return -2;
}

Task_Handle hvr_tasks_getTaskHandle(hvr_task_Idx taskId)
{
	if (taskId < TASK_IDX_COUNT)
	{
		return hvrTasks[taskId].handle;
	}

	return NULL;
}
