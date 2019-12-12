#include "Server.h"
using namespace std;
using namespace cv;

/**
\mainpage
Ce programme initialise l'adc et un gpio du serveur.Il ouvre un socket TCP et attend indéfiniment qu'un client 
s'y connecte. Le serveur démarre ensuite la camér et attend que le client lui fournisse une résolution à 
utiliser avec la caméra. Le programme entre ensuite dans une fonction qui se répète tant que le serveur ne 
reçoit pas un message d'arrêt qui est un entier non-signé de 32 bits ayant le bit le moins significatif à 0.
Dans cette boucle le serveur vérifie qu'il y a de la lumière et s'il a détecté un front montant provenant du
bouton poussoir. S'il y a de la lumière, le serveur continue à envoyer des images. Si en plus il y a eu un 
front montant le serveur indique au client qu'il doit sauvegarder l'image. S'il n'y avait pas de lumière, 
aucune image n'est envoyée et le serveur indique au client qu'il n'y a pas de lumière.

**/



int main()
{
	server_webcam();
    return 0;
}


