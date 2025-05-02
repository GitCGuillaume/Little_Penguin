# Little Penguin

Ce sujet est un challenge comportant une série d'exercice pour s'ignitier à la programmation kernel Linux

This all subject has been made from the eudyptula challenge.
All credits goes to little@eudyptula-challenge.org
http://eudyptula-challenge.org/

Assignment 01 :

Le but de ce module est d'insérer et retirer un module du kernel.

![image](https://github.com/user-attachments/assets/700f651c-1168-42e1-a25f-d697bcdb88ed)

Assignment 04 :

Ce module doit afficher un message de log lorsqu'un clavier se branche à l'aide du système Udev de Linux
Udev permet d'avoir une gestion des périphériques sous le système d'exploitation Linux
Pour se faire, il suffit de créer une "règle" qu'il traduira depuis son dossier de configuration.

![image](https://github.com/user-attachments/assets/abcf74e7-0798-4123-9518-b7943f57d898)

Assignment 05 :

Il est maintenant temps de créer un périphérique virtuel, cette fois-ci il faut créer une interface qui sert uniquement à être lu, ou à lire des données.

![image](https://github.com/user-attachments/assets/5801a7de-66f2-4d2e-9ed2-9c68cf3cf21d)

Lire le périphérique nous renvoit notre identifiant d'étudiant, envoyer notre identifiant ne doit rien afficher, si la donnée envoyée est fausse, elle enverra un message d'erreur (Argument invalide).

Assignment 07 :

Maintenant je dois monter des périphériques à des fins de test, qui s'appellent, id, jiffies et foo.

id est comme l'assignment 05 avec le nom d'identifiant.

jiffies doit afficher le temps en Epoch depuis le démarrage du système.

foo doit recevoir une valeur depuis un administrateur, et doit pouvoir être lu depuis n'importe quel utilisateur.

![image](https://github.com/user-attachments/assets/0a60e08a-3603-4aa8-a1d1-0af0fa8ed253)

Assignment 09 :

Nous devons maintenant créer un module qui va charger un fichier virtuel dans le dossier /proc, là où nous pouvons observer certains comportement du système que ça soit ses processus ou pour ses composants, comme obtenir toutes les partitions, la version du noyau Linux et bien d'autres

A l'aide des fonctions mise à disposition par le noyau linux, j'ai pu obtenir chaque nom et chaque point de monture qu'il représente.

![image](https://github.com/user-attachments/assets/cbbecf7f-423e-4cae-9130-9343bed97031)
