#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <pthread.h>

//Quelques couleur pour la visibilité
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"


union semun {
    struct semid_ds *buf;    /* Tampon pour IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Tableau pour GETALL, SETALL */
    struct seminfo  *__buf;  /* Tampon pour IPC_INFO */
};


int taille = 5; //Pour plus de modularité plus tard

struct dataClient {
    char nomClient[40];
    int nbCpu;
    int stockage;
}dataClient;


 struct dataStruct {
    char ville[20];
    int go;
    int cpu;
    struct dataClient listeRessExclusive[100];
    struct dataClient listeRessPartager[100];
}dataStruct;


int nbLigne(FILE *file) {
    int nbLignes = 0;
    int cpt;
    while((cpt = fgetc(file)) != EOF) if(cpt == '\n') nbLignes++;
        rewind(file);
        return nbLignes;
    }

void InitDataFromFile(struct dataStruct *data) {
    FILE* file = NULL;
    file = fopen("data.txt","r");
    taille = nbLigne(file);

    if(file != NULL) {
        for(int i = 0; i < taille; i++) {
           if(fscanf(file,"%s %i %i", data[i].ville,&data[i].go,&data[i].cpu) == 0) {
               printf("Erreur fscanf !");
               exit(0);
           }
        }
    }
    else {
        printf("erreur file");
    }
}

void affichageEtat(struct dataStruct* data) {
    printf(YEL"                 --- AFFICHAGE ETAT SYSTEME --- \n \n");
for(int i = 0; i < taille;i++) {
    printf("VILLE : %s, GO DISPONIBLE : %i, CPU DISPONIBLE : %i \n",data[i].ville,data[i].go,data[i].cpu);
}
printf(RED"|||| POUR TOUTE DEMANDE VEUILLEZ LIRE NOTRE DOCUMENTATION |||| \n" RESET);

}



 
//un recv classique , les lock/cond son à ajouter je pense
int recvType(int addr,char* s,int taille){
    int nbOctetRecu = 0; // Nombre d'octets reçus au total
    int nbrecv = 0; // Nombre d'octets reçus à chaque tour de boucle
    while(nbOctetRecu < taille){
        nbrecv = recv(addr,s+nbOctetRecu,taille-nbOctetRecu,0);
        switch(nbrecv){
            case -1 : 
                return -1;
            
            case 0 :  
                return 0;
            
            default :
                nbOctetRecu = nbOctetRecu + nbrecv;
                break;
        }
    }
    return 1;
}



//Un send classique qui indiquera si le changement se fais sur le cpu ou les go (type)
int sendType(int addr, void* msg,int size,char type[10]){
    int nbSend = 0;
    int returnSend = 0;
    while (nbSend < size){
            returnSend = send(addr,msg+nbSend,size-nbSend,0);
            switch(returnSend){
        
             case -1 : 
                 perror("Client: erreur lors de l'envoi:"); 
                 return -1;
            
             case 0 :  
                 printf("Client: serveur déconnecté \n"); 
                 return 0;
                 default :
                    nbSend = nbSend + returnSend;
                    break;
        }
    }
    return 1;
}

int sendFunction(int dsClient, char *message, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = send(dsClient,message + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur sendFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur sendFunction()");
            return 0;
        }
        nbOctet += value;
    }
    return nbOctet;
}

int RecvFunction(int dsClient, char *reponse, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = recv(dsClient,reponse + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur RecvFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur RecvFunction()");
            return 0;
        }
        nbOctet += value;
    }
    return 1;
}




/*int sendStruct(int addr, struct serverState * sState, int position,int type){
    int size;
    int returnInfo;

    switch(type){
        case 1 : if(returnInfo = sendType(addr,&sState->city[position],sizeof(int),"ville")<1){
            return -1;
        }
        case 2 : if(returnInfo = sendType(addr,&sState->cpu[position],sizeof(int),"cpu")<1){
            return -1;
        }
    }
    printf("A envoyer un message de type %i \n",type );
    return type;
}*/

