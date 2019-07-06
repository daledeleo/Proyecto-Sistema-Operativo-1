
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

using namespace cv;
struct Memory{
    int status;
    Mat de;
};
int main(int argc, char** argv) {
    int shmid;
    struct Memory *shm;
    //Mat *shared_mat;
    /*
    shmid = shmget(2009, sizeof(struct Memory), 0666 | IPC_CREAT);
    shm = (struct Memory *)shmat(shmid, 0, 0);
    shmctl(shmid, IPC_RMID, NULL);
    */
    Mat g=(imread(argv[1]));
    imshow("principal",g);
    printf("llegoSize: %d\n",(g).cols);
    printf("tamanio Mat: %ld\n",sizeof(Mat));
    if(fork()==0){
        shmid = shmget(2009, sizeof(Memory), 0);
        if(shmid<0){
        perror("shmget child");
        return 1;
    }
        //shm = (struct Memory *)shmat(shmid, 0, 0);
        shm= (Memory *)shmat(shmid, 0, 0);
        if(shm==(Memory *)-1){
        perror("shm");
        return 1;
    }   
        for(int i=0;i<=10;i++){
            printf("%d\n",i);
            shm->status=shm->status+i;
        }
        
        shm->de=imread(argv[1]);
        
        printf("size1: %ld\n",sizeof((shm->de).cols));
        exit(1);
    }
    
    shmid = shmget(2009, sizeof(Memory), IPC_CREAT|0666);
    if(shmid<0){
        perror("shmget");
        return 1;

    }
    shm = (Memory *)shmat(shmid, 0, 0);
    if(shm==(Memory *)-1){
        perror("shm");
        return 1;
    }
    wait(NULL);
    printf("resultado de status: %d, FIN\n",shm->status);
    if((shm->de).data){
        printf("yes\n");
        printf("size2: %ld\n",sizeof((shm->de).cols));
        cv::imshow("final",(shm->de));
    }
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    waitKey( 0 ); //remover
    
    return 1;
    /* 
   int shmid;
   char *shm;

   if(fork() == 0) {
      shmid = shmget(2009, SHMSIZE, 0);
      shm = (char *)shmat(shmid, 0, 0);
      char *s = (char *) shm;
      *s = '\0';  /* Set first location to string terminator, for later append/
      int i;
      for(i=0; i<5; i++) {
         int n;  /* Variable to get the number into 
         printf("Enter number<%i>: ", i);
         scanf("%d", &n);
         sprintf(s, "%s%d", s, n);  /* Append number to string 
      }
      strcat(s, "\n");  /* Append newline 
      printf ("Child wrote <%s>\n",shm);
      shmdt(shm);
   }
   else {
      /* Variable s removed, it wasn't used/
      /* Removed first call to wait as it held up parent process 
      shmid = shmget(2009, SHMSIZE, 0666 | IPC_CREAT);
      shm = (char *)shmat(shmid, 0, 0);
      wait(NULL);
      printf ("Parent reads <%s>\n",shm) ;
      shmdt(shm);
      shmctl(shmid, IPC_RMID, NULL);
   }
   return 0;
   */
}

/*
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
}
void *t_pthread(void *);
void *t_pthread2(void *);
Mat src, dst, dstx, dsty;
    Mat kernel;
    Point anchor;
    double delta;
    int ddepth;
    int kernel_size;

int main(int agr,char **argv){

    //pthread_t thread_id1;
    /*
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
    
   /// Declare variables
    
    pthread_t thread_id,thread_id1;

    /// Load an image
    src = imread( argv[1] );

    if( !src.data )
    { return -1; }
    pthread_create(&thread_id,NULL,t_pthread2,NULL);
    /// Initialize arguments for the filter
    anchor = Point( -1, -1 );
    delta = 0;
    ddepth = -1;
    //Gaussian
    kernel=getGaussianKernel(15,3);
    pthread_create(&thread_id1,NULL,t_pthread,NULL);
    pthread_join(thread_id1,NULL);
    pthread_join(thread_id,NULL);
    waitKey( 0 ); //remover
    return 0;
}
void *t_pthread(void *r){
        filter2D(src, dst, ddepth , kernel, anchor, delta
        , BORDER_DEFAULT );
        bool isSuccess = imwrite("output_lena.jpg",dst); 

        if (isSuccess == true){ //remover
            imshow( "imagen filtrada", dst );
        }
        pthread_exit(0);
}
void *t_pthread2(void *r){
    imshow( "imagen original", src ); //remover
    pthread_exit(0);
}
*/