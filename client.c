#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "fonctions.c"

int main(int argc, char *argv[])
{

    /* On récupère l'ip, le port et un nombre (pour l'instant) */

    if (argc != 3)
    {
        printf("utilisation : %s ip_serveur port_serveur \n", argv[0]);
        exit(0);
    }

    int ds = socket(PF_INET, SOCK_STREAM, 0);

    if (ds == -1)
    {
        printf("Client : Problème avec la creation de la socket\n");
        exit(1);
    }

    printf("Client: creation de la socket : ok\n");

    struct sockaddr_in adrServ;
    adrServ.sin_addr.s_addr = inet_addr(argv[1]);
    adrServ.sin_family = AF_INET;
    adrServ.sin_port = htons((short)atoi(argv[2]));

    int lgAdr = sizeof(struct sockaddr_in);

    printf("client :avant connect \n");

    /* Connexion au serveur */
    int conn = connect(ds, (struct sockaddr *)&adrServ, lgAdr);
    if (conn < 0)
    {
        perror("Client: Erreur lors de l'appel à connect() :");
        close(ds);
        exit(1);
    }
    printf("Client : demande de connexion reussie \n");
    char m[1501];

    //INITIALISATION DU CLIENT
    char name[20];
    printf("Votre nom\n");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    int taille = strlen(name);

    if (sendFunction(ds, name, sizeof(name)) < 1)
    {
        close(ds);
        exit(1);
    }

    char *ip = argv[1];
    char *port = argv[2];
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);

    //MEMOIRE PARTAGER

    key_t key = ftok("shmfile.txt", 100);
    int shmid = shmget(key, sizeof(struct dataStruct) * 5, 0666 | IPC_CREAT);
    struct dataStruct *ptrdata;

    //FIN MEMOIRE PARTAGER
    

    printf("Client : avant boucle \n"); 

    while (1)
    {

        printf(BLU " ###### Bienvenue dans notre système de réservation en ligne ###### \n" RESET);
        ptrdata = shmat(shmid, NULL, 0);
        affichageEtat(ptrdata);

        printf(BLU "Vous allez pouvoir saisir un message pour nous indiquer quel ressources vous voulez acquérir ! \n" RESET);
        fgets(m, sizeof(m), stdin);
        m[strlen(m) - 1] = '\0'; //retirer le saut de ligne \n parce que j'appuie sur entrer

        int taille = strlen(m);

        if (sendFunction(ds, m, sizeof(m)) < 1)
        {
            close(ds);
            exit(1);
        }

        if (sendFunction(ds, m, sizeof(m)) < 1)
        {
            close(ds);
            exit(1);
        }

        printf("Client : j'ai déposé un message de %lu octets\n", strlen(m));
    }

    close(ds);
    printf(" %li Client : je termine\n", strlen(m));
    //informer au serveur qu'un client est déconnecté

    return 0;
}
