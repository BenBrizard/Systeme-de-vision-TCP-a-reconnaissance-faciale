#ifndef INCLUDE_CLIENT_H_
#define INCLUDE_CLIENT_H_
/**
 * \var ESC
 * Touche qui est enregistrée par la fonction waitKey() lorsque la touche ESC est appuyée
 */
#define ESC 1048603
 /**
  * \var numPad1
  * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 1 du pad numerique est appuyée
  */
#define numPad1 1114033
  /**
   * \var numPad2
   * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 2 du pad numerique est appuyée
   */
#define numPad2 1114034
   /**
	* \var numPad3
	* Touche qui est enregistrée par la fonction waitKey() lorsque la touche 3 du pad numerique est appuyée
	*/
#define numPad3 1114035
	/**
	 * \var numPad4
	 * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 4 du pad numerique est appuyée
	 */
#define numPad4 1114036
	 /**
	  * \var charPad1
	  * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 1 du clavier est appuyée
	  */
#define charPad1 1048625
	  /**
	   * \var charPad2
	   * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 2 du clavier est appuyée
	   */
#define charPad2 1048626
	   /**
		* \var charPad3
		* Touche qui est enregistrée par la fonction waitKey() lorsque la touche 1 du clavier est appuyée
		*/
#define charPad3 1048627
		/**
		 * \var charPad4
		 * Touche qui est enregistrée par la fonction waitKey() lorsque la touche 1 du clavier est appuyée
		 */
#define charPad4 1048628

		/**
		 * \var IDOWN
		 * Signifie que le serveur ne détecte pas de lumiere : il n'y a pas de transfert d'images.
		 */
#define IDOWN 0b1
		/**
		 * \var PUSHB
		 * Le serveur détecte que le bouton est pressé et qu'il y a de la lumiere : il y a sauvegarde d'une photo du côté client.
		 */
#define PUSHB 0b10
		/**
		 * \var READY
		 * Le serveur détecte qu'il y a de la lumière, le client va recevoir une image.
		 */

#define READY 0b11

		/**
		 * \var TRESHOLD
		 * Variable spécifiant la valeur limite à partir de laquelle l'IA va reconnaître un visage ou non.
		 */
#define TRESHOLD 7000
		
		
#include<time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include<iostream>
#include <fstream>
#include <sstream>



/**
 * \struct FPS { int resX ; int resY ; double fps;}
 * \brief Contient toutes les resolution parmi lesquelles on doit choisir et le nombre d'images par secondes
 * correspondants.
 * \param resX
 * Nombre de pixel definissant la resolution en X
 * \param resY
 * Nombre de pixel definissant la resolution en Y
 * \param fps
 * Nombre d'images par secondes (fps) associees a la resolution corespondante.
 */
struct FPS { int resX ; int resY ; double fps;};

/**
 * \fn error(const char *msg)
 * \brief Arrete l'execution du programme et affiche un message d'erreur
 * @param msg
 * Message d'erreur a afficher
 */
void error(const char* msg);


/**
 * \fn client_webcam()
 * \brief Cette ouvre un socket TCP et tente de se connecter au serveur. Suite à la connection l'utilisateur décide
 * quelle résolution utiliser et l'envoie au serveut. Le client reçoit un message du serveur lui indiquant s'il 
 * doit mettre à jour l'image. L'affichage est mis à jour après la réception de l'image. Si le message reçu du 
 * serveur indique que le bouton a été appuyé, la fonction menuAI est appelée. Le client envoit ensuite des 
 * messages au serveur pour changer la résolution ou interrompre la communication.
 */
void client_webcam();


/**
 * \fn change_img_res()
 * \brief Cette fonction permet de modifier les dimensions de la structure image pour s'adapter à la nouvelle 
 * résolution.
 * @param image
 * Structure dont les dimensions doivent être modifiées pour s'adapter à la nouvelle résolution.
 * @param resY
 * Nombre de pixels verticals à assigner à l'image.
 * @param resX
 * Nombre de pixels horizontals à assigner à l'image.
 * \return Taille de l'image modifiée.
 */
int change_img_res(cv::Mat & image, int resY, int resX);


/**
 * \fn select_resolution()
 * \brief Cette fonction demande à l'usager de choisir une résolution et retourne la valeur de celle-ci seulement si elle est valide.
 * \return Indice de la résolution à utiliser.
 */
int select_resolution();


/**
 * \fn detectAndDraw()
 * \brief Cette fonction prend une image en paramètre et l'analyse afin de détecter des visages dans cette image. 
 * Elle peut détecter plusieur visages à la fois. 
 *
 * Si le mode est apprentissage, la fonction ne fonctionne qu'avec un seul visage. La foncion découpe l'image pour
 * ne garder que le visage et l'aggrandit à une grandeur spécifique. Cette image est ensuite enregistrée avec un
 * timestamp pour ne pas écraser les autres images. Le fichier faisant la liste des photos est aussi mise à jour.
 *
 * Si le mode est reconnaissance, la fonction encadre tous les visages détectés avec un rectangle bleu. Tous les 
 * visages sont ensuite analysés par l'intelligence artificielle de reconnaissance faciale. Si le visage est connu
 * de l'IA, le nom de la personne sera écrit au dessus de son visage. S'il n'est pas connu, les mots inconnus 
 * seront écrits au dessus du visage de la personne. L'image modifiée est finalement enregistrée avec un timestamp 
 * pour que les images prises précedemment ne soient pas écrasées.
 * @param img
 * Image sur laquelle doit etre fait la reconnaissance d'image ou l'apprentissage.
 * @param mode
 * Indique si on doit découper l'image ou si on veut l'encadrer et identifier la personne.
 * @param path
 * Repertoire dans lequel les images doivent etre enregistres. Ce paramètre est optionnel.
 * @param choix
 * Entier qui identifie la personne pour laquelle on veut ajouter une image. Ce paramètre est optionnel.
 * @param choix_resolution
 * Indice de la résolution utilisée. Permet d'adapter la grosseur de l'écriture sur l'image.
 */
void  detectAndDraw( cv::Mat& img,int mode, std::string path, int choix,int choix_resolution);

/**
 * \fn menuAI()
 * \brief Cette fonction demande à l'usager s'il souhaite enregistrer de nouvelles photos (mode apprentissage), 
 * faire la reconnaissance des visages ou entraîner l'AI. Si le mode est apprentissage l'usager doit spécifier
 * l'utilisateur pour lequel on ajoute une image. Si le mode est apprentissage ou reconnaissance la fonction 
 * detectAndDraw est appelée. Si le mode est entraînement, toutes les images de la base de données sont fournies
 * à l'IA pour qu'il soit entraîné.
 * @param mode 
 * Mode dans lequel l'application est utilisée.
 * @param choix
 * Identifiant de la personne choisie.
 * @param img
 * Image reçue du serveur.
 * @param resolution
 * Id de la résolution associée à l'image.
 */
void menuAI(int& mode, int& choix, cv::Mat& img, int resolution);


/**
 * \fn menuAI()
 * \brief Cette fonction permet de lire le nom des photos et leur identifiant associé et de mettre ces informations
 * dans le vecteur correspondant.
 * @param filename 
 * Nom du fichier csv dans lequel on souhaite lire les informations
 * @param images
 * Vecteur dans lequel les noms des images seront insérés.
 * @param labels
 * Vecteur dans lequel les identifiants associés à chaque image seront insérés.
 * @param separator
 * Charactère utilisé pour faire la séparation dans le fichier csv.
 */
void read_csv( const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator);


#endif /* INCLUDE_CLIENT_H_ */
