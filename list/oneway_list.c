#include <stdio.h>
#include <stdlib.h>

struct list_node {
	void *data;
	struct list_node *prev;
	struct list_node *next;
	
};
typedef struct list_node node_t;

struct list {
	int node_cnt;
	node_t *head;
};
typedef struct list list_t;

#define for_each_node(iter, list) for(iter = (list)->head; iter; iter = iter->next)

#define list_head(lst) (((lst)->node_cnt != 0) ? (lst)->head : NULL)

node_t *list_tail(list_t *lst)
{
	node_t *iter, *tail;
	
	for_each_node(iter, lst)
		tail = iter;
	return tail;
}

#define list_size(lst) ((lst)->node_cnt)
#define list_is_head(lst, node) (((lst)->head == node) ? 1 : 0)
#define list_is_tail(lst, node) ((list_tail() == node) ? 1 : 0)

int list_insert_tail(list_t *lst, node_t *node)
{
	node_t *iter, *tail;
	if (lst->node_cnt == 0)
		lst->head = node;
	else {
		tail = list_tail(lst);
		tail->next = node;
	}
	node->next = NULL;
	lst->node_cnt++;
	return 0;
}
int list_insert_head(list_t *lst, node_t *node)
{
	node_t *tmp = lst->head;
	lst->head = node;
	node->next = tmp;
	lst->node_cnt++;
}
int list_insert_next(list_t *lst, node_t *node1, node_t *node2)
{
	node_t *iter;
	
	if (node1 == NULL)
		return -1;
	
	for_each_node(iter, lst) {
		if  (iter == node1) {
			node2->next = node1->next;
			node1->next = node2;
			lst->node_cnt++;
			return 0;
		}
	}
	return -1;
}
node_t *list_del_tail(list_t *lst)
{
	node_t *iter, *tail;
	
	if (lst->node_cnt == 0)
		return NULL;
	
	if (lst->node_cnt == 1) {
		tail = lst->head;
		lst->head = NULL;
	}
	else 
		for_each_node(iter, lst) {
			if (iter->next->next == NULL) {
				tail = iter->next;
				iter->next = NULL;
			}
	}
	lst->node_cnt--;
	return tail; 
}
node_t *list_del_head(list_t *lst)
{
	node_t *tmp;
	
	if (lst->node_cnt == 0)
		return NULL;


	tmp = lst->head;
	lst->head = lst->head->next;
	lst->node_cnt--;
	
	return tmp;
}
int list_del_node(list_t *lst, node_t *node)
{
	node_t *iter, *tmp;
	
	if (node == NULL || lst->node_cnt == 0)
		return -1;
	
	if(node == lst->head)
		lst->head = node->next;
	else {
		for_each_node(iter, lst) 
			if (iter->next == node)
				iter->next = iter->next->next;
	}
	lst->node_cnt--;
	return 0;
}

int main()
{
	node_t *iter;
	list_t list = {0, NULL};

	node_t * nd0 = (node_t *)malloc(sizeof(node_t));
	node_t * nd1 = (node_t *)malloc(sizeof(node_t));
	node_t * nd2 = (node_t *)malloc(sizeof(node_t));
	node_t * nd3 = (node_t *)malloc(sizeof(node_t));
	
	nd0->data = (void *)malloc(sizeof(int));
	nd1->data = (void *)malloc(sizeof(int));
	nd2->data = (void *)malloc(sizeof(int));
	nd3->data = (void *)malloc(sizeof(int));
	*(int *)nd0->data = 0;
	*(int *)nd1->data = 1;
	*(int *)nd2->data = 2;
	*(int *)nd3->data = 3;
	

	list_insert_head(&list, nd0);
	list_insert_head(&list, nd1);
	list_insert_tail(&list, nd3);
	list_insert_next(&list, nd1, nd2);

	for_each_node(iter, &list)
		printf("%d \n", *(int *)(iter->data));
	
	return 0;
}
