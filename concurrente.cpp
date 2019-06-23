#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <pthread.h>

/* numeros de hilos y numeros de partes en la que se
va a dividir la imagen*/
#define NUM_HILOS    4   
using namespace cv;

Mat src, dst;
Mat kernel;
Point anchor;
double delta;
int ddepth;
pthread_t thread_id;
pthread_t threads_id[NUM_HILOS];

void *thread_funPoint(void *arg){
    // Initialize arguments for the filter
    Mat dst;
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;
    //Gaussian
    kernel=getGaussianKernel(15,3);
    filter2D((src, dst, ddepth , kernel,
    anchor, delta,BORDER_DEFAULT );
    return dst;
}
/*  Se procede a dividir la imagen en partes iguales para que cada hilo 
    trate a cada parte de la imagen
    ____________________
    |    |    |    |    |
    |    |    |    |    |
    |    |    |    |    |
    |    |    |    |    | 
    |____|____|____|____|
    */
void f_dividirImg(Mat img,Mat imgs[]){
    Mat img_cut;
    for(int i=0;i<NUM_HILOS;i++){
        Rect myROI();
        // Procedimiento para dividir la imagen
        
        imgs[i]=img_cut;
    }
}

int main (int argc, char** argv){
    
    //cargando la imagen
    src=imread(argv);

    //Si no se puede cargar la imagen el programa se detiene
    if(!src.empty()){
        return -1;
    }
    
    Mat imgs[SIZE];
    f_dividirImg(src,imgs);

    waitKey(0);
    return 1;
}