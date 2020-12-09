#include "fonctions.c" 

union semun {
    struct semid_ds *buf;    /* Tampon pour IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Tableau pour GETALL, SETALL */
    struct seminfo  *__buf;  /* Tampon pour IPC_INFO */
};

int main(int argc, char *argv[])
{


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

    struct sockaddr_in adrServ;
    adrServ.sin_addr.s_addr = inet_addr(argv[1]);
    adrServ.sin_family = AF_INET;
    adrServ.sin_port = htons((short)atoi(argv[2]));

    int lgAdr = sizeof(struct sockaddr_in);


    /* Connexion au serveur */
    int conn = connect(ds, (struct sockaddr *)&adrServ, lgAdr);
    if (conn < 0)
    {
        perror("Client: Erreur lors de l'appel à connect() :");
        close(ds);
        exit(1);
    }
    printf(GRN"Client : demande de connexion reussie \n"RESET);

    //INITIALISATION DU CLIENT
    char name[20];
    printf("Veuillez indiquer votre nom. (Aucun client ne doit avoir le même nom)\n");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    int taille = strlen(name);

    if(taille==0){
        printf("Nom invalide\n");
        close(ds); 
        exit(1);

    }

    if ( sendAll(ds, name) < 1)
    {
        close(ds); 
        exit(1);
    }

    int socket = ds;
    char *ip = argv[1];
    char *port = argv[2];

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
 
    struct gestionSendUpdate clientUpdate;
    clientUpdate.socket = ds;
    clientUpdate.verrou=&lock;
    clientUpdate.cond=&cond;

    pthread_t updt;

    initTaille();
     int size=0;

    //affichageEtat(ptrdata);
    if (pthread_create(&updt, NULL, UpdateClient, &clientUpdate) < 0) {
        printf("erreur pthread_create\n");
        exit(1);
    } 
    while (1)   
    {  
 
        printf(BLU " ###### Bienvenue dans notre système de réservation en ligne ###### \n" RESET);
        printf(BLU "Vous allez pouvoir saisir un message pour nous indiquer quel ressources vous voulez acquérir ! \n" RESET);

        printf("Combien de requêtes?/ Quitter?(q/Q)\n");
        printf(RED"Ne pas mettre plusieurs fois la même combinaison de site et de type(cpu/go) dans le lot de requêtes.\n"RESET);

        char * msg = malloc (20 * sizeof (char));
        fgets(msg, sizeof(msg), stdin);


        msg[strlen(msg) - 1] = '\0'; //retirer le saut de ligne \n parce que j'appuie sur entrer

        if ((strcmp(msg,"q")==0)||(strcmp(msg,"Q")==0))
        {
            printf("En cour de déconnexion\n");
            send2(clientUpdate.socket,msg, sizeof(msg));
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
        {   pthread_mutex_lock(clientUpdate.verrou);

            if ( send2(clientUpdate.socket,msg, sizeof(msg)) < 1){
                printf(RED"Erreur à l'envoie du nombre de requêtes.On vous déconnecte\n"RESET);
                send2(clientUpdate.socket,"q", (sizeof(char)));
                close(ds);
                free(msg);
                exit(1);

            }
            
            if(SendClient(/*ptrdata,*/&clientUpdate,msg)<1){
                printf(RED"Erreur pendant le send.On vous déconnecte\n"RESET);
                send2(clientUpdate.socket,"q", sizeof(char));
                close(ds);
                free(msg);
                exit(1);

            }
            else
                printf(GRN"Requêtes Envoyé ! Veuillez patientez...\n"RESET);
            free(msg);
            pthread_mutex_unlock(clientUpdate.verrou);
        }
    }

    close(ds);


    return 0;
}