

typedef struct node
{
	int index;
	struct node *next;

}index_node;

extern void insert_node(index_node *list, index_node *node);
extern index_node* create_node(int index);
extern void print_lists(index_node* lists);
extern void free_lists(index_node *lists);
