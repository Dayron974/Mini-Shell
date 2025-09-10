/*# *******************************************************
# Nom ..........: myshell.c
# Rôle ........ : Auteur
# Auteur ...... : Doryan, Dijoux
# Version ..... : V0.4 du 7/08/2023
# Licence ..... : réalisé dans le cadre du cours Systèmes d'Exploitation
# (L2/SE)
# Compilation : make
# Usage : Pour exécuter : ./myshell
#********************************************************/

#include "sys.h"

bgprocess *bgprocesses = NULL; //variabe globale pour que child_handler puisse acceder au processus d'arriere plan

int main(int argc, char * argv[]) 
{
	char ligne[MaxLigne];
	char * mot[MaxMot];
	char * dirs[MaxDirs];
	char prompt[MaxPathLength] = "?";	// taille max du prompt sera la meme que la taille max d'un nom de fichier
	int nbmots;
	char *path;
	int bg = 0; // flag pour commande en arrière-plan
	
	// Configure le gestionnaire pour le signal SIGCHLD
	gestion_signal();
	// Decouper UNE COPIE de PATH en repertoires 
	path = strdup(getenv("PATH"));
	nbmots = decouper(path, ":", dirs, MaxDirs);

	// Lire et traiter chaque ligne de commande 
	for(printf("%s",prompt); fgets(ligne, sizeof ligne, stdin) != 0; printf( "%s",prompt))
	{
		check_bgprocesses(); //verifie si les processus lancés en arriere plan sont terminés
		bg = 0;
		nbmots = decouper(ligne, " \t\n", mot, MaxMot);
		if (mot[0] == 0) // ligne vide
		continue;
		else if (strcmp(mot[nbmots-1], "&") == 0) 
		{
			bg = 1; // Marquer que le processus doit être lancé en arrière-plan
			mot[nbmots-1] = 0; // Supprimer "&" du tableau de commande
			nbmots--; // Réduire le nombre de mots dans la commande
		}
		//si le premier mot sur ligne de commande est chdir 
		if(strcmp(mot[0], "chdir")==0)
		{
			mcd(nbmots,mot,prompt);
		}
		else if(strcmp(mot[0], "monexit")==0)
		{
			free(path);
			exit(0);	// sort du programme actuel 
		}
		else if(strcmp(mot[0], "monMan")==0)
		{
			monMan(nbmots, mot);
		}
		else	// creation processus enfant 
		{
			executer_enfant(ligne, mot, dirs, nbmots, bg);
		}
	}
	return 0;
}
// decoupe une chaine à partir d'un caractere de separation
int decouper(char * ligne, char * separ, char * mot[], int maxmot){
 	int i ;

	mot[0] = strtok( ligne, separ);
	for( i = 1; mot[i-1] != 0; i++){
		if ( i == maxmot){
			fprintf ( stderr, "Erreur dans la fonction decouper: trop de mots\n");
			mot[i-1] = 0;
			break;
			}
		mot[i] = strtok( NULL, separ);
	}
	return (i -1);
}

