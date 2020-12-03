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

    //SEMAPHORE (A VERIFIER)

    key_t keySem = ftok("sharedSem.txt",10);
    if(keySem == -1) {
        printf("Erreur ftok sémaphore");
        exit(1);
    }

    int idSem = semget(keySem,1,IPC_CREAT | 0666);
    union semun egCtrl;
    ushort Sem[1];

    for(int i = 0; i < 1; i++) {
        Sem[i] = 1;
    }

    egCtrl.array = Sem;

    int initSem = semctl(idSem,0,SETALL,egCtrl);

    struct sembuf op[] = {
        { 0, -1 , SEM_UNDO}, //P
        { 0, 1 , SEM_UNDO}, //V
        { 0, 0 , SEM_UNDO}, //Z
    };

    //FIN SEMAPHORE

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
            char m[600];

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

            dataInit = shmat(shmid, NULL, 0);

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

            struct clientStruct client;
            initClient(&client,buffer,ds,dsCv,inet_ntoa(adCv.sin_addr), argv[1],dataInit);
            affichageClient(client);

            while (1) 
            {  
                affichageEtat(dataInit);

                if (recvAll(dsCv,m)<1)
                {
                    printf("Client %i : irrécuparable", ds);
                    free(buffer);
                    close(dsCv);
                    close(ds);
                    exit(1);
                    } 

                int nb = isInt(m);
                printf("%i requête(s) arrivent !\n", nb );

                char recv[3][20];
                struct recvStruct * recvS =  malloc(sizeof(struct clientStruct)*nb);

                for (int i = 0; i < nb; ++i)
                {
                for (int j = 0; j < 3; ++j)
                {
                    if (recvAll(dsCv,m)<1)
                    {
                        printf("Client %i : irrécuparable", ds);
                        free(buffer);
                        free(recvS);
                        close(dsCv);
                        close(ds);
                        exit(1);
                    }
                    printf("Serveur : a envoyé : <%s> \n",m);
                    strcpy(recv[j],m);
                    
                }  
                //printf("data ville %s : %i\n",dataInit[0].site,positionSite(dataInit,dataInit[1].site));

                //int size = lSharedSize(dataInit[0].LSGo);
                //printf("%i\n",size );

                strcpy(recvS[i].name,buffer);
                strcpy(recvS[i].site,recv[0]);
                strcpy(recvS[i].type,recv[1]);
                recvS[i].value= isInt(recv[2]);

                printf("%s\n",recvS[i].name);
                printf("%s\n",recvS[i].site);
                printf("%s\n",recvS[i].type);
                printf("%i\n",recvS[i].value);
                }
            
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