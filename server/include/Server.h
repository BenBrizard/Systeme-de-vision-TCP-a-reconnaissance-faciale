#ifndef INCLUDE_SERVER_H_
#define INCLUDE_SERVER_H_
		/**
		 * \var SEUIL
		 * Seuil de l'ADC, si on lit une valeur plus petite que SEUIL, c'est qu'il y a assez de lumiere pour la webcam
		 */
#define SEUIL 1000

		/**
		 * \var IDOWN
		 * Signifie que le serveur ne detecte pas de lumiere : il n'y a pas de transfert d'images.
		 */
#define IDOWN 0b1
		/**
		 * \var PUSHB
		 * Le serveur detecte que le bouton est presser et qu'il y a de la lumiere : il y a sauvegarde d'une photo du cote client.
		 */
#define PUSHB 0b10
		/**
		 * \var READY
		 * Le serveur detecte qu'il y a de la lumiere, on transmet donc les images.
		 */

#define READY 0b11

#include<iostream>
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>

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
 * \fn server_webcam()
 * \brief Fonction qui initialise les fichiers de l'ADC et d'un gpio, qui ouvre une connection TCP et attend 
 * qu'un client s'y connecte. Le serveur envoie alors des captures d'image chaque 30 ms jusqu'à ce que 
 * l'utilisateur lui signifie d'arrêter en changeant un bit dans un registre de configuration. Le client 
 * signifie au serveur la resolution des images qu'il desire recevoir. Le client peut demander un changement 
 * de resolution a tout moment. Le serveur continue d'envoyer des images en continu meme si la resolution 
 * change. Cette fonction vérifie aussi que la caméra est dans un endroit lumineux et supervise le bouton
 * poussoir pour dire au client d'enregistrer une photo si un front montant est détecté depuis le bouton.
 */
void server_webcam();

/**
 * \fn write_to_file()
 * \brief Fonction permettant d'ajouter du texte dans un fichier.
 * @param file
 * Nom du fichier à modifier. Le nom doit inclure l'arborescence du fichier si celui-ci n'est pas le répertoire
 * courant.
 * @param input
 * Texte qui doit être écrit dans le fichier.
 */
void write_to_file(char * file, char *input);

/**
 * \fn change_resolution()
 * \brief Fonction permettant de modifier un objet de type Mat afin d'adapter la résolution des images prises par
 * la caméra.
 * @param image
 * Image dont la résolution doit être modifiée.
 * @param capture
 * Vidéo dont la résolution doit être modifiée par la fonction.
 * @param resY
 * Nombre de pixels verticals à assigner au vidéo et à l'image.
 * @param resX
 * Nombre de pixels horizontals à assigner au vidéo et à l'image.
 * \return Grosseur de l'image modifiée. 
 */
int change_resolution(cv::Mat &image, cv::VideoCapture &capture,int resY, int resX);

/**
 * \fn read_gpio()
 * \brief Fonction permettant de lire la valeur d'un gpio.
 * @param file
 * Nom du fichier conrespondant au gpio à lire.
 * @param result
 * Variable dans laquelle est retournée ce qui est lu dans le fichier du gpio.
 */
void read_gpio(char * file, char& result);

/**
 * \fn read_adc()
 * \brief Fonction qui permet de lire la valeur contenue dans le fichier de l'adc de l'Odroid-C2.
 * \return Valeur lue par l'adc.
 */
int read_adc();

#endif


