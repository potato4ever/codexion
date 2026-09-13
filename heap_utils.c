/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:15:18 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 17:15:48 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_before(t_sim *sim, t_request *a, t_request *b)
{
	t_coder	*ca;
	t_coder	*cb;

	ca = &sim->coders[a->coder_id - 1];
	cb = &sim->coders[b->coder_id - 1];
	if (sim->config.policy == POLICY_EDF
		&& a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	if (sim->config.policy == POLICY_FIFO
		&& a->sequence != b->sequence)
		return (a->sequence < b->sequence);
	return (ca->compiles < cb->compiles);
}

void	swap_request(t_request **a, t_request **b)
{
	t_request	*temporary;

	temporary = *a;
	*a = *b;
	*b = temporary;
}

void	heap_sift_up(t_sim *sim, t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_before(sim, heap->items[index], heap->items[parent]))
			break ;
		swap_request(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

void	heap_sift_down(t_sim *sim, t_heap *heap, int index)
{
	int	child;

	while (1)
	{
		child = index * 2 + 1;
		if (child >= heap->len)
			break ;
		if (child + 1 < heap->len
			&& request_before(sim, heap->items[child + 1],
				heap->items[child]))
			child++;
		if (!request_before(sim, heap->items[child], heap->items[index]))
			break ;
		swap_request(&heap->items[child], &heap->items[index]);
		index = child;
	}
}
