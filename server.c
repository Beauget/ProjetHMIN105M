#include "fonctions.c"


union semun {
    struct semid_ds *buf;    /* Tampon pour IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Tableau pour GETALL, SETALL */
    struct seminfo  *__buf;  /* Tampon pour IPC_INFO */
};

 
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

    initTaille();
    key_t key = ftok("sharedServer.txt", 100);
     int shmid = shmget(key, sizeof(struct dataStruct) * taille, 0666 | IPC_CREAT);
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
    //printf("Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",dataInit[1].site,dataInit[1].go,dataInit[1].maxGo,dataInit[1].cpu,dataInit[1].maxCpu);

    //FIN MEMOIRE PARTAGER

    //SEMAPHORE (Actuellement 1 sémaphore par site Init à 1)

    key_t keySem = ftok("sharedSem.txt",10);
    if(keySem == -1) {
        printf("Erreur ftok sémaphore");
        exit(1);
    }

    int idSem = semget(keySem,taille,IPC_CREAT | 0666);
    union semun semCtrl;
    ushort tabSem[4]={1,0,0,0};

    semCtrl.array = tabSem;
    if(idSem == -1) {
        perror("Server : erreur création sémaphore");
        exit(1);
    }

    int initSem = semctl(idSem,0,SETALL,semCtrl);

    int parent = getpid(); //afin de savoir qui est le parent

  
    char Buffer[40];
    struct dataStruct etat;


    while (1)
    {   
        //affichageEtat(dataInit);
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
        {

            int pidChild = getpid();
            char m[60];

            //struct dataClient *infoClient = malloc(sizeof(struct clientStruct));
 
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

            printf("Server : Mise en place de la memoire partagé + du sémaphore dans le child\n");


            char *buffer = malloc(sizeof(char) * 21);
            if (recvAll(dsCv,buffer)<1)
            {
                printf("Client %i : erreur nom irrécuparable", ds);
                free(buffer);
                close(dsCv);
                close(ds); 
                exit(1);
            }
            printf(GRN"Bonjour %s ! \n", buffer);
            V(idSem,2,1);

            struct clientStruct client;
            initClient(&client,buffer,ds,dsCv,inet_ntoa(adCv.sin_addr), argv[1],dataInit);
            affichageClient(client);

             struct gestionSendUpdate serverUpdate;
            serverUpdate.etat= dataInit;
            serverUpdate.socket = dsCv;
            pthread_t updt; 



            if (pthread_create(&updt, NULL, UpdateServer, &serverUpdate) < 0) {
                printf("erreur pthread_create\n");
                close(serverUpdate.socket);
                exit(1);
            } 
            
            while (1) 
            {   char * msg = malloc (20 * sizeof (char));
                affichageEtat(dataInit);
                //strcpy(msg,"");

                if (recvWithSize2(client.socketServer,msg)<1)
                {
                    printf("Client %s : err nombre de requêtes", buffer);
                    free(buffer);
                    close(dsCv);
                    close(ds);
                    exit(1);
                }

                if ((strcmp(msg,"q")==0)||(strcmp(msg,"Q")==0))
                 {
                     printf("Client %s : veux se déconnecter\n", buffer);
                     close(dsCv);
                     P(idSem,2,1);
                     suppressionSharedClientAll(dataInit,buffer);
                     suppressionExcluClientAll(dataInit,&client);
                     printf("Client %s : c'est déconnecter\n", buffer);
                     free(buffer);
                     exit(0);
                 } 
 
                int nb = isInt(msg);
                printf("%s requête(s) arrivent !\n", msg);
                struct recvStruct * recvS =  malloc(sizeof(struct clientStruct)*nb);

                if (recvServer(client,recvS,isInt(msg))<0)
                {
                    printf("Client %s : err au recv\n", buffer );
                    free(recvS);
                }
                free(msg);

                int bool = -1;
                printf("tentative de %s\n", buffer);

                while(bool<0){
                    P(idSem,0,1);
                    //P(idSem,2,1);
                    bool = isPossible(dataInit,&client,recvS,nb);

                    if (bool>-1)
                    {   printf("possible %s\n", buffer);
                        actionAll(dataInit,&client,recvS,nb);
                        V(idSem,0,1);
                    }

                    else
                    {   
                        printf("Pas possible %s\n", buffer);
                        V(idSem,0,1);
                        //V(idSem,2,1);
                        Z(idSem,0);
                        
                    }                    

                }

 
                free(recvS);
                int nba = semctl(idSem, 2, GETVAL);
                printf("%i clients\n", nba);
                V(idSem,1, nba);

                printSharedData(dataInit);

              
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