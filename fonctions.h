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
    char type[20];
    char site[20];
    int value;
    
}recvStruct;