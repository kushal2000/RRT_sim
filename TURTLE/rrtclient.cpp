#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace cv;
using namespace std;

#define radius 80
float arr[100][2];int n=0;
typedef struct
{
  int x;
  int y;
}coordi;

struct leaf {
    leaf *parent;
    coordi position;
};

leaf start_leaf;
leaf end_leaf;
Mat img;
leaf* leafs[5000];
int totleafs = 0;
int reached = 0;
int step_size = 10;
int iter = 0;

int isValid(int x , int y)
{
  if(x>=0&&x<img.rows&&y>=0&&y<img.cols)
    return 1;
  return 0;
}

void init()
{
    start_leaf.position.x = 50;
    start_leaf.position.y = 50;
    start_leaf.parent = NULL;
    for(int i=start_leaf.position.x - 10; i < start_leaf.position.x + 10; i++)
    {
      for(int j=start_leaf.position.y - 10; j < start_leaf.position.y + 10; j++)
      {
        img.at<Vec3b>(i, j)[0] = 255;
        img.at<Vec3b>(i, j)[1] = 255;
        img.at<Vec3b>(i, j)[2] = 0;
      }
    }
    leafs[totleafs++] = &start_leaf;
    end_leaf.position.x = 550;
    end_leaf.position.y = 550;
    for(int i=end_leaf.position.x - 10; i < end_leaf.position.x + 10; i++)
    {
      for(int j=end_leaf.position.y - 10; j < end_leaf.position.y + 10; j++)
      {
        img.at<Vec3b>(i, j)[0] = 255;
        img.at<Vec3b>(i, j)[1] = 0;
        img.at<Vec3b>(i, j)[2] = 255;
      }
    }
    srand(time(NULL));
}
float leaf_dist(coordi p, coordi q)
{
  coordi v;
  v.x = p.x - q.x;
  v.y = p.y - q.y;
  return sqrt(pow(v.x, 2) + pow(v.y, 2));
}
int near_leaf(leaf rleaf)
{
  float min_dist = 999.0, dist= leaf_dist(start_leaf.position, rleaf.position);
  int nleaf = 0, i = 0;
  for(i=0; i<totleafs; i++)
  {
    dist = leaf_dist(leafs[i]->position, rleaf.position);
    if(dist<min_dist)
    {
      min_dist = dist;
      nleaf = i;
    }
  }
  return nleaf;
}
coordi stepping(coordi nleaf, coordi rleaf)
{
  coordi interm, step;
  float magn = 0.0, x = 0.0, y = 0.0;
  interm.x = rleaf.x - nleaf.x;
  interm.y = rleaf.y - nleaf.y;
  magn = sqrt((interm.x)*(interm.x) + (interm.y)*(interm.y));
  x = (float)(interm.x / magn);
  y = (float)(interm.y / magn);
  step.x = (int)(nleaf.x + step_size*x);
  step.y = (int)(nleaf.y + step_size*y);
  return step;
}
int validity(coordi p, coordi q)
{
  coordi largex, smallx,smally,largey;
  int x = 0, y = 0, i = 0 , j=0;
  double slope;
  if(q.x<p.x)
  {
    smallx = q;
    largex = p;
  }
  else
  {
    smallx = p;
    largex = q;
  }
  if(q.y<p.y)
  {
    smally = q;
    largey = p;
  }
  else
  {
    smally = p;
    largey = q;
  }
  if(largex.x==smallx.x)
  {
    x=largex.x;
    for(y=smally.y;y<=largey.y;y++)
    {
      if(!isValid(x,y)) return 0;
      for(i=x-5;i<x+6;i++)
      {
        for(j=y-5;j<y+6;j++)
        {
          if(isValid(i,j))
          {
            if((img.at<Vec3b>(i, j)[0] == 255) && (img.at<Vec3b>(i, j)[1] == 255) && (img.at<Vec3b>(i, j)[2] == 255))
              return 0;
          } 
        }  
      }
    }
  } 
  else
  {
    slope = (double)(largex.y - smallx.y)/(largex.x - smallx.x);
    if(slope<1)
    {
      for(x=smallx.x;x<=largex.x; x++)
      {
        y = (int)((x-smallx.x)*(slope) + smallx.y);
        if(!isValid(x,y)) return 0;
        for(i=x-5;i<x+6;i++)
        {
          for(j=y-5;j<y+6;j++)
          {
            if(isValid(i,j))
            {
              if((img.at<Vec3b>(i, j)[0] == 255) && (img.at<Vec3b>(i, j)[1] == 255) && (img.at<Vec3b>(i, j)[2] == 255))
                return 0;
            } 
          }  
        }
      } 
    }
    else
    {
      slope = (double)(largey.x - smally.x)/(largey.y - smally.y);
      for(y=smally.y;y<=largey.y; y++)
      {
        x = (int)((y-smally.y)*slope + smally.x);
        if(!isValid(x,y)) return 0;
        for(i=x-5;i<x+6;i++)
        {
          for(j=y-5;j<y+6;j++)
          {
            if(isValid(i,j))
            {
              if((img.at<Vec3b>(i, j)[0] == 255) && (img.at<Vec3b>(i, j)[1] == 255) && (img.at<Vec3b>(i, j)[2] == 255))
                return 0;
            } 
          }  
        }
      } 
    }
  }  
  return 1;
}
void draw_path()
{
  leaf current, prev;
  current = end_leaf;
  prev = *(end_leaf.parent);
  while(1)
  {
    line(img, Point(prev.position.y, prev.position.x), Point(current.position.y, current.position.x), Scalar(0, 255, 255), 2, 8);
    arr[n][1]=600-current.position.x;
    arr[n++][0]=current.position.y;
    if(prev.parent == NULL)
    {
      arr[n][1]=600-prev.position.x;
      arr[n++][0]=prev.position.y;
      break;
    }
    prev = *(prev.parent);
    current = *(current.parent);
  }
}

int is_node(int i,int j)
{
  for (int k = 0; k < totleafs; ++k)
  {
    if (leafs[k]->position.x==i&&leafs[k]->position.y==j)
    {
      return k;
    }
  }
  return 0;
}

int path_dist(leaf* stepleaf)
{
  float path=0;
  while(1)
  {
    if (stepleaf->parent==NULL)
    {
      break;
    }
    else
    {
      path+=leaf_dist(stepleaf->position,stepleaf->parent->position);
      stepleaf=stepleaf->parent;
    }
  }
  return path;
}

void rrt()
{

    int index = 0,k;
    leaf* rleaf = new leaf;
    leaf* stepleaf = new leaf;
    (rleaf->position).x = rand() % 600 + 1;
    (rleaf->position).y = rand() % 600 + 1;
    index = near_leaf(*rleaf);
    if((leaf_dist(rleaf->position, leafs[index]->position)) < step_size)
      return;
    else
      stepleaf->position = stepping(leafs[index]->position, rleaf->position);
    if(validity(leafs[index]->position, stepleaf->position))
    {
      leafs[totleafs++] = stepleaf;
      stepleaf->parent = leafs[index];
      line(img, Point((stepleaf->position).y, (stepleaf->position).x), Point(leafs[index]->position.y, leafs[index]->position.x), Scalar(255, 0, 0), 2, 8);

      for(int i=stepleaf->position.x - 2; i < stepleaf->position.x + 2; i++)
      {
        for(int j=stepleaf->position.y - 2; j < stepleaf->position.y + 2; j++)
        {
          if(!isValid(i,j))
            continue;
          img.at<Vec3b>(i, j)[0] = 0;
          img.at<Vec3b>(i, j)[1] = 255;
          img.at<Vec3b>(i, j)[2] = 0;
        }
      }

      for (int i = stepleaf->position.x-radius; i < stepleaf->position.x+radius; ++i)
      {
        for (int j = stepleaf->position.y-radius; j < stepleaf->position.y+radius; ++j)
        {
          if (k=is_node(i,j))
            {
              if(validity(stepleaf->position,leafs[k]->position))              
              {
                if(path_dist(stepleaf)>leaf_dist(stepleaf->position,leafs[k]->position)+path_dist(leafs[k]))
                  stepleaf->parent=leafs[k];
              }
            } 
        }
      }

      for (int i = stepleaf->position.x-radius; i < stepleaf->position.x+radius; ++i)
      {
        for (int j = stepleaf->position.y-radius; j < stepleaf->position.y+radius; ++j)
        {
          if (k=is_node(i,j))
          {
            if(validity(stepleaf->position,leafs[k]->position)) 
            {
              if(path_dist(stepleaf)+leaf_dist(stepleaf->position,leafs[k]->position) < path_dist(leafs[k]))
                leafs[k]->parent=stepleaf;
            }
          } 
        }
      }

      if((validity(stepleaf->position, end_leaf.position)) &&(leaf_dist(stepleaf->position,end_leaf.position)< 3*step_size))
      {
        reached = 1;
        leafs[totleafs++] = &end_leaf;
        end_leaf.parent = stepleaf;
        for (int i = end_leaf.position.x-radius*2; i < end_leaf.position.x+radius*2; ++i)
        {
          for (int j = end_leaf.position.y-radius*2; j < end_leaf.position.y+radius*2; ++j)
          {
            if (k=is_node(i,j))
            {
              if( path_dist(&end_leaf)>leaf_dist(end_leaf.position,leafs[k]->position)+path_dist(leafs[k]))
              {
                if(validity(end_leaf.position,leafs[k]->position)&&validity(end_leaf.position,leafs[k]->position))
                  end_leaf.parent=leafs[k];
              }
            } 
          }
        }
      draw_path();
      }
    }
    iter++;
}

int main()
{
    // Create a socket
  int sock = socket(AF_INET , SOCK_STREAM, 0);
  if(sock==-1)
     return 1;
    
  // Create a hint structure for the server we are connecting with
  int port; 
  cout << "Enter port no: ";
  cin >> port;
  string IPadd = "127.0.0.1";
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port);
  inet_pton(AF_INET , IPadd.c_str() , &hint.sin_addr);
    
  // Connect to the socket on the server
  int conn = connect(sock, (sockaddr*)&hint , sizeof(hint));

  Mat img1 = imread("rrt.png", 0);
  Mat img2(img1.rows,img1.cols,CV_8UC3,Scalar(0,0,0));
  for(int i=0;i<img1.rows;i++)
  {
    for(int j=0;j<img1.cols;j++)
    {
      if(img1.at<uchar>(i,j)>150)
      {  
        img2.at<Vec3b>(i,j)[0]=255;
        img2.at<Vec3b>(i,j)[1]=255;
        img2.at<Vec3b>(i,j)[2]=255;
      } 
    }
  }
  img=img2;
  init();
  namedWindow( "window", WINDOW_AUTOSIZE );
  while((reached == 0))
  {
    rrt();
    imshow("window", img);
    waitKey(1);
  }  
  if(conn == -1)
    return 1;
    
  // Inf Loop
  char buff[4096];
  // Wait for response from other client
  memset( buff , 0 , 4096);
  int bytesrecv2 = recv(sock , buff , 4096 , 0);
  int delay = 0;
  // Display Response
  cout << "<Server> " << string(buff , bytesrecv2) << "\n";  
  char data[10];
  cout << "<You> " << n <<endl;
          
  // Send reply to server
  gcvt(float(n),4,data);
  int sending = send(sock , string(data).c_str() , string(data).size() + 1 ,0 );
  while(delay++<100000);
  delay=0;
  for(int i=n-1;i>=0;i--)
  {
    for(int j=0;j<2;j++)
    {
      cout << "<You> " << arr[i][j] <<" ";
          
      // Send reply to server
      gcvt(arr[i][j],4,data);
      int sending = send(sock , string(data).c_str() , string(data).size() + 1 ,0 );
          
      // Check if failed
      if(sending == -1)
      {
          cout << "Could not be send! Try again please!\n";
          continue;
      } 
      while(delay++<100000);
      delay=0;
    }
     cout << endl;      
  }
  imshow("window", img);
  waitKey();
  close(sock); 

  return 0;
}

