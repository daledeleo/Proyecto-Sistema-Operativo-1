
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
//g++ threads.c -pthread
//g++ -o ejecutable holaMundo.cpp
//sudo g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 filter_demo_SO.cpp -o ejemplo_filtro -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

using namespace cv;

cv::Mat img;
cv::Mat top_left,top_right,bottom_left,bottom_right;
//Mat img;
/*
void left(void *g){
    (Mat*)&g=img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
    It's better to use Mat::(Range rowRange, Range colRange) here:
}*/
int main(int agr,char **argv){
    //pthread_t thread_id1;
    int status;
    int fd[2];
    pipe(fd);
    img= imread(argv[1]);
    pid_t pid;

    pid=fork();
    
    if(pid==0){
        //close(fd[0]);
        top_left = img(cv::Range(0, img.rows / 2 - 1), cv::Range(0, img.cols / 2 - 1));
        top_right= img(cv::Range(0, img.rows / 2 - 1), cv::Range(img.cols / 2, img.cols - 1));
        imwrite("output1.jpg",top_left); 
        imwrite("output2.jpg",top_right); 
        //close(fd[1]);
        printf("pid hijo: %ld\n",(long)getpid());
        
        exit(0);
    }else{
        top_left=imread("./output1.jpg");
        top_right=imread("./output2.jpg");
        bottom_left = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(0, img.cols / 2 - 1));
        bottom_right = img(cv::Range(img.rows / 2, img.rows - 1), cv::Range(img.cols / 2, img.cols - 1));
        printf("pid padre: %ld espera por el hijo %ld\n",
        (long)getpid(),(long)pid);
        waitpid(pid,&status,WUNTRACED|WCONTINUED);
        //close(fd[1]);
        cv::imshow("top_left", top_left);
        cv::imshow("top_right", top_right);
        //close(fd[0]);
        cv::imshow("bottom_left", bottom_left);
        cv::imshow("bottom_right", bottom_right);

        cv::hconcat(bottom_left,bottom_right,bottom_left);
        cv::hconcat(top_left,top_right,top_left);
        cv::vconcat(top_left,bottom_left,bottom_left);
        cv::imshow("imagen final",bottom_left);
        
        argv[0]=(char *)"/bin/rm ./output2.jpg ./output1.jpg";
        printf(argv[0]);
        cv::waitKey(0);
        execvp(argv[0],argv);
        
        
    }
    return 1;
}