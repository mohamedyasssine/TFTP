#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "serveur.h" // Ajout de l'en-tête serveur.h
#define BUFSIZE 1024

void erreur_verification(int r, char *message) {
    if (r < 0) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}
int creer_socket_UDP(int port)
{
    //socket de serveur (le descripteur)
   	int sock_serveur;
    //structure de l'addresse de serveur 
    struct sockaddr_in serv_addr;
    /*création de la socket udp ET ipv4*/
    sock_serveur = socket(AF_INET, SOCK_DGRAM, 0);
    erreur_verification(sock_serveur,"ERREUR LORS DE LA CRÉATION DE SOCKET SERVEUR");
    //AF_INET est le type de serveur utilisé qu'est ipv4
    serv_addr.sin_family = AF_INET;
    /*INADDR_ANY est une constante qui représente une adresse IP spéciale 
    utilisée pour écouter sur toutes les interfaces 
    réseau disponibles sur la machine. 
    donc le le serveur accepte les connexions entrantes sur toutes les interfaces réseau de
     la machine, y compris
     l'interface loopback (127.0.0.1) et toutes les interfaces réseau physiques.
    */
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /*convertit le numéro de port passé en paramètre 
    depuis l'ordre d'octets de l'hôte vers l'ordre d'octets du réseau.*/
	serv_addr.sin_port = htons(port);
    /*Après la création du socket, la fonction bind lie le socket à l'adresse et 
    au numéro de port spécifiés dans addr (structure de données personnalisée).*/
    int e = bind(sock_serveur, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    erreur_verification(e,"erreur lors liaisocket à l'addresse de serveur");
    return sock_serveur ;
}
void ecouter_packet()
{
    //la socket client 
    struct sockaddr client_addr;
    size_t client_size = sizeof(client_addr);
    //longueur de packet recu depuis le client 
    int recv_len;

    char buffer[BUFSIZE];
    char nom_fichier[512];//nom fichier recu dans la packet
    //mode de transfert de fichier (octer/netascci...) de packet recu
    char mode[10];
    //l'ocode de packet recu
    uint16_t opcode;
    while (1)
    {
        /*STRUCTURE DE RRQ ET WRQ PACKET [OPCODE(2bit)|nomfichier|0(fin de string|mode|0)]*/
        //recevoir le data (la packet)
        recv_len =recvfrom(listener, (char *)buffer, BUFSIZE, MSG_WAITALL,&client_addr, (socklen_t *) &client_size);
        memcpy(&opcode, (uint16_t *) & buffer, 2); //on copie l'opcode de packet qui contient 2 bit dans le tompon
        opcode = ntohs(opcode);   
        strcpy(nom_fichier, buffer + 2);//on copie le nom de fichier depuis le tompon 
        strcpy(mode, buffer + 3 + strlen(nom_fichier));//ici on copie le nom fichier 
        if(opcode!=1&&opcode!=2)//si le packet recu n'est pas RRQ ni WRQ alors c'est un erreur 
        {
            printf("invalide opcode (numero) de packet ");
            //envoyer_packet d'erreur a la client sent_error(); 
            /*mais ca dépend de message à envoyer donc on doit passer le message par paramétre
            et ca dépend de la code erreor car chaque erreur à 
            une code spécifique à mettre dans la packet donc on doit passer le code aussi en paramétre 

            */
        }
        // la ligne suivant permet d'allouer un espace memoire pou le chemin complet de fichier demandé par le client 

        //construction de chemin fe fichier
        /*on va supposer que le fichier demander par le client est dans le meme dossier que le serveur.c*/ 
        char *chemin = strdup(nom_fichier);
        if (access(chemin, F_OK) != -1)
		{
			// rien à faire
		}
        else
        {
            //on va envoyer un packet d'erreur avec code d'erreur 1 (fichier demandé n'existe pas)
            continue;
        }
    }
}
int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("mnombre de paramétre invalide");
        return -1;
    }
    int port=atoi(argv[1]);;
    listener=creer_socket_UDP(port);//créer la socket serveur et lieer à la socket serveur 


}