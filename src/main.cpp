#include "mainwindow.h"
#include <QtGui/QApplication>
#define TIMER_INTERVAL 200
GLint window_width=1000;
GLint window_height=600;
#define DISPLAY 1

void make_video(vector<cv::Mat>,vector<cv::Mat>,string,string);
void make_video(vector<cv::Mat>,vector<cv::Mat>,string,string,int loops);

 int main(int argc, char** argv) 
 {

	 QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();

//	make_video(track.get_orig_images(),osgV.osgVideo,vidfile,filewritepath,NO_LOOPS);
 }

 void make_video(vector<cv::Mat>lhs,vector<cv::Mat>rhs,string vidname,string path)
 {
	 if(lhs.size()==0 || rhs.size()==0 || (lhs.size() < rhs.size()))
		 return;
	 int bump=lhs.size() - rhs.size();
	 cv::Size imgsize=lhs[0].size();
	 cv::Mat vidimg(imgsize.height,2*imgsize.width,CV_8UC3);
	 cv::VideoWriter video(vidname, CV_FOURCC('D','I','V','X'), 30, vidimg.size(), true);
	 for(unsigned int i=0;i<rhs.size();i++)
	 {
		 string filename;
		 char num[5];
		 sprintf(num,"%d",i);
		 filename=path+string(num)+".png";
		 cv::Mat l=lhs[i+bump];
		 cv::Mat r=rhs[i];
		 cv::Mat lroi=vidimg(cv::Rect(0,0,imgsize.width,imgsize.height));
		 cv::Mat rroi=vidimg(cv::Rect(imgsize.width,0,imgsize.width,imgsize.height));
		 cv::resize(rhs[i],r,imgsize);
		 l.copyTo(lroi);
		 r.copyTo(rroi);;
		 cv::imshow("finalvideo",vidimg);
		 cv::waitKey(10);
		 video<<vidimg;
		 video<<vidimg;
		 video<<vidimg;
		 cv::imwrite(filename,vidimg);
	 }
	 video.release();
 }
 void make_video(vector<cv::Mat>lhs,vector<cv::Mat>rhs,string vidname,string path,int loops)
 {
	if(LOOP)
		return;
	 if(lhs.size()==0 || rhs.size()==0)
	 return;
	 cv::Size imgsize=lhs[0].size();
	 cv::Mat vidimg(imgsize.height,2*imgsize.width,CV_8UC3);
	 cv::VideoWriter video(vidname, CV_FOURCC('D','I','V','X'), 30, vidimg.size(), true);
	 for(unsigned int i=0;i<rhs.size();i++)
	 {
		 string filename;
		 char num[5];
		 sprintf(num,"%d",i);
		 filename=path+string(num)+".png";
		 cv::Mat l=lhs[i%lhs.size()];
		 cv::Mat r=rhs[i];
		 cv::Mat lroi=vidimg(cv::Rect(0,0,imgsize.width,imgsize.height));
		 cv::Mat rroi=vidimg(cv::Rect(imgsize.width,0,imgsize.width,imgsize.height));
		 cv::resize(rhs[i],r,imgsize);
		 l.copyTo(lroi);
		 r.copyTo(rroi);;
		 //cv::imshow("finalvideo",vidimg);
		 //cv::waitKey(10);
		 video<<vidimg;
		 video<<vidimg;
		 video<<vidimg;
		 cv::imwrite(filename,vidimg);
	 }
	 video.release();
 }