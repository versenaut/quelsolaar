#define P_MAX_TASKS 300000

#if defined _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

/* Timer data structure, for use with the p_timer_ calls. */
typedef struct {
#if defined _WIN32
	LARGE_INTEGER	now;
#else
	struct timeval	now;
#endif
} PTimer;

void p_task_add(uint id, float importance, boolean (*func)(uint id));
void p_task_compute(uint count);

extern void	p_timer_start(PTimer *t);		/* Start timer. */
extern double	p_timer_elapsed(const PTimer *t);	/* Returns seconds since timer was started. */

#define P_ENOUGH_SLOT 2
