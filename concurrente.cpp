#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* numeros de hilos y numeros de partes en la que se
va a dividir la imagen solo puede ser 4*/
#define NUM_HILOS    4   
using namespace cv;
void inicializar();
void *f_top_leftandprocess(void *rt);
void *f_top_rightandprocess(void *rt);
void *f_bottom_leftandProcess(void *rt);
void *f_bottom_rightandprocess(void *rt);

//Variables globales
Mat img, dst;
Mat img2;
Mat kernel;
Point anchor;
double delta;
int ddepth;
pthread_t thread_id,thread_id1,thread_id2,thread_id3,thread_id4;
Mat top_left,top_right,bottom_left,bottom_right;
Mat imgs[NUM_HILOS];

/*  Se procede a dividir la imagen en partes iguales para que cada hilo 
    trate a cada parte de la imagen
    __________   
    |    |    |
    |____|____|
    |    |    | 
    |____|____|
    */
void *f_top_leftandprocess(void *rt){
    top_left = img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
    filter2D(top_left, top_left, ddepth , kernel,anchor, delta,BORDER_DEFAULT);
    pthread_exit(0);
}

void *f_top_rightandProcess(void *rt){
    top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
    filter2D(top_right, top_right, ddepth , kernel,
    anchor, delta,BORDER_DEFAULT );
    pthread_exit(0);
}

void *f_bottom_leftandProcess(void *rt){
    bottom_left = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(0, img.cols / 2 - 1));
    filter2D(bottom_left, bottom_left, ddepth , kernel,
    anchor, delta,BORDER_DEFAULT );
    pthread_exit(0);
}

void *f_bottom_rightandProcess(void *rt){
    bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
    filter2D(bottom_right, bottom_right, ddepth , kernel,
    anchor, delta,BORDER_DEFAULT );
    pthread_exit(0);
}
void *mostrar_img(void *hg){
    imshow("imagen principal",img2);
    pthread_testcancel();
}
int main (int argc, char** argv){
    pid_t pid;
    int status;
    img2=imread(argv[1]);
    //cargando la imagen
    img=imread(argv[1]);
    
    //Si no se puede cargar la imagen el programa se detiene
    if(!img.data){
        return -1;
    }
    

    // Initialize arguments for the filter
    inicializar();

    pid=fork();
    if(pid<0){
        perror("fork");
        exit(1);
    }else if(pid==0){ //Proceso hijo 
        
        //se crea 2 hilosde trabajo
        pthread_create(&thread_id1,NULL,f_top_leftandprocess,NULL);
        pthread_create(&thread_id2,NULL,f_top_rightandProcess,NULL);
        pthread_join(thread_id1,NULL);
        pthread_join(thread_id2,NULL);
        imwrite("output1.jpg",top_left); 
        imwrite("output2.jpg",top_right); 
        
    }else{
        pthread_create(&thread_id,NULL,mostrar_img,NULL);
        pthread_create(&thread_id3,NULL,f_bottom_leftandProcess,NULL);
        pthread_create(&thread_id4,NULL,f_bottom_rightandProcess,NULL);
        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        top_left=imread("./output1.jpg");
        top_right=imread("./output2.jpg");
        pthread_join(thread_id3,NULL);
        pthread_join(thread_id4,NULL);
        pid_t y=fork();
        if(y==0){
            hconcat(bottom_left,bottom_right,bottom_left);
            hconcat(top_left,top_right,top_left);
            vconcat(top_left,bottom_left,bottom_left);
            imshow("imagen final",bottom_left);
        }else{
            pthread_join(thread_id,NULL);
            waitpid(y, &status, WUNTRACED | WCONTINUED);
            waitKey(0);
        }
        exit(0);
        //execv("/bin/rm ./output1.jpg ./output2.jpg",NULL);
    }
        exit(0);

}

void inicializar(){
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;
    //Gaussian
    kernel=getGaussianKernel(15,3);
}