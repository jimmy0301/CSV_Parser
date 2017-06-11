#include <stdio.h>
#include <stdlib.h>

#include "list.h"


void insert_node(index_node *list, index_node *node)
{
	index_node *tmp;
	if (list == NULL || node == NULL)
		return;

	tmp = list;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	node->next = tmp->next;
	tmp->next = node;
}

index_node* create_node(int index)
{
	index_node *first_node = (index_node *)malloc(sizeof(index_node));
	if (first_node == NULL)
		return NULL;

	first_node->index = index;
	first_node->next = NULL;

	return first_node;
}

void print_lists(index_node *lists)
{
	index_node *n = lists;

	while (n != NULL)
	{
		printf("%d ", n->index);

		n = n->next;
	}

	printf("\n");
}

void free_lists(index_node *lists)
{
	if (lists->next != NULL)
	{
		free_lists(lists->next);
	}

	if (lists != NULL) {
		free(lists);
	}
}
