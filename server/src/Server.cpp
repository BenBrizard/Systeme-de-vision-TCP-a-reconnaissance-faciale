#include "Server.h"
using namespace std;
using namespace cv;

/*****************************************************************************************************************
* 						   Global variables declaration
*****************************************************************************************************************/




void error(const char* msg)
{
	perror(msg);
	exit(0);
}


void write_to_file(char * file, char * input)
{
	std::fstream myfile_(file, std::ios_base::out);
	if (myfile_.fail())
		error("Pas reussi a ouvrir le fichier demandé \n");
	myfile_ << input;
	myfile_.close();
}

int change_resolution(cv::Mat &image, cv::VideoCapture &capture,int resY, int resX)
{
	capture.set(CV_CAP_PROP_FRAME_WIDTH, resX);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, resY);
	image.Mat::create(resY, resX, CV_8UC3);
	return image.total() * image.elemSize();
}

void read_gpio(char * file, char& result)
{
	std::fstream myfile_gpio(file, std::ios_base::in);
	if (myfile_gpio.fail())
		error("Pas reussi a ouvrir le fichier demandé \n");

	myfile_gpio.get(result);
	myfile_gpio.close();
}

int read_adc()
{
	std::fstream myfile_adc("/sys/class/saradc/ch0", std::ios_base::in);//Le nom du fichier peut changer selon le matériel utilisé pour le serveur et le système de fichiers.
	if (myfile_adc.fail())
		error("Pas reussi a ouvrir le fichier de l'adc \n");
	int i = 0;
	char character;
	char buffer[10];
	while (myfile_adc.get(character))
	{
		buffer[i] = character;
		i++;

	}
	myfile_adc.close();
	return atoi(buffer);
}

void server_webcam()
{
	FPS FPS_Logitech[4];//4 résolutions utilisables
	FPS_Logitech[0] = { 176, 144, 0 };
	FPS_Logitech[1] = { 160, 120, 0 };
	FPS_Logitech[2] = { 320, 240, 0 };
	FPS_Logitech[3] = { 960, 720, 0 };
	
	int fd = 0;
	uint32_t valeur_adc = 0, message_rcv = 0, message_send = 0, choix_resolution = 0, choix_resolution2 = 0;
	char buf[10];
	int buf_itr = 0;
	char appuyer_positive_logic,c ;
	char old_appuyer_positive_logic = '0';
	int appuyer = 0, toutEstbeau = 1;
	int sockfd, newsockfd, portno=4099;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n_rcv, n_send;




	Mat image;

	/*****************************************************************************************************************
	 * 						   Initialize the files used by the Odroid's adc and a gpio pin
	 *****************************************************************************************************************/
	write_to_file("/sys/class/gpio/export", "228");
	write_to_file("/sys/class/gpio/gpio228/direction", "in");
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char*)& serv_addr, sizeof(serv_addr));
	//sil y a sauvegarde d'une photo
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr*) & serv_addr,sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr*) & cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");


	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cout << "Failed to connect to the camera." << endl;
	}


	/*****************************************************************************************************************
	 * 							Recveive resolution 1st time
	 *****************************************************************************************************************/

	if ((n_rcv = recv(newsockfd, &message_rcv, 4, 0)) == -1)
	{
		error("recv du message failed");
	}
	choix_resolution = (message_rcv & 6) >> 1;//lecture des 2èmes et 3èmes bits les moins significatif pour le changement de résolution.

	choix_resolution2 = choix_resolution;

	int imgSize = change_resolution(image, capture, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);


	while (toutEstbeau == 1)
	{
		valeur_adc = read_adc();

		read_gpio("/sys/class/gpio/gpio228/value", appuyer_positive_logic);

		if (appuyer_positive_logic == '1')
			appuyer_positive_logic = '0';
		else
			appuyer_positive_logic = '1';

		if (old_appuyer_positive_logic == '0' && appuyer_positive_logic == '1')
			appuyer = 1;
		else
			appuyer = 0;

		old_appuyer_positive_logic = appuyer_positive_logic;

		if (valeur_adc < SEUIL)
		{
			if (appuyer)
			{
				message_send = PUSHB;//luminosité suffisante et front montant sur le bouton poussoir
			}

			else
			{
				message_send = READY;//luminosité suffisante
			}
		}
		else
		{
			message_send = IDOWN;//luminosité insuffisante
		}

		/*****************************************************************************************************************
		 * 							 Envoie du message au client, permettant le transfert d'image ou non
		 *****************************************************************************************************************/
		if ((n_send = send(newsockfd, &message_send, 4, 0)) == -1)
		{
			error("sending the message failed");
		}


		if (message_send != IDOWN)
		{
			capture >> image;
			if (image.empty())
			{
				close(newsockfd);
				error("Failed to capture an image");

			}

			/*****************************************************************************************************************
			 * 					1						Envoi de l'image
			 *****************************************************************************************************************/


			if (n_send = send(newsockfd, image.data, imgSize, 0) == -1)
			{
				error("sending the image failed");
			}



			/*****************************************************************************************************************
			 * 							Réception de la prochaine résolution
			 *****************************************************************************************************************/
			if ((n_rcv = recv(newsockfd, &message_rcv, 4, 0)) == -1)
			{
				error("recv failed");
			}
			else
			{
				if (message_rcv & 1)
					toutEstbeau = 1;
				else
				{
					toutEstbeau = 0;//si le client a envoyé un message d'interruption de la communication on arrête l'application serveur
				}

				choix_resolution2 = (message_rcv & 6) >> 1;
				if (choix_resolution2 != choix_resolution)
				{
					cout << "Changed resolution to " << (int)choix_resolution2 << endl;
					choix_resolution = choix_resolution2;
					imgSize = change_resolution(image, capture, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);

				}

			}
		}
	} //while
	cout << "Terminating the socket" << endl;
	close(newsockfd);

}
