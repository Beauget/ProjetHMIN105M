#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

//Quelques couleur pour la visibilité
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

struct LExclu
{   char type[20]; //Pas besoins de séparer les type dans le exclusif, pas de calcul du max dessus!
	char site[20];
	int quantity;	
}LExclu;



struct LShared
{
	char name[40];
	int quantity;	
}LShared;

struct clientStruct {
    char name[40];
    int socket;
    int socketServer;
    char ip[20];
    char port[20];
    struct LExclu exclu[101];
    struct dataStruct * data;
}dataClient;


 struct dataStruct {
    char site[20];
    int maxGo;
    int go;
    struct LShared LSGo[101];

    int maxCpu;
    int cpu;
    struct LShared LSCpu[101];
}dataStruct;

struct recvStruct
{
    char name[40];
    int isExclu; //1 ou 0
    char type[20];
    char site[20];
    int value;
    
}recvStruct;


struct gestionSendUpdate{
    int socket;
    //char * name;
    char msg[200];
    pthread_mutex_t *verrou;
    pthread_cond_t *cond;
}gestionSendUpdate;

struct SendUpdate{
    struct dataStruct *etat;
    int size;
    int socket;
    char * name;
    char msg[200];
}SendUpdate;

/*Permet de savoir le nombre de lignes dans le fichier */
int nbLigne(FILE *file);

/*Permet de conserver le nombre de sites*/
int initTaille();

/*Récupère les informations et les met dans la structure */
void InitDataFromFile(struct dataStruct *data); 

/*Initialise la structure clientStruct*/
void initClient(struct clientStruct * client,char * name,int socket, int socketServer, char * ip ,char * port,struct dataStruct * data);

/*Affiche les informations relatives au client*/
void affichageClient(struct clientStruct client);

/*Affiche la stucture dataStruct , utilisé pour pour la mémoire partagé*/
void affichageEtat(struct dataStruct* data);

/*Si le char * est un nombre positif,le renvoie. Sinon, renvoie -1*/
int isInt(char * msg);

/*convertit un nombre en char **/
void intToChar(int value, char * msg);

/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lSharedSize(struct LShared * l);
/*RENVOIE LA QUANTITY MAX DE LA LISTE PARTAGER*/
int maxLShared(struct LShared * l);
int maxLSharedType(struct dataStruct * data,int position,char * type);
/*PREMIERE APPARITION DU CLIENT DANS LA LISTE*/                                            
int lSharedPosition(struct LShared * l,char * name,int size);
/*SUPPRESSION SUIVANT UNE POSITION */
void suppressionShared(struct LShared * l, int position, int size);
/*SUPPRIME UNE VALEUR SELON SA POSITION*/                                                                              
void suppressionSharedType(struct dataStruct* data,char * site,char * type,int pos,int size);
/*SUPPRESSION DE TOUTE LES APPARITIONS DU CLIENT */
void suppressionSharedClient(struct LShared * l, char* name, int size);
/*ADD/MISE A JOUR DANS LA LISTE DES RESERVATION PARTAGER DANS LA STRUCTURE PARTAGER*/
void actionShared(struct dataStruct* data,int position,int pos, char * name, char * type, int value); 

/*affiche une structure LShared en particulier*/
void printShared(struct LShared * l, int size);

/*renvoie la position du client dans la liste des données partagé , -1 sinon*/
int isInSharedType(struct dataStruct * data,int position, struct clientStruct * client ,char * type);
/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lExcluSize(struct LExclu * l);
/*RENVOIE LA POSITION DU SITE SI IL EST DANS LA LISTE EXCLUSIF -1 SINON */    
int lExcluPosition(struct LExclu * l,char * type, char * site);
/*SUPRESSION D'UN  SITE EN PARTICULIER */
void suppressionExcluClient(struct clientStruct * client,int position, int size);
/*MET A JOUR LA STRUCTURE CLIENT OU BIEN AJOUTE À LA FIN DE LA LISTE*/
void actionExcluClient(struct clientStruct * client,int size, char * type, char * site, int value);
/*renvoie la valeur maximum allouable par un site*/
int returnMaxSite(struct dataStruct * data,int position,char * type);

/*L'envoie de la structure partagé en message tcp*/
int sendStruct(struct dataStruct * data,int size,int socket);

/*Fonctions permettant de pouvoir correctements envoyer et reçevoir des messages entre clients et serveurs*/
int sendFunction(int dsClient, char *message, int taille);
int RecvFunction(int dsClient, char *reponse, int taille);
int sendAll(int socket,char * m);
int recvAll(int socket,char * msg);
int recvAll2(int socket, char * buf, int len) ;
int recv2(int sock, char * data);
int sendall2(int sock, const char* data, int data_length);
int send2(int sock, const char* data, int data_length);

/*Renvoie la position du site dans la structure partagé, s;i elle n'y est pas on renvoie -1*/
int positionSite(struct dataStruct * data, char * site);

/*MET A JOUR LA STRUCTURE EXLUSIF EN DIMINUANT LE NOMBRE DE DE GO/CPU */
void actionExclu(struct dataStruct* data, char * site, char * type, int value);


/*MET A JOUR LA STRUCTURE EXCLUSIF EN AJOUTANT LE NOMBRE DE DE GO/CPU */
/*Met a jour la structure partagé en cas de déconnexion d'un client*/
void suppressionExclu(struct dataStruct* data, int position, char * type, int value);
void suppressionSharedClientAll(struct dataStruct* data,char * name);

/*fonctions permettant d'envoyer les requêtes en tcp, renvoie -1 en cas de soucis*/
int isExcluSend(char * msg);
int siteSend(char * msg);
int typeSend(char * msg);
int valueSend(char * msg);

/*fonction utilisant les 4 au dessus pour faire les envoie , renvoie -1 en cas de soucis*/
int SendClient(struct gestionSendUpdate * client,char * size);

/*fonctions permettant de recevoir les requêtes en tcp, renvoie -1 en cas de soucis*/
int recvServer(struct clientStruct  client,struct recvStruct * recvS ,int size);

/*permet de savoir ce qu'il restera en cas de diminution du cpu/go d'un site*/
int restantType(struct dataStruct * data,int position,char * type, int value);

/*Modifie la structure partage en appliquant une requête exclusive*/
void actionExcluAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS);

/*Récupère la quantité de cpu/go d'un site, renvoie -1 en cas d'erreur*/
int getValue(struct dataStruct *data, int position,char * type, int posList);

/*Modifie la structure partage en appliquant une requête partagé*/
void actionSharedAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS);

/*Modifie la structure partage en appliquant les requêtes partagées et exclusives*/
void actionAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct * recvS,int size);

/*initialise le thread POSIX mutex*/
void initMutex(pthread_mutex_t * verrou);

/*Décrémente un sémaphore avec la valeur n*/
int P(int semid, int semnum, int n);
/*incrémente un sémaphore avec la valeur n*/
int V(int semid, int semnum, int n);
/*Mise en attente jusqu'à que le sémaphore atteins 0*/
int Z(int semid, int semnum);

/*Recois en permanence les messages envoyé en tcp */
void * UpdateClient(void *param);
/*Le serveur envoie un message à son client quand il y a un mise à jour dans la structure*/
void * UpdateServer(void *param);
/*Permet de savoir l'état des données partagé de la mémoire partagé*/
void printSharedData(struct dataStruct * data );

/*Vérifie si le client a déja réservé le site en exclusif, renvoie sa position ou bien un endroit ou la stocker*/
int inExclu(struct LExclu * l, char * site, int size);

/*vérfifie si la requête exclusif est possible.Renvoie -1 si impossible*/
int isPExclu(struct dataStruct * data,int position, struct LExclu * l,char * type, char * site,int value, char * name);

/*vérifie si la requête partagé est possible selon le type et le site.Renvoie -1 si impossible, 0 si probable , 1 si possible*/
int isPSharedType(struct LShared * l,char * name);

/*vérifie si la requête partagé est possible et le site.Renvoie -1 si impossible, 0 si probable , 1 si possible*/
int isPShared(struct dataStruct * data,int position,char * type ,char * name,int value);

/*vérifie si la requête partagé est possible et le site.Renvoie -1 si impossible, 0 si probable , 1 si possible*/
int isPossible(struct dataStruct * data, struct clientStruct * client, struct recvStruct * recvS, int size);



