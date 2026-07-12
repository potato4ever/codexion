#include "codexion.h"

long	now_ms(void)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	return (now.tv_sec * 1000L + now.tv_usec / 1000L);
}

void	ms_to_timespec(long target_ms, struct timespec *ts)
{
	ts->tv_sec = target_ms / 1000L;
	ts->tv_nsec = (target_ms % 1000L) * 1000000L;
}
