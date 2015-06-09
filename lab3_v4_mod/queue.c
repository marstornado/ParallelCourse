#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./queue.h"
/*
 * queue.h -- public interface to the queue module
 */
#define public
#define private static

typedef struct {
    void *value;
    struct qNode *next;
}qNode;

/* create an empty queue */
public void* qopen(void){
    qNode *head = (qNode*) malloc(sizeof(qNode));
    head->value = NULL;
    head->next = NULL;
    return head;
}

/* deallocate a queue, assuming every element has been removed and deallocated */
public void qclose(void *qp){
    free(qp);
}

/* put element at end of queue */
public void qput(void *qp, void *elementp){
    /*initialize a tempt node with the same value of qp*/
    qNode *tmpt = (qNode*) qp;
    /*if the queue is empty*/
    if (tmpt->value == NULL) tmpt->value = elementp;
    else {
        /*initialize a new node and add it to the queue*/
        qNode *newNode = (qNode*) malloc(sizeof(qNode));
        newNode->value = elementp;
        newNode->next = NULL;
        /*get the pointer of the last element in the queue*/
        for (; tmpt->next != NULL; tmpt = (qNode*)tmpt->next);
        tmpt->next = (struct qNode*)newNode;
    }
}

/* get first element from a queue */
public void* qget(void *qp){
    qNode *tmpt = (qNode*) qp;
    /*if the queue is empty, retun null*/
    if (tmpt->value == NULL) return NULL;
    /*if the only one node in the queue*/
    if (tmpt->next == NULL) {
        void* returnValue = tmpt->value;
        tmpt->value = NULL;
        return returnValue;
    }
    /*because value of qp cannnot be changed, in the case, we copy the value of the second node to first one and delete the second one*/
    else {
        qNode *secNode = (qNode*) tmpt->next;
        void* returnValue = (qNode*) tmpt->value;
        tmpt->next = secNode->next;
        tmpt->value = secNode->value;
        free(secNode);
        return returnValue;
    }
}

/* apply a void function (e.g. a printing fn) to every element of a queue */
public void qapply(void *qp, void (*fn)(void* elementp)){
    qNode *tmpt = (qNode*) qp;
    /*if the queue is empty, return null*/
    if (tmpt->value == NULL) return;
    /*else while until the end of the queue*/
    while (tmpt != NULL) {
        fn(tmpt->value);
        tmpt = (qNode*)tmpt->next;
    }
}

/* search a queue using a supplied boolean function, returns an element */
public void* qsearch(void *qp,
                     int (*searchfn)(void* elementp,void* keyp),
                     void* skeyp){
    qNode *tmpt = (qNode*) qp;
    /*search when queue is not empty*/
    if (tmpt->value != NULL) {
        while (tmpt != NULL) {
            if (searchfn(tmpt->value, skeyp))
                return tmpt->value;
            tmpt = (qNode*) tmpt->next;
        }
    }
    return NULL;
}

/* search a queue using a supplied boolean function, removes an element */
public void* qremove(void *qp,
                     int (*searchfn)(void* elementp,void* keyp),
                     void* skeyp){
    qNode * tmpt = (qNode*) qp;
    void* returnValue;
    /*if the queue is empty, return empty*/
    if (tmpt->value == NULL) return NULL;
    /*if there is only one node in the queue and it is what we search*/
    else if ((tmpt->next == NULL)&(searchfn(tmpt->value, skeyp))) {
        returnValue = tmpt->value;
        tmpt->value = NULL;
        return returnValue;
    }
    /*more than one node in the queue*/
    else if (tmpt->next != NULL) {
        while (tmpt != NULL) {
            /*if found*/
            if (searchfn(tmpt->value, skeyp)) {
                /*if the found element is the head node*/
                if (tmpt == qp) {
                    qNode* secNode = (qNode*)tmpt->next;
                    returnValue = tmpt->value;
                    tmpt->next = secNode->next;
                    tmpt->value = secNode->value;
                    free(secNode);
                    return returnValue;
                }
                /*if the found element is not the head node*/
                else {
                    qNode* iter = qp;
                    while ((qNode*)iter->next != tmpt) iter = (qNode*)iter->next;
                    iter->next = tmpt->next;
                    return tmpt->value;
                }
            }
            tmpt = (qNode*)tmpt->next;
        }
    }
    return NULL;
}

/* concatenatenates q2 onto q1, q2 may not be subsequently used */
public void qconcat(void *q1p, void *q2p){
    qNode *q1pTmpt = (qNode*) q1p;
    qNode *q2pTmpt = (qNode*) q2p;
    qNode *newNode = (qNode*) malloc(sizeof(qNode));
    newNode->next = NULL;
    newNode->value = NULL;
    /*if two queues are both empty*/
    if ((q1pTmpt->value == NULL)&(q2pTmpt->value == NULL))
        return;
    /*if q1p is empty*/
    else if (q1pTmpt->value == NULL) {
        q1pTmpt->value = q2pTmpt->value;
        q1pTmpt->next = q2pTmpt->next;
        q2pTmpt->next = NULL;
	q2pTmpt->value = NULL;
    }
    /*if q1p is not empty, a new node needs to be created. And copy the value of q2p to the new node*/
    else if (q1pTmpt->value != NULL) {
        for (; q1pTmpt->next != NULL; q1pTmpt = (qNode*) q1pTmpt->next);
        newNode->next = q2pTmpt->next;
        newNode->value = q2pTmpt->value;
        q1pTmpt->next = (struct qNode*) newNode;
        q2pTmpt->next = NULL;
        q2pTmpt->value = NULL;
    }
}


