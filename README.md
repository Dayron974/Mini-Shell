# Mini-Shell
Réalisation d'un shell élémentaire qui inclut certaines fonctionnalités de base.

## Description 
Les fonctionnalités sont : l’implémentation des commandes cd, exit et man, la gestion des redirections d’entrées-sorties, la gestion des pipes, la gestion des processus en arrière-plan.

## Prérequis

- Compilateur GCC
- Make
  
## Utilisation

Compilation : make
Usage : Pour exécuter : ./myshell

## Contraintes 

Ce shell ne permet qu’une seule redirection d’entrée/sortie par ligne de commande. Pour ajouter la possibilité de
plusieurs redirections d’entrées/sorties, il faudrait modifier la logique du programme. C’est
à dire qu’il faudrait changer la manière de traiter une ligne de commande en la séparant en
plusieurs commandes separées par les chevrons et le fichier de redirection. Puis, réaliser autant
d’execv qu’il y a de commandes. Le même problème s’est posé pour les pipes.

## Statut

Projet étudiant de deuxième année de licence Informatique de l'université Paris 8. Il est réalisé dans le cadre du cours Systèmes d'Exploitation.
Année universitaire : 2022/2023.
