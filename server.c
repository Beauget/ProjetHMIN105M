#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "fonctions.c"




int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Serveur : utilisation: %s numero_port\n", argv[0]);
        exit(1);
    }

    //SERVEUR TCP
    int ds = socket(PF_INET, SOCK_STREAM, 0);
    if (ds == -1)
    {
        perror("Serveur : probleme creation socket");
        exit(1);
    }
    printf("Serveur: creation de la socket : ok\n");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(ds, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Serveur : Erreur lors de l'appel à bind() \n");
        close(ds);
        exit(1);
    }

   

    printf("Serveur: bind : ok\n");


    struct sockaddr_in adCv; // pour obtenir l'adresse du client accepté.
    socklen_t lgCv = sizeof(struct sockaddr_in);
    int dsCv;

    //FIN SERVEUR TCP


    //MEMOIRE PARTAGER
    key_t key = ftok("sharedServer", 100);
     int shmid = shmget(key, sizeof(struct dataStruct)* taille, 0666 | IPC_CREAT);
    if (shmid < 0)
    {
        printf("Serveur : erreur création de mémoire partagé || %s", strerror(errno));
        exit(1);
    }
    struct dataStruct *dataInit;

    dataInit = (struct dataStruct*)shmat(shmid,NULL,0);
    if((void *)dataInit == (void *)-1) {
        printf("Erreur shmat");
    }
    InitDataFromFile(dataInit);

    //FIN MEMOIRE PARTAGER

    int parent = getpid(); //afin de savoir qui est le parent

    while (1)
    {

        int ecoute = listen(ds, 5);
        if (ecoute < 0)
        {
            printf("Serveur : Erreur lors de l'appel à listen() \n");
            close(ds);
            exit(1);
        }

        if ((dsCv = accept(ds, (struct sockaddr *)&adCv, &lgCv)) < 0)
        {
            perror("    // Si le serveur n'a pas reçu le nombre d'octets attendu, il boucleServeur, probleme accept :");
            close(ds);
            exit(1);
        }

        printf("Serveur: le client %s:%d est connecté  \n", inet_ntoa(adCv.sin_addr), ntohs(adCv.sin_port));
        printf("En attente de recevoir le nom du client %i \n ", dsCv);

        int child; //je crée un enfant du processus car un client c'est connecté !

        if (child = fork())
        { //je fork

            int pidChild = getpid();
            char m[1024];
            struct dataClient *infoClient = malloc(sizeof(struct dataClient));

            /*recupère la mémoire partagé */
            key_t key = ftok("sharedServer", 100);

            if ((shmid = shmget(key, sizeof(struct dataStruct) * taille, 0666 | IPC_CREAT)) < 0)
                perror("Serveur : cannot shmget");


            if (shmid < 0)
            {
                perror("Serveur : shmid <0");
                close(ds);
                exit(1);
            }

            dataInit = shmat(shmid, NULL, 0);

            printf("Server : Mise en place de la memoire partagé + du sémaphore dans le child\n");

            if (RecvFunction(dsCv, m, sizeof(m)) < 1)
            {
                close(dsCv);
                close(ds);
                printf("Client %i : erreur au calcul de la taille du message \n", ds);
                exit(1);
            }
            char *buffer = malloc(sizeof(char) * (taille + 1));

            int err = recvType(dsCv, buffer, taille);

            if (err < 1)
            {
                free(buffer);
                close(dsCv);
                close(ds);
                printf("Client %i : erreur nom irrécuparable", ds);
                exit(1);
            }

            printf("Bonjour %s ! \n", buffer);

            while (1)
            {
                char m[1024];

                //la taille change de valeur après le recvType
                int taille = 0;
                if (recvType(dsCv, m, sizeof(m)) < 1)

                { 
                    close(dsCv);
                    close(ds);
                    exit(1);
                }

                buffer = malloc(sizeof(char) * (taille + 1));

                //Le buffer récupère le message après le recvType, elle utilisa la taille
                //au dessus pour etre sure de prendre exactement ce qu'il faut
                int err = recvType(dsCv, buffer, taille);

                if (err < 1)
                { //semctl(idSemaphore,5,SETVAL,semctl(idSemaphore,5,GETVAL) -1);
                    free(buffer);
                    close(dsCv);
                    close(ds);
                    exit(1);
                }

                //printf("Serveur : j'ai reçu %d octet(s) \n", err * taille);
                printf("Serveur : a envoyé : <%s> \n",buffer);
                printf("data ville %s",dataInit[0].ville);
            }

              
        }
        if (child == parent)
        {
            wait(&child);
            printf("Listen failed...\n");
            close(dsCv);
            close(ds);
            shmctl(shmid,IPC_RMID,NULL);
            exit(1);
        }
    }
    close(dsCv);
    close(ds);
    printf("Serveur : je termine\n");
    exit(0);
}
