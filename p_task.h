#define P_MAX_TASKS 30

void p_task_add(uint id, float importance, boolean (*func)(uint id));
void p_task_compute(uint count);

#define P_ENOUGH_SLOT 2
