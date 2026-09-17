/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zabelhac <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 17:03:29 by zabelhac          #+#    #+#             */
/*   Updated: 2026/09/13 17:15:30 by zabelhac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *heap, int capacity)
{
	heap->items = malloc(sizeof(*heap->items) * (size_t)capacity);
	if (!heap->items)
		return (0);
	heap->len = 0;
	heap->cap = capacity;
	return (1);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->len = 0;
	heap->cap = 0;
}

int	heap_push(t_sim *sim, t_heap *heap, t_request *request)
{
	if (heap->len == heap->cap)
		return (0);
	heap->items[heap->len] = request;
	heap->len++;
	heap_sift_up(sim, heap, heap->len - 1);
	return (1);
}

t_request	*heap_peek(t_heap *heap)
{
	if (heap->len == 0)
		return (NULL);
	return (heap->items[0]);
}

t_request	*heap_pop(t_sim *sim, t_heap *heap)
{
	t_request	*top;

	if (heap->len == 0)
		return (NULL);
	top = heap->items[0];
	heap->len--;
	if (heap->len > 0)
	{
		heap->items[0] = heap->items[heap->len];
		heap_sift_down(sim, heap, 0);
	}
	return (top);
}

int	heap_remove(t_sim *sim, t_heap *heap, t_request *request)
{
	int	index;

	index = 0;
	while (index < heap->len && heap->items[index] != request)
		index++;
	if (index == heap->len)
		return (0);
	heap->len--;
	if (index == heap->len)
		return (1);
	heap->items[index] = heap->items[heap->len];
	if (index > 0 && request_before(sim, heap->items[index],
			heap->items[(index - 1) / 2]))
		heap_sift_up(sim, heap, index);
	else
		heap_sift_down(sim, heap, index);
	return (1);
}
