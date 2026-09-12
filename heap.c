#include "codexion.h"

int	request_before(t_sim *sim, t_request *a, t_request *b)
{
	if (sim->config.policy == POLICY_EDF && a->deadline != b->deadline)
		return (a->deadline < b->deadline);
  return (a->sequence < b->sequence);
}

static void	swap_request(t_request **a, t_request **b)
{
	t_request	*temporary;

	temporary = *a;
	*a = *b;
	*b = temporary;
}

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
	int	index;

	if (heap->len == heap->cap)
		return (0);
	index = heap->len++;
	heap->items[index] = request;
	while (index > 0 && request_before(sim, heap->items[index],
			heap->items[(index - 1) / 2]))
	{
		swap_request(&heap->items[index], &heap->items[(index - 1) / 2]);
		index = (index - 1) / 2;
	}
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
	int		index;
	int		child;

	if (heap->len == 0)
		return (NULL);
	top = heap->items[0];
	heap->len--;
	if (heap->len == 0)
		return (top);
	heap->items[0] = heap->items[heap->len];
	index = 0;
	while (1)
	{
		child = index * 2 + 1;
		if (child >= heap->len)
			break ;
		if (child + 1 < heap->len && request_before(sim,
				heap->items[child + 1], heap->items[child]))
			child++;
		if (!request_before(sim, heap->items[child], heap->items[index]))
			break ;
		swap_request(&heap->items[child], &heap->items[index]);
		index = child;
	}
	return (top);
}

int	heap_remove(t_sim *sim, t_heap *heap, t_request *request)
{
	int	index;
	int	parent;
	int	child;

	index = 0;
	while (index < heap->len && heap->items[index] != request)
		index++;
	if (index == heap->len)
		return (0);
	heap->len--;
	if (index == heap->len)
		return (1);
	heap->items[index] = heap->items[heap->len];
	parent = (index - 1) / 2;
	if (index > 0 && request_before(sim, heap->items[index], heap->items[parent]))
	{
		while (index > 0 && request_before(sim, heap->items[index],
				heap->items[(index - 1) / 2]))
		{
			swap_request(&heap->items[index], &heap->items[(index - 1) / 2]);
			index = (index - 1) / 2;
		}
		return (1);
	}
	while (1)
	{
		child = index * 2 + 1;
		if (child >= heap->len)
			break ;
		if (child + 1 < heap->len && request_before(sim,
				heap->items[child + 1], heap->items[child]))
			child++;
		if (!request_before(sim, heap->items[child], heap->items[index]))
			break ;
		swap_request(&heap->items[child], &heap->items[index]);
		index = child;
	}
	return (1);
}
