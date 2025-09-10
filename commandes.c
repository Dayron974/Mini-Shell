/*# *******************************************************
# Nom ..........: commandes.c
# Rôle ........ : Auteur
# Auteur ...... : Doryan, Dijoux
# Version ..... : V0.4 du 8/08/2023
# Licence ..... : réalisé dans le cadre du cours Systèmes d'Exploitation
# (L2/SE)
# Compilation : make
# Usage : Pour exécuter : ./myshell
#********************************************************/

#include "sys.h" 

extern bgprocess *bgprocesses;  // Déclaration de la variable globale
//execute commande
void execute_command(char *command[MaxMot], char *dirs[MaxDirs])
{
	char path[MaxPathLength];
	int i;

	// exec de la commande
	for(i = 0; dirs[i] != 0; i++)
 	{
       		snprintf(path, sizeof path, "%s/%s", dirs[i], command[0]);
        	execv(path, command);
	}
	fprintf(stderr, "%s: not found\n", command[0]);
	exit(1);
}

// lance un processus enfant 
void executer_enfant(char *ligne, char *mot[MaxMot], char *dirs[MaxDirs], int nbmots, int bg)
{
	int tmp, pipe_exist = 0;
	int pipefd[2];
	
	if(bg == 1)
	{
		pipe(pipefd); // cree un pipe pour processus en bg
	}
	tmp = fork(); // lancer le processus enfant
	if (tmp < 0)
	{
		perror("fork") ;
		return;
	}
	if ( tmp != 0) // parent
	{ 
		if(bg == 1) // si le processus doit être exécuté en arrière-plan
		{
			close(pipefd[1]); // Ferme le côté écriture dans le processus parent
			// ajout du processus a la liste des processus en arriere plan
			bgprocess *new_process = malloc(sizeof(bgprocess));
			new_process->pid = tmp; // le PID du nouveau processus
			new_process->cmd = strdup(ligne); // copie de la ligne de commande
			new_process->output = NULL; // sortie du processus
			new_process->next = bgprocesses; 
			new_process->pipe_read = pipefd[0]; // stocke le côté lecture du pipe
			bgprocesses = new_process;
		}
		else // attendre la fin de l ' enfant
		{
			while(wait(0) != tmp)
			;
		}
		return;
	}
	// enfant : test redirections et exec du programme
	if (bg == 1) 
	{
		close(pipefd[0]); // Ferme le côté lecture
		dup2(pipefd[1], STDOUT_FILENO); // redirige stdout vers le pipe
		close(pipefd[1]); // Ferme le côté écriture après la redirection
	}
	pipe_exist = redirection(mot, nbmots);
	if(pipe_exist > 0)
	{
		tube(mot,pipe_exist,nbmots,dirs);
	}
	else
	{
		execute_command(mot, dirs);
	}
}

void mcd(int nb,char * mot[MaxMot],char prompt[MaxPathLength]) // commande chdir
{
	char * wd = NULL;;
	int t;
	if ( nb != 2) // deux mots uniquement sur la ligne de commandes
	{
		fprintf ( stderr, "usage: %s un seul parametre est necessaire\n", mot[0]) ;
	}
	else
	{
		wd = mot[1];
		t = chdir(wd);	//appel systeme chdir pour changer repertoire courant
		if ( t < 0)	//test echec de chdir
		{
			perror("chdir");
		}
		else
		{
			//nouveau prompt en fonction du repertoire courant
			strcpy(prompt,wd);	
			strcat(prompt," :");
		}
		
	}
}
//gere la commande monMan
void monMan(int nb, char *mot[MaxMot])
{
	char filename[MaxPathLength];
	char line[MaxLigne];
	if (nb != 2)
	{
		printf("Usage : 'monMan [commande]'\n");
	}
	else
	{
		// Construire le nom du fichier
        	snprintf(filename, sizeof(filename), "manPages/%s.txt", mot[1]);
        	FILE *file = fopen(filename, "r");
        	if (file == NULL)
		{
		    printf("Il y a eu un probleme lors de l'ouverture de la documentation\n");
		}
		else
		{
			// Lire et afficher chaque ligne du fichier
			while (fgets(line, sizeof(line), file) != NULL)
			{
				printf("%s", line);
			}
		    
		    // Fermer le fichier
		    fclose(file);
		}
	}
}
