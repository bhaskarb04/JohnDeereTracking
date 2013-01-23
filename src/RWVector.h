#ifndef RW_VEC
#define RW_VEC


#include <fstream>
#include <vector>
using namespace std;

#include "Tracker.h"
void save_vector(vector<vector<showcircle> > myvec, char *fname,int rows,int cols,double depth);
vector<vector<showcircle> > read_vector(char *fname,int &rows,int&cols,double &depth);
void save_contour(vector<vector<vector<cv::Point> > >myvec, char *fname);
vector<vector<vector<cv::Point> > > read_contour(char *fname);
vector<vector<vector<cv::Point3f> > > find_contours(string depthpath,vector<vector<showcircle> >tracks,int rows,int cols);

#endif