#include "fonctions.c" 

union semun {
    struct semid_ds *buf;    /* Tampon pour IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Tableau pour GETALL, SETALL */
    struct seminfo  *__buf;  /* Tampon pour IPC_INFO */
};

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

    //INITIALISATION DU CLIENT
    char name[20];
    printf("Votre nom\n");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    int taille = strlen(name);

    if ( sendAll(ds, name) < 1)
    {
        close(ds); 
        exit(1);
    }

    int socket = ds;
    char *ip = argv[1];
    char *port = argv[2];
    //MEMOIRE PARTAGER

    key_t key = ftok("sharedServer.txt", 100);
    int shmid = shmget(key, sizeof(struct dataStruct) * taille, 0666 | IPC_CREAT);
    struct dataStruct *ptrdata;
    ptrdata = (struct dataStruct*)shmat(shmid,NULL,0);
 
    //FIN MEMOIRE PARTAGER

    struct clientStruct client;   
    initClient(&client,name,ds,-1 ,argv[1],argv[2],ptrdata); 
    affichageClient(client);

/*    key_t keySem = ftok("sharedSem.txt",10);
    if(keySem == -1) {
        printf("Erreur ftok sémaphore");
        exit(1);
    }

   int idSem = semget(keySem,taille,IPC_CREAT | 0666);
    union semun semCtrl;
    ushort tabSem[1];
    for(int i = 0; i < 1; i++) {
        tabSem[i] = 1;
    }
    semCtrl.array = tabSem;
    if(idSem == -1) {
        perror("Server : erreur création sémaphore");
        exit(1);
    }

    int initSem = semctl(idSem,0,SETALL,semCtrl);


    struct gestionSys * paramGestionSys = malloc(sizeof(gestionSys));
    paramGestionSys->idSem = initSem ; 
    pthread_mutex_init(&paramGestionSys->verrou,NULL);

    pthread_t * affiche;
    affiche = (pthread_t *) malloc (sizeof(pthread_t));*/
     

    printf("Client : avant boucle \n"); 

    //int x = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
 
    char Buffer[40];
    struct dataStruct etat;

    struct gestionSendUpdate clientUpdate;
    clientUpdate.etat= &etat;
    clientUpdate.socket = ds;
    clientUpdate.verrou=&lock;
    clientUpdate.cond=&cond;

    pthread_t updt;

    initTaille();
     int size=0;

    affichageEtat(ptrdata);
    if (pthread_create(&updt, NULL, UpdateClient, &clientUpdate) < 0) {
        printf("erreur pthread_create\n");
        exit(1);
    } 
    while (1)   
    {  
 
        if (size<0)
        {
            printf("Vous avez atteins le nombre maximum de réservations\n");
        }
 
        printf(BLU " ###### Bienvenue dans notre système de réservation en ligne ###### \n" RESET);
        //ptrdata = shmat(shmid, NULL, 0);
        //affichageEtat(ptrdata);

        printf(BLU "Vous allez pouvoir saisir un message pour nous indiquer quel ressources vous voulez acquérir ! \n" RESET);

        printf("Combien de requêtes?/ Quitter?(q/Q)\n");

        char * msg = malloc (20 * sizeof (char));
        fgets(msg, sizeof(msg), stdin);


        msg[strlen(msg) - 1] = '\0'; //retirer le saut de ligne \n parce que j'appuie sur entrer

        if ((strcmp(msg,"q")==0)||(strcmp(msg,"Q")==0))
        {
            printf("En cour de déconnexion\n");
             sendWithSize2(client.socket,msg, sizeof(msg));
            free(msg);
            close(ds);
            exit(0);
        }

        if (isInt(msg)<=0)
        {
            printf(YEL"Mettre un chiffre supérieur à 0\n"RESET);
            free(msg);
        }

        else
        {

            if ( sendWithSize2(client.socket,msg, sizeof(msg)) < 1){
                printf(RED"Erreur à l'envoie du nombre de requêtes\n"RESET);
            }
            
            if(SendClient(ptrdata,&client,msg)<1)
                printf(RED"Erreur pendant le send\n"RESET);
            else
                printf(GRN"Requêtes Envoyé ! Veuillez patientez...\n"RESET);
            free(msg);
        }
    }

    close(ds);


    return 0;
}