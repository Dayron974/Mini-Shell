#ifndef H_HEAD202302081120DDI
#define H_HEAD202302081120DDI

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

enum {
MaxLigne = 1024, // longueur max d'une ligne de commandes
MaxMot = MaxLigne / 2, // nbre max de mot dans la ligne
MaxDirs = 100, // nbre max de repertoire dans PATH
MaxPathLength = 512, // longueur max d'un nom de fichier
};

//structure contenant les infos des processus en arriere-plan
typedef struct bgprocess {
	pid_t pid;
	char *cmd;
	char *terminated_cmd;
	int pipe_read; // pour stocker le côté lecture du pipe
	char * output;
	struct bgprocess *next;
} bgprocess;


int decouper(char *, char *, char **, int ); // decoupe une chaine à partir d'un caractere de separation

void executer_enfant(char *ligne, char *mot[MaxMot], char *dirs[MaxDirs], int nbmots, int bg); // execute une commande
void mcd(int nb,char * mot[MaxMot],char prompt[MaxPathLength]); // commande chdir
void monMan(int nb, char *mot[MaxMot]); //gere la commande monMan

int coupe(char * mot[MaxMot],int nbmots,int index); // fonction qui met à NULL le mot suivant le chevron sur la ligne de commande 
void choixRedirection(char* file, int mode, int flag); // fonction pour gérer la fermeture et l'ouverture des fichiers
int redirection(char * mot[MaxMot],int nbmots); // verifie la presence operateur de redirection
void tube(char * mot[MaxMot],int pipe_index,int nbmots,char * dirs[MaxDirs]); // fonction qui gere 2 commandes liées avec pipe
void execute_command(char *command[MaxMot], char *dirs[MaxDirs]); //execute commande


void gestion_signal(); // Configure le gestionnaire pour SIGCHLD
void child_handler(int sig); //gere chaque processus en arriere plan qui se termine 
void check_bgprocesses(); //verifie si processsus en arriere plan sont terminés

#endif

