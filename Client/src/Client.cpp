/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */
#include "Client.h"
using namespace std;
using namespace cv;

//global variable
std::string cascadeName, nestedCascadeName;

void error(const char* msg)
{
	perror(msg);
	exit(1);
}


int change_img_res(cv::Mat & image, int resY, int resX)
{
	image.Mat::create(resY, resX, CV_8UC3);
	return image.total() * image.elemSize();
}

// Function for Face Detection



//La fonction suivante a été copiée de la source suivante.
/***************************************************************************************
*	
*    Titre: Face Recognition with OpenCV
*    Auteur(s): OpenCV documentation
*    Date de consultation: 2019/11/21
*    Disponibilité: https://docs.opencv.org/3.4/da/d60/tutorial_face_main.html
*
***************************************************************************************/
void read_csv( const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}


//La fonction suivante a été copiée de la source suivante et a ensuite été modifiée.
/***************************************************************************************
*
*    Titre: OpenCV C++ Program for Face Detection
*    Auteur(s): Shashwat Jain, geeksforgeeks
*    Date de consultation: 2019/11/14
*    Disponibilité: https://www.geeksforgeeks.org/opencv-c-program-face-detection/
*
***************************************************************************************/

//Certaines modifications proviennent de la source suivante et ont été adaptées à notre cas.
/***************************************************************************************
*	
*    Titre: Face Recognition with OpenCV
*    Auteur(s): OpenCV documentation
*    Date de consultation: 2019/11/21
*    Disponibilité: https://docs.opencv.org/3.4/da/d60/tutorial_face_main.html
*
***************************************************************************************/

void  detectAndDraw(  cv::Mat& img,int mode, std::string path="", int choix=-1, int choix_resolution=0)
{
	double font []={0.6,0.5,0.8,1.0};//grosseurs d'écriture pour le nom
	double rec_height[]={18,15,24,30};//epaisseurs du rectangle qui fera le fond pour l'écriture
	// PreDefined trained XML classifiers with facial features
	CascadeClassifier cascade, nestedCascade;
	Mat img_resized;
	img_resized.create(176,176,CV_8UC3);//cette image sert de gabarit lorsque l'on appelle resize sur d'autres fonctions
	double scale=1;
	// Load classifiers from "opencv/data/haarcascades" directory
	// Change path before execution
	//cascade.load( "/export/tmp/4205_34/ele4205-19-34/Client/include/haarcascade_frontalface_alt.xml" ) ;
	cascade.load( "../Client/include/haarcascade_frontalface_alt.xml" ) ;
	Mat img_clone = img.clone();
	
	vector<Rect> faces;
	Mat gray, smallImg;
	cvtColor( img_clone, gray, COLOR_BGR2GRAY ); // Convert to Gray Scale
	double fx = 1 / scale;
	// Resize the Grayscale Image
	resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
	equalizeHist( smallImg, smallImg );
	// Detect faces of different sizes using cascade classifier
	cascade.detectMultiScale( smallImg, faces, 1.1,2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
	if(mode==0 && faces.size()>1)
	{
		return;
	}
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer(0,TRESHOLD);
	model->load("modele_eigenface.yml");
	string noms="";
	for ( size_t i = 0; i < faces.size(); i++ )
	{
		Rect r = faces[i];
		Point center;
		Scalar color = Scalar(255, 0, 0); // Color for Drawing tool
		int radius;
		double aspect_ratio = (double)r.width/r.height;

		if(mode==1)
		{

			Mat img_crop=img_clone.clone();//img_crop est utilisé comme image qui est passé à l'IA pour faire l'identification
			rectangle( img_clone, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),cvPoint(cvRound((r.x + r.width-1)*scale),cvRound((r.y + r.height-1)*scale)), color, 3, 8, 0);//entourer le visage avec un rectangle bleu
			Rect myROI(faces[i].x,faces[i].y,faces[i].width,faces[i].height);
			img_crop=img_crop(myROI);
			resize(img_crop, img_resized, img_resized.size(), 0, 0);
			cvtColor(img_resized, img_resized,CV_BGR2GRAY);
			int result;
			double confiance;
			model->predict(img_resized,result,confiance);
			if(result==-1)
				noms="Inconnu";
			else
			{
				std::ifstream listeDeNomIn("ListeDeNom.csv", ifstream::in);//aller chercher le nom correspondant à l'id
				if (listeDeNomIn)
				{

					int itr=0;
					while(itr<=result)
					{
						getline(listeDeNomIn,noms);
						itr++;
					}
					listeDeNomIn.close();
				}
				else
					error("Le fichier contenant les noms n'a pas pu être ouvert");

			}
			Point coin(faces[i].x,faces[i].y);
			rectangle( img_clone, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),cvPoint(cvRound((r.x + r.width)*scale),cvRound((r.y -rec_height[choix_resolution])*scale)), color, CV_FILLED, 8, 0);//mettre un fond bleu en haut du visage
			cv::putText(img_clone,noms, coin,FONT_HERSHEY_SIMPLEX,font[choix_resolution],Scalar(255,255,255),2,8,false);//ecire le nom
			if(result!=-1)
				cout<<"L'AI predit l'usager "<<noms<<endl;
			else
				cout<<"L'AI ne reconnait pas cet usager."<<endl;
		}
		else//contourage des photos et ajustement à la bonne grosseur
		{

			Rect myROI(faces[i].x,faces[i].y,faces[i].width,faces[i].height);
			img_clone=img_clone(myROI);
			resize(img_clone, img_resized, img_resized.size(), 0, 0);
			cvtColor(img_resized, img_resized,CV_BGR2GRAY);
		}
	}

	if(faces.size()>0){
		time_t t = time(0);   // get time now
		char name_picture[80];
		struct tm* now = localtime( & t );
		strftime (name_picture,80,"%Y-%m-%d-%R_%M_%S.png",now);
		if(mode==0)//sauvegarde des photos croppées et ajout du nom de la photo dans la base de données
		{
			string fileName= path+"/"+name_picture;
			cv::imwrite(fileName, img_resized);
			ofstream listeDePics;
			listeDePics.open("ListeDePhotos.csv",ios::app);
			listeDePics<<fileName<<";"<<choix<<"\n";
			listeDePics.close();
		}
		else//sauvegarde de l'image avec les visages encadrés et l'identification
		{
			cv::imwrite(name_picture, img_clone);
			return;
		}
	}
}

int select_resolution()
{
	int choix_resolution = 0;
	cout << "Veuillez choisir une resolution \n [1] 176 x 144 \n [2] 160 x 120 \n [3] 320 x 240 \n [4] 960 x 720 \n" << endl;
	cin >> choix_resolution;

	while (choix_resolution > 4 || choix_resolution < 1)
	{
		cout << "Inscrire un chiffre entre 1 et 4" << endl;
		cin >> choix_resolution;
	}
	return choix_resolution -= 1;// les indices de la struct FPS vont de 0 à 3
}


//La partie entraînement de la fonction suivante a été copiée de la source suivante et a ensuite été légèrement modifiée.
/***************************************************************************************
*
*    Titre: Face Recognition with OpenCV
*    Auteur(s): OpenCV documentation
*    Date de consultation: 2019/11/21
*    Disponibilité: https://docs.opencv.org/3.4/da/d60/tutorial_face_main.html
*
***************************************************************************************/

void menuAI(int& mode, int& choix, cv::Mat& img, int resolution)
{
	cout<<"Selectionnez le mode choisi en ecrivant 0 ou 1"<<endl;
	cout<<"0 : Mode apprentissage"<<endl;//ajout de photos
	cout<<"1 : Mode reconnaissance"<<endl;
	cout<<"2 : Mode training AI"<<endl;//entrainement de l'IA avec toutes les photos
	cin>> mode;
	while(mode<0 || mode>2)
	{
		cout<< "La valeur entree n'est pas valide. Entrez 0 ou 1."<<endl;
		cin>>mode;
	}
	if (mode==0)
	{
		ifstream listeDeNomIn;
		ofstream listeDeNomOut,listePhotos;
		listeDeNomIn.open("ListeDeNom.csv", ios::in);
		if(listeDeNomIn.fail())
			error("Impossible d'ouvrir le fichier ListeDeNom.csv en lecture");
		cout<<"Choisissez pour qui ajouter des photos en entrant le nombre correspondant."<<endl;
		vector<string> row;
		string line,word, nameOfDirectory, path;

		while(getline(listeDeNomIn,line))
		{
			row.push_back(line);
		}
		cout<<"0 : Ajouter une personne"<<endl;;
		for(int j=0;j<row.size();j++)
		{
			cout<<j+1<<" : "<<row[j]<<endl;
		}
		cin>>choix;
		while(choix<0 && choix>row.size())
		{
			cout<< "La valeur entree n'est pas valide."<<endl;
			cin>>choix;
		}
		listeDeNomIn.close();
		if(choix==0){
			cout<<"Quel est le nom de la personne que vous souhaitez ajouter?"<<endl;
			cin>>nameOfDirectory;
			path ="./" + nameOfDirectory;
			char pathname[path.size()+1];//folder ou deposer les photos
			strcpy(pathname, path.c_str());

			//on verifie si le fichier existe deja
			if(!mkdir(pathname,0755))
			{
				nameOfDirectory+="\n";
				listeDeNomOut.open("ListeDeNom.csv",ios::app);
				if(listeDeNomOut.fail())
					error("Impossible d'ouvrir le fichier ListeDeNom.csv en écriture");
				listeDeNomOut<<nameOfDirectory;
				listeDeNomOut.close();
			}
			else
				cout<<"Ce repertoire existe deja"<<endl;
			choix=row.size();
		}
		else
		{
			choix--;
			path="./"+row[choix];
			char pathname[path.size()+1];//folder ou deposer les photos
			strcpy(pathname,path.c_str());
		}
		detectAndDraw(img,mode,path,choix);
	}
	//mode 1 : reconnaissance
	else if(mode==1)
	{
		detectAndDraw(img,mode,"",-1,resolution);

	}

	//mode 2 : training AI
	else
	{

		vector<Mat> images;
		vector<int> labels;
		// Read in the data. This can fail if no valid
		// input filename is given.
		try {
			read_csv("ListeDePhotos.csv", images, labels);
		} catch (cv::Exception& e) {
			cerr << "Error opening file \"" << "ListeDePhotos.csv" << "\". Reason: " << e.msg << endl;
			// nothing more we can do
			exit(1);
		}
		// Quit if there are not enough images for this demo.
		if(images.size() < 1) {
			string error_message = "il faut au moins une image pour entraîner l'IA.";
			CV_Error(CV_StsError, error_message);

		}
		// Get the height from the first image. We'll need this
		// later in code to reshape the images to their original
		// size:
		int height = images[0].rows;
		// The following lines simply get the last images from
		// your dataset and remove it from the vector. This is
		// done, so that the training data (which we learn the
		// cv::FaceRecognizer on) and the test data we test
		// the model with, do not overlap.

		//Mat testSample = images[images.size() - 1];
		//int testLabel = labels[labels.size() - 1];
		//images.pop_back();
		//labels.pop_back();

		// The following lines create an Eigenfaces model for
		// face recognition and train it with the images and
		// labels read from the given CSV file.
		// This here is a full PCA, if you just want to keep
		// 10 principal components (read Eigenfaces), then call
		// the factory method like this:
		//
		//      cv::createEigenFaceRecognizer(10);
		//
		// If you want to create a FaceRecognizer with a
		// confidence threshold (e.g. 123.0), call it with:
		//
		//      cv::createEigenFaceRecognizer(10, 123.0);
		//
		// If you want to use _all_ Eigenfaces and have a threshold,
		// then call the method like this:
		//
		//      cv::createEigenFaceRecognizer(0, 123.0);
		//
		Ptr<FaceRecognizer> model = createEigenFaceRecognizer(0,TRESHOLD);
		model->train(images, labels);
		model->save("modele_eigenface.yml");
		cout<<"L'entrainement est terminé, vous pouvez reprendre une photo."<<endl;//ceci n'est pas un message de debuggage, il indique a l'usager qu'il peut reprendre une photo.
	}
	return ;

}


void client_webcam()
{

	/*****************************************************************************************************************
	 * 							Local variable declarations
	 *****************************************************************************************************************/
	FPS FPS_Logitech[4];
	FPS_Logitech[0] = { 176, 144, 0 };
	FPS_Logitech[1] = { 160, 120, 0 };
	FPS_Logitech[2] = { 320, 240, 0 };
	FPS_Logitech[3] = { 960, 720, 0 };
	uint32_t message_send = 0, message_rcv = 0;

	int sockfd, portno=4099, n_send, n_rcv, ptr=0, key=100, toutEstbeau=1, choix_resolution = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.7.2");//adresse ip prédéfinie pour le serveur
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);

	Mat  img;

	if (sockfd < 0)
		error("ERROR opening socket");


	if (connect(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	namedWindow("Client", CV_WINDOW_AUTOSIZE);// Create a window for display.

	//d'abord envoie message a l'usager pour choisir resolution
	choix_resolution = select_resolution();

	message_send |= ((choix_resolution) << 1) + 1;

	/*****************************************************************************************************************
	 * 							Send resolution to server 1st time
	 *****************************************************************************************************************/
	if ((n_send = send(sockfd, &message_send, 4, 0)) == -1)
	{
		error("Le message n'a pas été envoyé correctement.");
	}

	//on initialise le buffer a la plus grande taille possible
	uchar sockData[FPS_Logitech[3].resX * FPS_Logitech[3].resY * 3];

	//on dimensionne la Matrice img correctement selon le choix de l'usager
	int imgSize = change_img_res(img, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);

	pid_t pid = getpid();
	while (toutEstbeau == 1)
	{

		/*****************************************************************************************************************
		 * 							Receive message from server, allowing the transfer of images
		 *****************************************************************************************************************/
		if ((n_rcv = recv(sockfd, &message_rcv, 4, 0)) == -1)
			error("Pas recu le message du server\n");


		if (message_rcv != IDOWN)
		{

			/*****************************************************************************************************************
			 * 											Receive image from server
			 *****************************************************************************************************************/

			for (int i = 0; i < imgSize; i += n_rcv)
			{
				if ((n_rcv = recv(sockfd, sockData + i, imgSize - i, 0)) == -1)
				{
					error("recv failed");
				}
			}
			// Assign pixel value to img
			ptr = 0;
			for (int i = 0; i < img.rows; i++) {
				for (int j = 0; j < img.cols; j++) {
					img.at<cv::Vec3b>(i, j) = cv::Vec3b(sockData[ptr + 0], sockData[ptr + 1], sockData[ptr + 2]);
					ptr = ptr + 3;
				}
			}


			imshow("Client", img);

			/*****************************************************************************************************************
			 * 											Fork to SAVE
			 *****************************************************************************************************************/
			//detectAndDraw(img,0,"/export/tmp/4205_34/ele4205-19-34/build_Client/Olivier","/export/tmp/4205_34/ele4205-19-34/build_Client/Olivier/ListeDePhotos.csv");
			if (message_rcv == PUSHB)
				pid = fork();
			if (pid == -1)
			{
				exit(EXIT_FAILURE);
			}
			if (pid == 0)
			{
				int choix=-1, mode =-1;
				menuAI(mode,choix,img,choix_resolution);

				_exit(EXIT_SUCCESS);
			}
			else
			{

				key = waitKey(30);

				if (key == ESC)//traitement de la touche appuyée (ou non) pour envoyer  le message approprié au serveur.
				{
					message_send &= 0;
					toutEstbeau = 0;
				}
				else if (key == numPad1 || key == charPad1)
				{
					message_send &= 1;
					choix_resolution = 0;
					imgSize = change_img_res(img, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);
				}
				else if (key == numPad2 || key == charPad2)
				{
					message_send &= 1;
					message_send |= 2;
					choix_resolution = 1;
					imgSize = change_img_res(img, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);
				}
				else if (key == numPad3 || key == charPad3)
				{
					message_send &= 1;
					message_send |= 4;
					choix_resolution = 2;
					imgSize = change_img_res(img, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);
				}
				else if (key == numPad4 || key == charPad4)
				{
					message_send &= 1;
					message_send |= 6;
					choix_resolution = 3;
					imgSize = change_img_res(img, FPS_Logitech[choix_resolution].resY,FPS_Logitech[choix_resolution].resX);
				}
				else
				{
					message_send |= 1;
				}

				/*****************************************************************************************************************
				 * 							Send resolution to server
				 *****************************************************************************************************************/
				n_send = send(sockfd, &message_send, 4, 0);
			}
		}
	}
	cout << "On a ferme le socket" << endl;
	close(sockfd);
}