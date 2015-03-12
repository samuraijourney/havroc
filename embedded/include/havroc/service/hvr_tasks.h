/*
 * hvr_tasks.h
 *
 */

#ifndef HVR_TASKS_H_
#define HVR_TASKS_H_

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

typedef enum _hvr_task_Idx
{
	//put tasks here in order as above
	TASK_1 = 0,
	//do not remove TASK_IDX_COUNT and leave it as
	//last item as it maintains task count
	TASK_IDX_COUNT
} hvr_task_Idx;

void hvr_tasks_init();
void hvr_tasks_changePriority(hvr_task_Idx taskId, int priority);
int hvr_tasks_getPriority(hvr_task_Idx taskId);
Task_Handle hvr_tasks_getTaskHandle(hvr_task_Idx taskId);

#endif
