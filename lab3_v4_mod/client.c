#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_SIZE 1024 

typedef struct message
{
    char name[20];
    char mtext[BUF_SIZE];
}mes_t, *mes_p;

int flag, s;
struct sockaddr_in server;
socklen_t socSize = sizeof(struct sockaddr_in);
char buf[BUF_SIZE], buf2[BUF_SIZE];
mes_t *mes_buf, *mes_buf2;

void * sendM() {
    while(flag) {
        
        scanf("%s", buf2);
        strcpy(mes_buf2->mtext, buf2);
        
        /* send message */
        if (sendto(s, mes_buf2, sizeof(*mes_buf2), 0, (struct sockaddr *) &server, socSize) == -1) {
            printf("sendto() error");
            break;
        }
        
        if (!strcmp(buf2,"\\leave")) {
            printf("leave the chat room. \n");
            flag = 0;
            break;
        }
    }
    flag = 0;
    return NULL;
}

int main(int argc, char *argv[]) {
    
    int n, port;
    pthread_t thread1;
    
    if (argc < 4) {
        printf("Usage: %s name <host> <port>\n", argv[0]);
        return 1;
    }
    
    mes_buf = (mes_p)malloc(sizeof(mes_t));
    mes_buf2 = (mes_p)malloc(sizeof(mes_t));
    flag=1;
    strcpy(mes_buf->name, argv[1]);
    strcpy(mes_buf2->name, argv[1]);
    
    port = atoi(argv[3]);
    
    /* initialize socket */
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1;
    }
    
    /* initialize server addr */
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(argv[2], &server.sin_addr);
    
    printf("let us input the message:\n");
    /* start a thread to process the input message */
    pthread_create(&thread1, NULL, sendM, NULL);
    
    /* receive echo message*/
    while (flag) {
        //printf("start warting flag ");
        n = (int)recvfrom(s, mes_buf, sizeof(*mes_buf), 0, (struct sockaddr *) &server, &socSize);
        if (n > 0) {
            printf("%s says: \"%s\"\n", mes_buf->name,mes_buf->mtext);
        }
    }
    
    printf("the person quit the program. \n");
    pthread_join(thread1, NULL);
    pthread_exit(NULL);
    close(s);
    return 0;
}
