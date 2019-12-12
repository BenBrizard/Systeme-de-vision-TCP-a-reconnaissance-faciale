# Système de vision TCP à reconnaissance faciale

## Description
Ce projet est un exercice théorique visant à familiariser les élèves à la programmation de socket et à la compilation croisée. Une application de type client-serveur est ici implémentée. Le serveur est implémenté sur un board Odroid-C2. Celui-ci est relié à un petit circuit et une caméra USB y est branchée. D'abord, on exécute l'application serveur, qui attend une connexion TCP d'un client. On démarre ensuite l'application client. Celui-ci communique avec le serveur et spécifie la résolution de la vidéo qu'il veut recevoir. Le client peut changer cette résolution à tout moment. Le circuit côté serveur contient une photo-résistance. Lorsque la photo-résistance est exposée à la lumière, le serveur ouvre la caméra USB et envoie une vidéo en temps réel au client. Si la photo-résistance est couverte, aucune image n'est envoyée. Sur le circuit côté serveur, on peut appuyer sur un bouton. Si celui-ci est pressé, le serveur le signale au client. Le client sauvegarde alors cette image. L'application propose 3 modes au client :


- 0 : Mode apprentissage 
Si la photo ne contient qu'un seul visage, le client indique qui se trouve sur la photo. La photo est ajoutée à la base de données permettant d'entraîner l'intelligence artificielle de reconnaissance faciale.
-  1 : Mode reconnaissance 
L'application utilise l'intelligence artificielle (voir FaceRecognizer et EigenFaceRecognizer dans la documentation d'openCV [https://docs.opencv.org/2.4/modules/contrib/doc/facerec/facerec_tutorial.html]) pour identifier les personnes sur la photo. L'intelligence artificielle base sa prédiction à l'aide des photos fournies dans le mode apprentissage.
-  2 : Mode training AI 
Lorsque plusieurs photos sont ajoutées à la base de données, on peut entraîner le modèle. Avant d'entrer en mode reconnaissance, il faut entrer dans ce mode. Si des photos sont ajoutées à la base de données, il faut entrer dans ce mode avant de faire la reconnaissance pour tenir compte des nouvelles photos.

## Dépendances
- OpenCV2 : Prise d'image depuis la caméra, détection et classification de visages.


## Installation
Dans le cadre du projet, les deux exécutables ont été compilés sous Linux. Les commandes données prennent pour acquis que l'environnement de compilation croisée est déjà configuré. Suite au téléchargement du répertoire, accédez-y et effectuez les commandes suivantes. Voici les commandes à effectuer.
### Serveur
Étant dans le répertoire racine du projet, effectuez :
```
mkdir build_server
cd build_server
```
Considérant qu'on compile un exécutable dans une autre architecture, il faut utiliser la commande "source" afin d'utiliser les bons outils de compilation. Entrez ensuite :
```
cmake -D CMAKE_BUILD_TYPE=Release ../server
make
```
Dans le cadre du projet, pour transférer l'exécutable dans l'Odroid, nous proposons la commande suivante :
```
scp Projet_Server root@192.168.7.2:/home/   
```
### Client
Étant dans le répertoire racine du projet, effectuez :
```
mkdir build_client
cd build_client
cmake -D CMAKE_BUILD_TYPE=Release ../Client
make
```
## Utilisation
Exécuter l'application serveur en premier. Allez dans l'Odroid-C2, dans le répertoire "/home" si vous avez suivi les commandes lors de l'installation, et entrez dans un terminal :
```
./Projet_Server
```
Exécuter l'application client par la suite. Allez dans le répertoire build_client où se trouve l'exécutable et entrez dans un terminal :
```
./Projet_Client
```

## Documentation
Le projet est documenté avec Doxygen. Pour générer les fichiers de documentation relatifs au Client, effectuez les commandes suivantes à partir de la racine du répertoire source :
```
cd Client
doxygen Doxyfile
```
Pour visualiser la documentation, aller dans /Client/doc/html et ouvrir le fichier index.html.


Pour la documentation relative à l'application serveur, effectuez les commandes suivantes à partir de la racine du répertoire source :
```
cd server
doxygen Doxyfile
```
Pour visualiser la documentation, aller dans /server/doc/html et ouvrir le fichier index.html.
