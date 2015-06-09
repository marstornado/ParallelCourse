//
//  main.c
//  lab3_udp
//
//  Created by mastornado on 10/12/14.
//  Copyright (c) 2014 mastornado. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "queue.h"
#include <pthread.h>
#include "./lockQueue.h"

#define BUF_SIZE 1024

typedef struct message
{
    char name[20];
    char mtext[BUF_SIZE];
}mes_t, *mes_p;


typedef struct clients {
    char name[30];
    struct sockaddr_in cli_ad;
    int id;
} clients_t, *clients_ptr;

typedef struct servMes {
    char name[30];
    char mtext[BUF_SIZE];
    struct sockaddr_in cli_ad;
    //int id;
}servM_t, *servM_p;

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
struct sockaddr_in serv_addr, cli_addr, cli_addr2;
socklen_t socSize = sizeof(struct sockaddr_in);
int s, port, revlen;
int idNum = 0;
void *qbegin, *qmessage;
mes_t *mes_buf;
clients_t *client1;
servM_t *servmes, *servmes2;

void itemPrint(void* elementp){
    clients_t *t = (clients_t*)elementp;
    if (t != NULL)
        printf("the queuu contains %s\t\t%d\n", t->name, t->id);
}

void whoPrint(void* elementp){
    clients_t *t = (clients_t*)elementp;
    char bufp[30];
    if (t != NULL)
        strcat(servmes2->mtext, t->name);
        strcat(servmes2->mtext, " ");
        snprintf(bufp, 30, "%d", t->id);
        strcat(servmes2->mtext, bufp);
        strcat(servmes2->mtext, " ;");
        printf("the queuu contains %s\t\t%d\n", t->name, t->id);
}

int searchfn(void* elementp, void* keyp){
    clients_t *itemElement;
    char name[30];
    
    itemElement = (clients_t *) elementp;
    strcpy( name, (char *)keyp);
    if ((elementp == NULL)|(keyp == NULL)) return 0;
    if (!strcmp(itemElement->name, name)) return 1;
    else return 0;
}

void broadCastMes(void* client) {
    struct sockaddr_in cli_mem_ad;
    mes_t mes_buf3;
    strcpy(mes_buf3.name, servmes2->name);
    strcpy(mes_buf3.mtext, servmes2->mtext);
    cli_mem_ad = ((clients_t *)client)->cli_ad;
    if(cli_mem_ad.sin_addr.s_addr != servmes2->cli_ad.sin_addr.s_addr) {
        sendto(s, &mes_buf3, sizeof(mes_buf3), 0, (struct sockaddr *) &cli_mem_ad, socSize);
    }
}

void * processMessage() {
    while (1) {
        servmes2 = LockQGet(qmessage);
        
        if ( servmes2 != NULL ) {
            mes_t mes_buf2;
            strcpy(mes_buf2.name, servmes2->name);
            strcpy(mes_buf2.mtext, servmes2->mtext);
            
            //PING
            if (!strcmp(mes_buf2.mtext, "\\ping")) {
                strcpy(mes_buf2.mtext, "the server is working");
                sendto(s, &mes_buf2, revlen, 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
            }
            
            //general
            if (LockQSearch(qbegin, searchfn, (void *)servmes2->name) != NULL) {
                
                if (!strcmp(mes_buf2.mtext, "\\leave")) {
                    strcpy(mes_buf2.mtext, "leave the chat room");
                    strcpy(servmes2->mtext, mes_buf2.name);
                    strcpy(servmes2->mtext, mes_buf2.mtext);
                    LockQApply(qbegin, broadCastMes);
                    //LockQApply(qbegin, itemPrint);
                    LockQRemove(qbegin, searchfn, (void *)mes_buf2.name);
                    LockQApply(qbegin, itemPrint);
                    printf("%s leave the room.\n", mes_buf2.name);
                    strcpy(mes_buf2.mtext, "the server: bye!");
                    sendto(s, &mes_buf2, sizeof(mes_buf2), 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
                }
                else if (!strcmp(mes_buf2.mtext, "\\who")) {
                    //strcpy(mes_buf2.mtext, mes_buf2.name);
                    //sendto(s, &mes_buf2, sizeof(mes_buf2), 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
                    strcpy(servmes2->mtext, "the member in the room: ");
                    LockQApply(qbegin, whoPrint);
                    strcpy(mes_buf2.mtext, servmes2->mtext);
                    strcpy(mes_buf2.name, "server ");
                    sendto(s, &mes_buf2, sizeof(mes_buf2), 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
                }
                else {
                    // echo back to clients
                    printf("broad cast the message. \n");
                    //LockQApply(qbegin, itemPrint);
                    LockQApply(qbegin, broadCastMes);
                    
                }
            }
            
            //Join
            if (!strcmp(mes_buf2.mtext, "\\join")) {
                client1 = (clients_t *)malloc(sizeof(clients_t));
                strcpy(client1->name, mes_buf2.name);
                client1->id = ++idNum;
                client1->cli_ad = servmes2->cli_ad;
                
                if (LockQSearch(qbegin, searchfn, (void *)mes_buf->name) == NULL) {
                    LockQPut(qbegin, client1);
                    strcpy(mes_buf2.mtext, "join_the_chat_room");
                    sendto(s, &mes_buf2, sizeof(mes_buf2), 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
                }
                else {
                    strcpy(mes_buf2.mtext, "server_has_the_same_name");
                    sendto(s, &mes_buf2, sizeof(mes_buf2), 0, (struct sockaddr *) &(servmes2->cli_ad), socSize);
                }
            }
        }
    }
    return NULL;
}

void * waitMessage() {
    printf("waiting on port %d\n", port);
    while (1) {
        revlen = (int)recvfrom(s, mes_buf, sizeof(*mes_buf), 0, (struct sockaddr *) &cli_addr, &socSize);
        printf("Server Received from %s:%d: \t",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
        printf("%s says: \"%s\"\n", mes_buf->name, mes_buf->mtext);
        servmes = (servM_t *)malloc(sizeof(servM_t));
        strcpy(servmes->name, mes_buf->name);
        strcpy(servmes->mtext, mes_buf->mtext);
        
        memcpy(&(servmes->cli_ad), &cli_addr, sizeof(struct sockaddr_in));
        LockQPut(qmessage, servmes);
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    
    if (argc < 2) {
        printf("please input the right parameter");
        return 1;
    }
    
    pthread_t threadS, threadP;
    mes_buf = (mes_p)malloc(sizeof(mes_t));
    qbegin = LockQOpen();
    qmessage = LockQOpen();
    
    s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        printf("fail to create a new socket");
        return 1;
    }
    
    //bind to server port */
    port = atoi(argv[1]);
    
    memset((char *) &serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    
    if (bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        printf("bind error");
        return 1;
    }
    
    pthread_create(&threadS, NULL, waitMessage, NULL);
    pthread_create(&threadP, NULL, processMessage, NULL);
    printf("wait for the end of the thread,\n");

    //close the server
    pthread_join(threadS, NULL);
    pthread_join(threadP, NULL);
    pthread_exit(NULL);
    qclose(qbegin);
    qclose(qmessage);
    close(s);
    return 0;
}
