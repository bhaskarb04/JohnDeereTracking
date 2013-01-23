#include "RWVector.h"

void write_single_vector(const char* vec,int size,ofstream &os,int type)
{
	os.write((const char*)&size,sizeof(size));
	if(type==1)
		os.write(vec,size*sizeof(showcircle));
	else if(type==2)
		os.write(vec,size*sizeof(cv::Point));
}

void write_single_vector(vector<showcircle> myvec2,int size,ofstream &os)
{
	os.write((const char*)&size,sizeof(size));
	for(int i=0;i<size;i++)
	{
		showcircle sc=myvec2[i];
		os.write((const char*)&(sc.age),sizeof(sc.age));
		os.write((const char*)&(sc.p),sizeof(sc.p));
		os.write((const char*)&(sc.radius),sizeof(sc.radius));
		os.write((const char*)&(sc.visible),sizeof(sc.visible));
		os.write((const char*)&(sc.z),sizeof(sc.z));
		int myvecsize = myvec2[i].contour.size();
		if(myvecsize)
			write_single_vector((const char*)&(sc.contour[0]),myvecsize,os,2);
		else
			os.write((const char*)&myvecsize,sizeof(myvecsize));
	}
}


void save_vector(vector<vector<showcircle> > myvec, char *fname,int rows,int cols,double depth)
{
	ofstream os (fname,ios::binary);
	os.write((const char*)&rows,sizeof(rows));
	os.write((const char*)&cols,sizeof(cols));
	os.write((const char*)&depth,sizeof(depth));
	int size1=myvec.size();
	os.write((const char*)&size1,sizeof(size1));
	for(unsigned int i=0;i<size1;i++)
	{
		int myvecsize=myvec[i].size();
		if(myvecsize)
			write_single_vector(myvec[i],myvec[i].size(),os);
		else
			os.write((const char*)&myvecsize,sizeof(myvecsize));
	}
	os.close();
}

/*void save_contour(vector<vector<vector<cv::Point> > >myvec, char *fname)
{
	ofstream os (fname,ios::binary);
	
	int size1=myvec.size();
	os.write((const char*)&size1,sizeof(size1));
	for(unsigned int i=0;i<size1;i++)
	{
		int size2=myvec[i].size();
		os.write((const char*)&size2,sizeof(size2));
		for(unsigned int j=0;j<size2;j++)
		{
			int myvecsize = myvec[i][j].size();
			if(myvecsize)
				write_single_vector((const char*)&myvec[i][j][0],myvecsize,os,2);
			else
				os.write((const char*)&myvecsize,sizeof(myvecsize));
			
		}
	}
	os.close();
}
*/
vector<vector<showcircle> > read_vector(char *fname,int &rows,int&cols, double &depth)
{
	ifstream is (fname,ios::binary);
	int size;
	is.read((char*)&rows,sizeof(rows));
	is.read((char*)&cols,sizeof(cols));
	is.read((char*)&depth,sizeof(depth));
	is.read((char*)&size,sizeof(size));
	vector<vector<showcircle> > readvec(size);
	vector<showcircle> nothing;
	for(unsigned int i=0;i<size;i++)
	{
		int size2;
		is.read((char*)&size2,sizeof(size2));
		if(size2)
		{
			readvec[i].resize(size2);
			for(int j=0;j<size2;j++)
			{
				showcircle sc;
				is.read((char*)&(sc.age),sizeof(sc.age));
				is.read((char*)&sc.p,sizeof(sc.p));
				is.read((char*)&sc.radius,sizeof(sc.radius));
				is.read((char*)&sc.visible,sizeof(sc.visible));
				is.read((char*)&sc.z,sizeof(sc.z));
				int myvecsize;
				is.read((char*)&myvecsize,sizeof(myvecsize));
				if(myvecsize)
				{
					sc.contour.resize(myvecsize);
					is.read((char*)&sc.contour[0],sizeof(cv::Point)*myvecsize);
				}
				readvec[i][j]=sc;
			}
		}
	}
	return readvec;
}
/*
vector<vector<vector<cv::Point> > > read_contour(char *fname)
{
	ifstream is (fname,ios::binary);
	int size;
	is.read((char*)&size,sizeof(size));
	vector<vector<vector<cv::Point> > > readvec(size);
	vector<showcircle> nothing;
	for(unsigned int i=0;i<size;i++)
	{
		int size2;
		is.read((char*)&size2,sizeof(size2));
		if(size2)
		{
			readvec[i].resize(size2);
			for(unsigned int j=0;j<size2;j++)
			{
				int size3;
				is.read((char*)&size3,sizeof(size3));
				readvec[i][j].resize(size3);
				is.read((char*)&readvec[i][j][0], size3 * sizeof(cv::Point));
			}
			
		}
	}
	return readvec;
}
*/
cv::Rect find_boundary(vector<cv::Point>c)
{
	int xmin=c[0].x;
	int xmax=c[0].x;
	int ymin=c[0].y;
	int ymax=c[0].y;

	for(unsigned int i=0;i<c.size();i++)
	{
	
		xmin=(xmin>c[i].x)?c[i].x:xmin;
		xmax=(xmax<c[i].x)?c[i].x:xmax;
		ymin=(ymin>c[i].y)?c[i].y:ymin;
		ymax=(ymax<c[i].y)?c[i].y:ymax;
	
	}
	return(cvRect(xmin,ymin,xmax-xmin,ymax-ymin));
}

vector<cv::Point> inside_contour_list(cv::Mat img,cv::Rect r)
{	
	cv::Mat image=img(r);
	vector<cv::Point> points;
	for(unsigned int i=0;i<image.rows;i++)
	{
		for(unsigned int j=0;j<image.cols;j++)
		{
			if(image.at<uchar>(i,j))
				points.push_back(cvPoint(r.x+j,r.y+i));
		}
	}
	return points;
}

cv::Mat read_depth_file(string fname,int rows,int cols)
{
	fstream rawfile;
	rawfile.open(fname.c_str(),ios_base::binary | ios_base::in);
	cv::Mat depthmap(rows,cols,CV_32FC1);
	rawfile.read((char*)depthmap.data,rows*cols*sizeof(float));
	cv::transpose(depthmap,depthmap);
	float minval=100;
	float *data=(float*)depthmap.data;
	for(int i=0;i<rows*cols;i++,data++)
		minval=(minval>(*data))?(*data):minval;
	cv::subtract(depthmap,minval,depthmap);
	return depthmap;
}
vector<vector<vector<cv::Point3f> > > ThreeDContour(string depthpath,vector<vector<vector<cv::Point> > > list,int rows,int cols)
{
	vector<vector<vector<cv::Point3f> > > Dc;
	for(unsigned int frame=0;frame < list.size();frame++)
	{
		vector<vector<cv::Point3f> > Dframe;
		if(list[frame].size())
		{
			char num[5];
			sprintf(num,"%d",frame);
			string filename=depthpath+"\\"+string(num)+".raw";
			cv::Mat depthmap=read_depth_file(filename,rows,cols);
			cv::imshow("MyDepthMap",depthmap);
			cv::waitKey(10);
			for(unsigned int i=0;i<list[frame].size();i++)
			{
				vector<cv::Point3f> Dcontour;
				for(unsigned int j=0;j<list[frame][i].size();j++)
				{
					cv::Point3f p;
					p.x=list[frame][i][j].x;
					p.y=list[frame][i][j].y;
					p.z=depthmap.at<float>(list[frame][i][j].y,list[frame][i][j].x);
					Dcontour.push_back(p);
				}
				Dframe.push_back(Dcontour);
			}
		}
		Dc.push_back(Dframe);
	}
	return Dc;
}
vector<vector<vector<cv::Point3f> > > find_contours(string depthpath,vector<vector<showcircle> >tracks,int rows,int cols)
{
	vector<vector<vector<cv::Point> > >list;
	cv::Mat original_image=cv::Mat::zeros(rows,cols,CV_8UC1);
	for(unsigned int i=0;i<tracks.size();i++)
	{
		vector<vector<cv::Point> > frame;
		for(unsigned int j=0;j<tracks[i].size();j++)
		{
			if(!tracks[i][j].visible)
				continue;
			cv::Mat temp=original_image.clone();
			cv::fillConvexPoly(temp,tracks[i][j].contour,CV_RGB(255,255,255));
			frame.push_back(inside_contour_list(temp,find_boundary(tracks[i][j].contour)));
			//cv::imshow("image",temp);
			//cv::waitKey();
		}
		list.push_back(frame);
	}
	return (ThreeDContour(depthpath,list,rows,cols));
}