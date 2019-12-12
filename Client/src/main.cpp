/**
\mainpage
Ce programme ouvre un socket TCP et tente de se connecter au serveur. Suite a la connection, le client envoie un 
message au serveur, lui indiquant la résolution à utiliser. Le client reçoit un message du serveur lui indiquant
s'il doit mettre à jour l'image (luminosité suffisante au serveur), si c'est le cas l'exécution continue normalement,
sinon le client attend un message du serveur lui disant de mettre à jour l'image. Lorsque la nouvelle image est bien
reçue le client met à jour l'affichage. Si le message reçu du serveur indique que le bouton a été appuyé, une
fonction de reconnaissance faciale est appelée. L'utilisateur peut alors décider d'ajouter l'image dans sa base de
données, d'entrâiner l'intelligence artificielle ou demander à l'intelligence artificielle de reconnaître les 
visages sur la photo, s'ils y en a. Le client permet aussi d'envoyer des messages au serveur pour changer la résolution
ou interrompre la communication.
**/
#include "Client.h"
using namespace cv;
using namespace std;


int main(void)
{

	client_webcam();

    return 0;
}
