
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
//g++ threads.c -pthread
//g++ -o ejecutable holaMundo.cpp
//sudo g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 filter_demo_SO.cpp -o ejemplo_filtro -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

using namespace cv;
pid_t pid;
cv::Mat img;
//Mat img;
/*
void left(void *g){
    (Mat*)&g=img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
    It's better to use Mat::(Range rowRange, Range colRange) here:
}*/
int main(int agr,char **argv){
    //pthread_t thread_id1;
    img= imread(argv[1]);
    
    pid=fork();
    if(pid==0){
        cv::Mat top_left = img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
        cv::Mat top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
    }else{
        cv::Mat bottom_left = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(0, img.cols / 2 - 1));
        cv::Mat bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
    }
    cv::imshow("top_left", top_left);
    cv::imshow("top_right", top_right);
    cv::imshow("bottom_left", bottom_left);
    cv::imshow("bottom_right", bottom_right);

    cv::hconcat(bottom_left,bottom_right,bottom_left);
    cv::hconcat(top_left,top_right,top_left);
    cv::vconcat(top_left,bottom_left,bottom_left);
    cv::imshow("imagen final",bottom_left);
    cv::waitKey(0);
    return 1;
}