/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:49 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 20:02:09 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static int	self_ready_locked(t_coder *coder, long now,
		t_dongle *first, t_dongle *second)
{
	t_request	*req;

	if (first == second)
		return (0);
	req = &coder->request;
	if (heap_peek(&first->queue) != req)
		return (0);
	if (heap_peek(&second->queue) != req)
		return (0);
	if (first->busy || second->busy)
		return (0);
	if (first->cooldown_until > now || second->cooldown_until > now)
		return (0);
	return (1);
}

static void	claim_pair_locked(t_coder *coder, long now,
		t_dongle *first, t_dongle *second)
{
	heap_pop(coder->sim, &first->queue);
	if (second != first)
		heap_pop(coder->sim, &second->queue);
	first->busy = 1;
	second->busy = 1;
	coder->request.queued = 0;
	coder->request.owns_pair = 1;
	coder->last_start = now;
	coder->request.deadline = now + coder->sim->config.time_to_burnout;
}

int	try_acquire_self_locked(t_coder *coder)
{
	t_sim		*sim;
	t_dongle	*first;
	t_dongle	*second;
	long		now;
	int			granted;

	sim = coder->sim;
	if (sim->stopped)
		return (0);
	first = &sim->dongles[left_dongle(coder->id)];
	second = &sim->dongles[right_dongle(sim, coder->id)];
	now = now_ms();
	lock_pair(sim, left_dongle(coder->id), right_dongle(sim, coder->id));
	granted = self_ready_locked(coder, now, first, second);
	if (granted)
		claim_pair_locked(coder, now, first, second);
	unlock_pair(sim, left_dongle(coder->id), right_dongle(sim, coder->id));
	return (granted);
}

void	request_pair_locked(t_coder *coder)
{
	int			left;
	int			right;
	t_sim		*sim;
	t_dongle	*first;
	t_dongle	*second;

	sim = coder->sim;
	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	first = &sim->dongles[left];
	second = &sim->dongles[right];
	coder->request.sequence = sim->next_sequence++;
	coder->request.deadline = coder->last_start + sim->config.time_to_burnout;
	coder->request.queued = 1;
	lock_pair(sim, left, right);
	heap_push(sim, &first->queue, &coder->request);
	if (second != first)
		heap_push(sim, &second->queue, &coder->request);
	unlock_pair(sim, left, right);
}

void	release_pair_locked(t_coder *coder)
{
	t_sim	*sim;
	int		left;
	int		right;
	long	now;

	if (!coder->request.owns_pair)
		return ;
	sim = coder->sim;
	left = left_dongle(coder->id);
	right = right_dongle(sim, coder->id);
	now = now_ms();
	release_dongles(sim, left, right, now);
	coder->request.owns_pair = 0;
}
