#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*	
Allocate a linked list node with a given key
Allocate a node using malloc(),
initialize the pointers to NULL,
set the key value to the key provided by the argument
 */
struct list_node *allocate_node_with_key(int key)
{
	struct list_node* temp;
	temp = (struct list_node*)malloc(sizeof(struct list_node ));
	temp->key = key;
	temp->next = NULL;
	temp-> prev = NULL;
	temp -> value = NULL;
	return temp;
}

/*	
Initialize the key values of the head/tail list nodes (I used -1 key values)
Also, link the head and tail to point to each other
 */
void initialize_list_head_tail(struct list_node *head, struct list_node *tail)
{
	head-> key = -1;
	tail -> key = -1;
	head -> next = tail;
	head -> prev = tail;
	tail -> next = head;
	tail -> prev = head;
}

/*	
Insert the *new_node* after the *node*
 */
void insert_node_after (struct list_node *node, struct list_node *new_node)
{
	new_node->next = node->next; //switch the next and prev nodes 
	(node->next)->prev = new_node;
	new_node->prev = node;
	node->next = new_node;
}

/*	
Remove the *node* from the list
 */
void del_node (struct list_node *node)
{
	(node->prev)->next = node-> next; //remove from list
	(node->next)->prev = node-> prev; //link the next and prev nodes
	free(node); //free the allocated data
}

/*	
Search from the head to the tail (excluding both head and tail,
as they do not hold valid keys), for the node with a given *search_key*
and return the node.
You may assume that the list will only hold nodes with unique key values
(No duplicate keys in the list)
 */
struct list_node *search_list (struct list_node *head, int search_key)
{
	struct list_node* temp;
	temp = head->next;
	while(temp->key !=-1){
		if(temp->key == search_key) return temp;
		temp=temp->next;
	}
	return NULL;
}

/*	
Count the number of nodes in the list (excluding head and tail), 
and return the counted value
 */
int count_list_length (struct list_node *head)
{
	struct list_node* temp = head->next;
	int cnt=0; //count the times of loop
	while(temp->key != -1){
		cnt++;
		temp = temp->next;
	}
	return cnt;
}

/*	
Check if the list is empty (only head and tail exist in the list)
Return 1 if empty. Return 0 if list is not empty.
 */
int is_list_empty (struct list_node *head)
{
	if((head->next)->key == -1) return 1;
	else return 0;
}

/*	
Loop through the list and print the key values
This function will not be tested, but it will aid you in debugging your list.
You may add calls to the *iterate_print_keys* function in the test.c
at points where you need to debug your list.
But make sure to test your final version with the original test.c code.
 */
void iterate_print_keys (struct list_node *head)
{
	struct list_node* temp=head->next;
	while(temp->key != -1){
		printf("%d\n",temp->key); //print everytime
		temp = temp->next;
	}
}

/*	
Insert a *new_node* at the sorted position so that the keys of the nodes of the
list (including the key of the *new_node*) is always sorted (increasing order)
 */
int insert_sorted_by_key (struct list_node *head, struct list_node *new_node)
{
	struct list_node* temp = head-> next;
	if(temp->key ==-1) insert_node_after(head,new_node); //if list is empty
	else{ //else
		while((temp->key < new_node->key) && (temp->key != -1)){ //search the proper point
			temp = temp->next;
		}
		insert_node_after(temp->prev,new_node); // add it on
		return 0;
	}	
}
