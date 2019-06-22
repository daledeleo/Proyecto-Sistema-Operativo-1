#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

/*
para compilar:

sudo g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 filter_demo_SO.cpp -o ejemplo_filtro -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

para ejecutar:
./ejemplo_filtro lena.jpg

para medir el tiempo:
 
time ./ejemplo_filtro lena.jpg

*/

using namespace cv;

/** @function main */
int main ( int argc, char** argv )
{
  /// Declare variables
  Mat src, dst, dstx, dsty;
  Mat kernel;
  Point anchor;
  double delta;
  int ddepth;
  int kernel_size;

  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }
  
  /// Initialize arguments for the filter
  anchor = Point( -1, -1 );
  delta = 0;
  ddepth = -1;
  imshow( "imagen original", src ); //remover

  //Gaussian
  kernel=getGaussianKernel(15,3);
  filter2D(src, dst, ddepth , kernel, anchor, delta
  , BORDER_DEFAULT );
  bool isSuccess = imwrite("output_lena.jpg",dst); 

  if (isSuccess == true) //remover
	  imshow( "imagen filtrada", dst );

  waitKey( 0 ); //remover
  return 0;
}



