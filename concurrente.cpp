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
#define NUM_HILOS    4   
using namespace cv;
void inicializar();
void *thread_process(void *rt);

//Variables globales
Mat img, dst;
Mat kernel;
Point anchor;
double delta;
int ddepth;
pthread_t thread_id,thread_id1,thread_id2,thread_id3,thread_id4;
Mat top_left,top_right,bottom_left,bottom_right;
Mat imgs;


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
    }else if(strcmp("top_right",(char *)rt)==0){
        top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
        filter2D(top_right, top_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }else if(strcmp("bottom_left",(char *)rt)==0){
        bottom_left = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(0, img.cols / 2 - 1));
        filter2D(bottom_left, bottom_left, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }else{
        bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
        filter2D(bottom_right, bottom_right, ddepth , kernel,
        anchor, delta,BORDER_DEFAULT );
    }
    pthread_exit(0);
}
int main (int argc, char** argv){
    pid_t pid;
    int status;
    //cargando la imagen
    img=imread(argv[1]);
    
    //Si no se puede cargar la imagen el programa se detiene
    //sudo g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 concurrente.cpp -pthread -o concurrente -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

    if(!img.data){
        return -1;
    }
    imshow("imagen principal",img);
    
    // Initialize arguments for the filter
    inicializar();

    pid=fork();
    if(pid<0){
        perror("fork");
        exit(1);
    }else if(pid==0){ //Proceso hijo 
        //se crea 2 hilosde trabajo
        pthread_create(&thread_id1,NULL,thread_process,(void *)"top_left");
        pthread_create(&thread_id2,NULL,thread_process,(void *)"top_right");
        
        pthread_create(&thread_id3,NULL,thread_process,(void*)"bottom_left");
        pthread_create(&thread_id4,NULL,thread_process,(void *)"bottom_right");
        
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        pthread_join(thread_id3,NULL);
        pthread_join(thread_id4,NULL);

        hconcat(bottom_left,bottom_right,bottom_left);
        hconcat(top_left,top_right,top_left);
        vconcat(top_left,bottom_left,bottom_left); 

        imwrite("imagen_final.jpg",bottom_left);
    }else{
        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        bottom_left=imread("imagen_final.jpg");
        imshow("imagen final",bottom_left);
        waitKey(0);

        /* 
        pthread_create(&thread_id3,NULL,thread_process,(void*)"bottom_left");
        pthread_create(&thread_id4,NULL,thread_process,(void *)"bottom_right");
        
        pthread_join(thread_id3,NULL);
        pthread_join(thread_id4,NULL);
        hconcat(bottom_left,bottom_right,bottom_left);

        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        imgs=imread("imagen_final.jpg");
        vconcat(imgs,bottom_left,bottom_left);            
		*/
        //execv("/bin/rm ./output1.jpg ./output2.jpg",NULL);
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