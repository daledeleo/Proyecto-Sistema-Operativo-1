#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* numeros de hilos y numeros de partes en la que se
va a dividir la imagen solo puede ser 4*/
using namespace cv;
void inicializar();
void *thread_process(void *rt);

//Variables globales
Mat img, dst;
Mat kernel;
Point anchor;
double delta;
int ddepth;
pthread_t thread_id1,thread_id2;
Mat top_left,top_right,bottom_left,bottom_right;


/*  Se procede a dividir la imagen en partes iguales para que cada hilo 
    trate a cada parte de la imagen
    __________   
    |    |    |
    |____|____|
    |    |    | 
    |____|____|
    */
void *thread_process(void *rt){
    if(strcmp("top_left",(char *)rt)==0){
        top_left = img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
        filter2D(top_left, top_left, ddepth , kernel,anchor, delta,BORDER_DEFAULT);
    }
    /*else if(strcmp("top_right",(char *)rt)==0){
        top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
        filter2D(top_right, top_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }*/else if(strcmp("bottom_left",(char *)rt)==0){
        bottom_left = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(0, img.cols / 2 - 1));
        filter2D(bottom_left, bottom_left, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }
    
    /*else{
        bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
        filter2D(bottom_right, bottom_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }*/
    pthread_exit(0);
}
int main (int argc, char** argv){
    pid_t pid;
    int status;
    //cargando la imagen
    img=imread(argv[1]);
    
    /*
    sudo g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 concurrente.cpp -pthread -o concurrente -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
     */
    
    //Si no se puede cargar la imagen el programa se detiene
    if(!img.data){
        return -1;
    }
    imshow("imagen principal",img);
    //pthread_create(&thread_id2,NULL,thread_process,NULL);
    // Initialize arguments for the filter
    inicializar();

    pid=fork();
    if(pid<0){
        perror("fork");
        exit(1);
    }else if(pid==0){ //Proceso hijo 
        //se crea 1 hilo de trabajo
        pthread_create(&thread_id1,NULL,thread_process,(void *)"bottom_left");
        bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
        //bottom_right = img(cv::Range(img.rows / 2, img.rows ), cv::Range(img.cols -1, img.cols/2 -1));
        filter2D(bottom_right, bottom_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
        
        pthread_join(thread_id1,NULL);

        hconcat(bottom_left,bottom_right,bottom_left);
        imwrite("imagen_finalbottom.jpg",bottom_left);
    }else{
        
        pthread_create(&thread_id1,NULL,thread_process,(void *)"top_left");
        //top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
        top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
        filter2D(top_right, top_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );

        pthread_join(thread_id1,NULL);
        hconcat(top_left,top_right,top_left);
        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        bottom_left=imread("imagen_finalbottom.jpg");
        vconcat(top_left,bottom_left,bottom_left);
        imshow("imagen final",bottom_left);
        //pthread_join(thread_id2,NULL);
        waitKey(0);
    }
    return 1;

}

void inicializar(){
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;
    //Gaussian
    kernel=getGaussianKernel(15,3);
}