#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#include "params.h"
#include "util.h"
#include <string.h>



void insque_arc(inode_t *elem) 
{
	const char *fname = elem->fname;

	if(findelem_list(T1,fname))
	{
		//in t1
		rmelem_list(T1, fname);
		addtolist(T2, elem);
		//return cached value
		return;
	}

	if(findelem_list(T2,fname))
	{
		//in t2
		rmelem_list(T2, fname);
		addtolist(T2, elem);
		//return cached value
		return;
	}

	if(findelem_list(B1,fname))
	{
		//in b1
		int max = MAX(get_size(B2)/ get_size(B1), 1);
		arc->p = MIN(arc->c, arc->p + max);
		replace(fname);
		rmelem_list(B1, fname);
		addtolist(T2, elem);
	}

	if(findelem_list(B2,fname))
	{
		//in b2
		int m1 = MAX(get_size(B1)/ get_size(B2), 1);
		arc->p = MAX(0, arc->p - m1);
		replace(fname);
		rmelem_list(B2, fname);
		addtolist(T2, elem);
	}


	if((get_size(T1) + get_size(B1)) == arc->c)
	{
		if(get_size(T1) < arc->c)
		{
			// fusion needs to know about this
			remque_head_arc(B1);
			replace(fname);
		}else
		 {
		 	//
		 }
	}else
	 {
	 	int total = get_size(T1) + get_size(B1) + get_size(T2) + get_size(B2);
	 	if(total >= arc->c)
	 	{
	 		if(total == (2 * arc->c))
	 		{
	 			remque_head_arc(B2);
	 		}
	 		replace(fname);
	 	}
	 }
	addtolist(T1, elem);
}

/* Helper function to append an elem to a list */

void addtolist(int list_index, inode_t *elem)
{
	inode_t* list_head = arc_heads[list_index];
	inode_t* list_tail = arc_tails[list_index];
	arc_list_size[list_index] += 1;
	// Add the elem to the LRU Q tail
	if (list_head == NULL)  // If Q is empty 
	{ 
		elem->next = elem;
		elem->prev = elem;
		insque(elem, elem);	
		arc_heads[list_index] = elem;
		arc_tails[list_index] = elem;
	}
	else  // Othersise only update the tail
	{ 
		insque(elem, arc_tails[list_index]);
		arc_tails[list_index] = elem;
	}
}



/**
 * Remove a particular element of ARC Q given a file name
 */
void rmelem_arc(const char *fname) 
{
	//log_msg("\nrmelem_arc\n");
    int list_index = 0;
    
	do 
	{
    	rmelem_list(list_index++, fname);	
	} while(list_index < 4);	//this is an array that points to the arc list heads, pos 4  == NULL
	
}


void rmelem_list(int list_index, const char *fname)
{
		inode_t *found = findelem_list(list_index, fname);
		
		if (NULL != found) 
		{
			if (found == arc_heads[list_index])  //if it's the head
			{ 
				remque_head_arc(list_index);
			}
			else if (found == arc_tails[list_index])  //if it's tail
			{
				arc_tails[list_index] = found->prev;
				remque(found);
				free(found);
			}
			else 
			{
				remque(found);
				free(found);				
			}	
		}

}

void  remque_arc(char *fname)
{
	if(arc_heads[B1] != NULL)
	{
		strcpy(fname, arc_heads[B1]->fname);
		remque_head_arc(B1);
	}else if(arc_heads[B2] != NULL)
	{
		strcpy(fname, arc_heads[B2]->fname);
		remque_head_arc(B2);
	}
}



void remque_head_arc(int list_index) 
{

	if (arc_heads[list_index] == NULL)
	{
		//schfs_error("remque_lru ARC Q is alreay empty");
	}

	inode_t *head = arc_heads[list_index];

	//only one elem
	if (head == head->next)  
	{
		arc_heads[list_index] = NULL;
		arc_tails[list_index] = NULL;
	}else
	 {
		 //reset the ARC head
		arc_heads[list_index] = head->next;	
	 }
	arc_list_size[list_index] -= 1;
	remque(head);
	free(head);
}



inode_t* remove_node(int list_index)
{

	if (arc_heads[list_index] == NULL)
	{
		//schfs_error("remque_lru ARC Q is alreay empty");
	}

	inode_t *head = arc_heads[list_index];

	//only one elem
	if (head == head->next)  
	{
		// need to fix this
		arc_heads[list_index] = NULL;
		arc_tails[list_index] = NULL;
	}else
	 {
	 	//reset the ARC head
	 	arc_heads[list_index] = head->next;
	 }
	remque(head);
	return head;
}





inode_t *findelem_arc(char *fname)
{
	//log_msg("\nrmelem_arc\n");
    int list_index = 0;
    
	do 
	{
    	findelem_list(list_index++, fname);	
	} while(list_index < 4);	//this is an array that points to the arc list heads, pos 4  == NULL
}
/**
 * Find a particular elem of ARC, only return the first match
 */
inode_t* findelem_list(int list_index, const char *fname) 
{
	//log_msg("\nfindelem_arc\n");
	
	if (NULL == arc_heads[list_index]) 
		return NULL;	
	
	inode_t *curr = arc_heads[list_index];
	do 
	{
		if (strcmp(curr->fname, fname) == 0)  //found match in LRU Q
		{
			return curr;
		}
		
		curr = curr->next;
	} while(curr != arc_heads[list_index]);	//this is a double cyclic list
	return NULL;
}

void replace(const char *fname)
{
	inode_t *old = NULL;
	if( (findelem_list(B2, fname) && get_size(T1) == arc->p) ||
		get_size(T1) > arc->p)
	{
		old = remove_node(T1);
		addtolist(B1, old);
	}else
	 {
	 	old = remove_node(T2);
	 	addtolist(B2, old);
	 }
	 //remove from ssd
}


int arc_cache_has_files()
{
	return arc_heads[B1] != NULL || arc_heads[B2] != NULL; 
}

void intialize_arc(arc_p * ac)
{
	arc = ac;
	arc_heads = arc->arc_heads;
	arc_tails = arc->arc_tails;
	arc_list_size = arc->arc_list_size;
	arc->c = 10;
	arc->p = 0;
	int i = 0;
	for (; i < 4; ++i)
	{
		arc_heads[i] = NULL;
		arc_tails[i] = NULL;
		arc_list_size[i] = 0;
	}

}


inode_t *create_node(char *name)
{
	inode_t *elem = (inode_t*)calloc(1,sizeof(inode_t)); 
	strcpy(elem->fname, name );
	return elem; 
}

int get_size(int list_index)
{
	return arc->arc_list_size[list_index];
}

/*int main()
{

	intialize_arc();
	insque_arc(create_node("Dwayne"));
	insque_arc(create_node("Dwayne"));
	insque_arc(create_node("Dwayne"));
	//inode_t *e = arc_heads[0];

}*/

