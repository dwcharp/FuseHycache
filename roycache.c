/* Second Chance Caching 
 * Developed by Long Nangong
 * lnangong@hawk.iit.edu
 * 11/7/2013             
 */


#include "royparams.h"
#include "util.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include "log.h"


/*search element in the Q (it's double cyclic Q)*/
inode_t* findelem_scc(const char *fname)
{
        log_msg("\nfindelem_scc\n");

        if (FUSION_DATA->scc_head == NULL)
                return NULL;

        inode_t *curr = FUSION_DATA->scc_head;
        do
        {
                if (strcmp(curr->fname, fname) == 0) //found match in LRU Q
                {
                        return curr;
                }

                curr = curr->next;
        }
        while (curr != FUSION_DATA->scc_head);  //check each element until pointer points back to the head of Q

        return NULL; // If there's no match in the Q
}

//insert element to the Q 
void ins_Q(inode_t *elem)
{
	//insert new element to tail
	elem->prev = FUSION_DATA->scc_tail; 
	elem->next = FUSION_DATA->scc_tail->next; //it's cyclic Q
	FUSION_DATA->scc_tail->next = elem;
	FUSION_DATA->scc_tail = elem; //new inserted element becomes tail
	FUSION_DATA->scc_head->prev = FUSION_DATA->scc_tail;//it's double cyclicQ
}

//remove an element from the Q
void rem_Q(inode_t *elem)
{
	//only one element in the Q
	if(elem == elem->next) 
	{
		FUSION_DATA->victim = NULL;
	        FUSION_DATA->scc_head = NULL;
       		FUSION_DATA->scc_tail = NULL;
		free(elem);
		return;
	}
	//more than one element in the Q
	else if(elem == FUSION_DATA->scc_head) //if it's head
		FUSION_DATA->scc_head = elem->next;
	
	else if(elem == FUSION_DATA->scc_tail) //if it's tail
		FUSION_DATA->scc_tail = elem->prev;
		
	//remove procedure
	inode_t *prev = elem->prev;
	inode_t *next = elem->next;
	//if this element found by victim pointer, move pointer to the next and remove this elem
	if(FUSION_DATA->victim == elem)
		FUSION_DATA->victim = elem->next;

	prev->next = next;
	next->prev = prev;

	free(elem);

}

/**
 *update reference bit, if file found in the Q
 *if file not found in the Q, insert it into the Q
 *fname is full path ssd
 */
void insque_scc(const char *fname)
{
        log_msg("\ninsque_scc\n");

        // Find the element in the Q
        inode_t *found = findelem_scc(fname);

        // If it is already in the FIFO Q, update the reference bit
        if (found != NULL)     
                found->ref_bit = 1;
	
	//File not found, insert it to the Q
	else
	{
		inode_t *elem = (inode_t *)malloc(sizeof(inode_t));
		strcpy(elem->fname, fname);
		elem->ref_bit = 1;

		// If FIFO Q is empty 
		if (FUSION_DATA->scc_head == NULL)
		{
		 	elem->next = elem;
		        elem->prev = elem;
		        FUSION_DATA->scc_head = elem;
		        FUSION_DATA->scc_tail = elem;
		}
		// FIFO Q is nonempty 
		else
			ins_Q(elem);
		
	}

	log_msg("\ninsque_scc debug: insque files\n");
	inode_t *pt = FUSION_DATA->scc_head; 
	do
	{
		log_msg("%s-->",pt->fname);
		pt = pt->next;
	}while(pt != FUSION_DATA->scc_head);
}

/**
 *find victim file, and remove it from the Q
 *copy back victim's file name to remove it to hdd
 */
void remque_scc(char *fname)
{
        log_msg("\nremque_scc\n");

        if (FUSION_DATA->scc_head == NULL)
                fusion_error("remque_scc FIFO Q is alreay empty");
        
	if(FUSION_DATA->victim == NULL) //first time to use victim pointer, or all cached files have been removed from the Q
		FUSION_DATA->victim = FUSION_DATA->scc_head;

        //only one elememt in the Q
        if (FUSION_DATA->victim == FUSION_DATA->victim->next)
        {
		strcpy(fname,FUSION_DATA->victim->fname); //copy back victim's path to fname to remove file to hdd
                rem_Q(FUSION_DATA->victim); //remove victim from the Q
		
                return;
        }

	//have more than one element in the Q
	while(1) 
	{       //find victim element
		if(FUSION_DATA->victim->ref_bit == 0)
		{
			strcpy(fname,FUSION_DATA->victim->fname);//copy back victim's path to fname to remove file to hdd
			rem_Q(FUSION_DATA->victim); //remove victim from the Q
			log_msg("\nremque_scc--victim dirname = %s \n", fname);
			break;
		}
		else
		{
			//if pointed element's refrence bit is not 0, clear it, move to next elment
			FUSION_DATA->victim->ref_bit = 0;		
			FUSION_DATA->victim = FUSION_DATA->victim->next;
		}
	}
	
	log_msg("\nremque_scc debug: insque files\n");
        inode_t *pt = FUSION_DATA->scc_head;
        do
        {
                log_msg("-->%s",pt->fname);
                pt = pt->next;
        }while(pt != FUSION_DATA->scc_head);
}

//remove specific file from the Q
void rmelem_scc(const char *fname)
{
	log_msg("\nrmelem_scc\n");

	if(FUSION_DATA->scc_head == NULL)
		return;
	
	inode_t *found = findelem_scc(fname); //search file from the Q
	if(found != NULL)
		rem_Q(found); //remove file
	
}
