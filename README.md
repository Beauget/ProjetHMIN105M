# ProjetHMIN105M

## Compilation

```bash
make
```

## Usage

Premier terminal :
```bash
./server <socket>
```
Deuxième terminal :
```bash
./client <ip> <socket>
```
Si on souhaite lancer dans la même machine l'ip doit être : 127.000.001 

La socket doit être inutilisé. (en générale à partir de 1025 elles sont disponibles)

## Client

Tout les clients connecté simultanement doivent avoir un nom différents et inférieur à 19 caratères. Le nombre maximum de clients connectés est 20.

Quand on indique le nombre de requête(s). Aucune ne doit avoir la même combinaison de site et de type cpu/go 

Exemple interdit:
```
Paris cpu ....
Paris c.....
```
Exemple possible:
```
Paris cpu ....
Paris go.....
```
Tout les sites demandé doivent respecter les majuscules et les minuscules. Sinon la requête ne sera pas traité et considéré "improbable".

Veuillez attendre une réponse après l'envoie d'un lot de requêtes.

3 cas sont possible :
```
Requête(s) effectué(s).
```
Toute les requêtes étaient possible et les données on étaient mise à jour.

```
Requête(s) annulée(s) : improbable(s)
```
Les requêtes ne sont pas possibles pour différentes raisons. (Le nombre demandé est impossible à avoir, le site n'existe pas, le client essaye de prendre au total plus que possible..)

```
Pas possible mais probable,mise en attente le temps de 3 tentatives.
```
Le serveur tente de de réupérer les données demandé dès qu'il à la main. Au bout de 3 tentatives, le clients est prévenu que les requêtes on étaient annulé et l'invite à faire de nouvelle demandes.

Si vous souhaitez vous déconnecter, faire la demande quand elle est disponible:
```
Combien de requêtes?/ Quitter?(q/Q)
```
Il n'est pas possible de supprimer une requête après l'avoir faite.

Les données sont réalloué qu'à la déconnexion du client.

## Serveur

Le serveur affiche des informations concernant les clients connectés/déconnectés.
Après avoir était lancé, aucune manipulation est à faire dessus.

## Quit

Pour quitter correctement il faut d'abords d'assurer qu'il n'y ai plus de client connecté.
Ensuite faire ctrl + c sur le terminal ou est lancé le serveur.

## Changing the shared data

Il est possible de changer les données à manipuler en modifiant le fichier data.txt en réspectant le modèle si dessous:

```
<Nom> <Nombre total de go> <Nombre total de cpu>
<Nom> <Nombre total de go> <Nombre total de cpu>
...
```
note 1 : (Si le serveur à déja étais lancé une première fois il faut redémarrer la machine sinon la création/utilisation de la mémoire partagé ne fonctionnera pas)

note 2 : Le nombre de caratère de chaque <..> ne doit pas dépasser 19.

Amusez vous bien !
