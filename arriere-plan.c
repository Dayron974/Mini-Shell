/*# *******************************************************
# Nom ..........: arriere-plan.c
# Rôle ........ : Auteur
# Auteur ...... : Doryan, Dijoux
# Version ..... : V0.3 du 08/08/2023
# Licence ..... : réalisé dans le cadre du cours Systèmes d'Exploitation
# (L2/SE)
# Compilation : make
# Usage : Pour exécuter : ./myshell
#********************************************************/

#include "sys.h"

extern bgprocess *bgprocesses;  // Variable globale processus arriere-plan

// Configure le gestionnaire pour SIGCHLD
void gestion_signal()
{
	struct sigaction sa;
	sa.sa_handler = &child_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1) 
	{
		perror(0);
		exit(1);
	}
}

//gere chaque processus en arriere plan qui se termine 
void child_handler(int sig) {
	int status;
	pid_t pid;
	//tant qu'il y a des processus fils qui ont terminé
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 
	{
	// recherche du processus dans la liste et marque comme terminé
		for (bgprocess *p = bgprocesses; p != NULL; p = p->next) 
		{
			if (p->pid == pid) 
			{
				p->terminated_cmd = p->cmd; // garde la ligne de commande
				p->cmd = NULL; //indique que le processus est terminé
			}
		}
	}
}

//verifie si processsus en arriere plan sont terminés
void check_bgprocesses() 
{
	char buffer[MaxLigne]; 
	ssize_t bytes; // pour lire la sortie du processus
	bgprocess **pp = &bgprocesses;
	while (*pp != NULL) 
	{
		if ((*pp)->cmd == NULL) 
		{ // le processus s'est terminé
			// Lis la sortie du processus
			bytes = read((*pp)->pipe_read, buffer, sizeof(buffer) - 1);
			if(bytes < 0)
			{
				perror("Erreur dans la lecture de la sortie du processus bg");
				exit(1);
			}
			buffer[bytes] = '\0';
			// Stocke la sortie
			(*pp)->output = strdup(buffer);
			
			printf("Background job %d (command: %s) finished\n", (*pp)->pid, (*pp)->terminated_cmd);
			
			if ((*pp)->output) 
			{
				printf("%s\n", (*pp)->output);
				free((*pp)->output);
			}

			// Ferme le côté lecture du pipe
			close((*pp)->pipe_read);
            
			// libere memoire allouée du processus terminé
            		free((*pp)->terminated_cmd); 
            		bgprocess *to_delete = *pp;
            		*pp = (*pp)->next; 
            		free(to_delete);
        	} 
		else 
		{
			pp = &(*pp)->next; // passe au processus suivant
		}
	}
}
