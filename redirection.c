/*# *******************************************************
# Nom ..........: redirection.c
# Rôle ........ : Auteur
# Auteur ...... : Doryan, Dijoux
# Version ..... : V0.5 du 08/08/2023
# Licence ..... : réalisé dans le cadre du cours Systèmes d'Exploitation
# (L2/SE)
# Compilation : make
# Usage : Pour exécuter : ./myshell
#********************************************************/

#include "sys.h"

int coupe(char * mot[MaxMot],int nbmots,int index) // fonction qui met à NULL le mot suivant le chevron sur la ligne de commande 
{
	int i;
	// les mots suivant la redirection sont mis a NULL
	if((nbmots - index) > 2)
	{
		fprintf(stderr, "Ce shell n'accepte qu'une seule redirection par ligne. Les mots suivants le fichier de redirection seront ignorés.\n");
	}
	// a parti du chevron mettre tous les mots a NULL
	for(i= index;i<nbmots ;i++)
	{
		mot[i] = NULL;
	}
	nbmots = nbmots -i;
	return nbmots;
}

// fonction pour gérer la fermeture et l'ouverture des fichiers
void choixRedirection(char* file, int mode, int flag) 
{ 
	int t;
	t = close(mode);// fermer fichier 0,1 ou 2 
	assert(t >= 0);
	
	// ré−ouvrir vers le fichier demandé sur ligne de commande
	t = open(file, flag, 0666);
	if(t != mode)
	{
		fprintf(stderr, "Erreur : impossible d'ouvrir fichier de redirection\n");
		exit(1);
	}
}	

// verifie la presence operateur de redirection
int redirection(char * mot[MaxMot],int nbmots)
{
	int i;
	int pipe_index = 0;
	for(i=0; i<nbmots; i++) // parcours de la ligne de commande 
	{
		if(strcmp(mot[i], ">")==0) // redirection sortie standard
		{
			choixRedirection(mot[i+1], 1, O_WRONLY|O_CREAT);
			nbmots = coupe(mot,nbmots,i); 

		}	
		else if(strcmp(mot[i], "<")==0)// redirection entree standard
		{
			choixRedirection(mot[i+1], 0, O_RDONLY|O_CREAT);
			nbmots = coupe(mot,nbmots,i);
		}
		else if(strcmp(mot[i], "2>")==0)// redirection sortie d'erreur
		{
			choixRedirection(mot[i+1], 2, O_WRONLY|O_CREAT);
			nbmots = coupe(mot,nbmots,i);
		}
		else if(strcmp(mot[i], ">>")==0) // redirection sortie standard sans ecraser
		{
			choixRedirection(mot[i+1], 1, O_WRONLY|O_CREAT|O_APPEND);
			nbmots = coupe(mot,nbmots,i);
		}
		else if(strcmp(mot[i], "2>>")==0)// redirection sortie d'erreur sans ecraser
		{
			choixRedirection(mot[i+1], 2, O_WRONLY|O_CREAT|O_APPEND);
			nbmots = coupe(mot,nbmots,i);

		}
		else if(strcmp(mot[i], "|")==0)// pipe
		{
			pipe_index = i; // recupere index du pipe
			return pipe_index;
		}
	}
	return 0;
}

// fonction qui gere 2 commandes liées avec pipe
void tube(char * mot[MaxMot],int pipe_index,int nbmots,char * dirs[MaxDirs])  
{
	int status, fd[2], t, f, i;
	char * command[pipe_index +1];
	char * command2[nbmots-pipe_index];
	char* redirection_operators[] = {"|", "<", ">", ">>", "2>", "2>>", NULL};
	
	// Copier les mots de la ligne de commande dans deux tableaux differents
	for (i = 0; i < pipe_index; i++) 
	{
		command[i] = mot[i];
	}
	command[pipe_index] = NULL; //terminer le tableau 
	for (i = pipe_index + 1; i < nbmots; i++) 
	{
		//cherche un operateur de redirection sur la ligne de commande
		for (char **op = redirection_operators; *op; op++)
		{
			if (strcmp(mot[i], *op) == 0)// affiche message d'erreur
			{
				fprintf(stderr, "Erreur: Ce shell n'accepte qu'une seule redirection par ligne.\n");
				exit(1); 
			}
		}
        	command2[i - pipe_index - 1] = mot[i];
	}
	command2[nbmots - pipe_index - 1] = NULL; // Terminer le tableau avec NULL
	
	t = pipe(fd); // fonction qui cree le pipe 
	assert(t >= 0);
	
	f = fork(); // creer un processus qui va realiser la deuxieme commande 
	if (f < 0)
	{
		perror("fork");
	}
	if (f != 0)// parent : ecrit la sortie de la commande dans le tube
	{ 
		close(fd[0]); // ferme le cote lecture pour eviter resultats inattendus
		dup2(fd[1], 1); // connecte le cote ecriture avec stdout 
		close(fd[1]); 
		
		//exec de la commande
		execute_command(command, dirs);
		waitpid(f, &status, 0);
	}
	else
	{
		close(fd[1]); // ferme le cote ecriture pour eviter resultats inattendus
		dup2(fd[0],0); // connecte le cote lecture avec stdin
		close(fd[0]);
		
		//exec de la commande
		execute_command(command2, dirs);
	}
}
