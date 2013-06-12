#include "Tracker.h"
//constructors
Tracker::Tracker()
{
	cout<<"Please enter the filename"<<endl;
	cin>>path_original;
	load_images(path_original,false);
}

Tracker::Tracker(string pname,bool video)
{
	path_original=pname;
	if(!video)
		load_images(path_original,false);
	else
		load_images_video(path_original,false);
}

//destructor
Tracker::~Tracker()
{
	list_images.clear();
	list_image_names.clear();
}
//function to load the images
void Tracker::load_images(string pname,bool show)
{
	DIR *dir;
	struct dirent *ent;
    dir = opendir (pname.c_str());
	if (dir != NULL) 
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			string s(ent->d_name);
			int x=s.find(".png");
			if(s.find(".png")!=string::npos)
				list_image_names.push_back(s);
		}
		closedir (dir);
	} 
	else 
	{
		/* could not open directory */
		perror ("Cant opend Directory");
		return;
	}
	for(unsigned int i=0;i<list_image_names.size();i++)
	{
		list_images.push_back(cv::imread(pname+"\\"+list_image_names[i]));
		list_images_org.push_back(list_images[i]);
		if(i==0)
			ref=list_images[i].clone();
		if(show)
		{
			cv::imshow("showme",list_images[i]);
			cv::waitKey(50);
		}
	}
	imgrows=list_images[0].rows;
	imgcols=list_images[0].cols;
}

void Tracker::load_images_video(string p, bool show)
{
	cv::VideoCapture video(p);
	if(!video.isOpened())
		throw MyException("Video Corrupt");
	cv::Mat frame;
	int firstframe=0;
	while(video.read(frame))
	{
		if(firstframe==1)
		{
			ref=frame.clone();
		}
		firstframe++;
		cv::Mat temp=frame.clone();
		list_images.push_back(temp);
		list_images_org.push_back(temp);
		if(show)
		{
			cv::imshow("Video Captured",frame);
			cv::waitKey(10);
		}
	}
	//cv::imshow("reference",ref);
	//cv::waitKey(0);
	imgrows=list_images[0].rows;
	imgcols=list_images[0].cols;

}

void Tracker::clean_image(bool show)
{
	cv::Mat strelem;
	strelem=cv::getStructuringElement(cv::MORPH_ELLIPSE,cvSize(4,4));
	list_images_cleaned.resize(list_images.size());
	for(unsigned int i=0; i < list_images.size();i++)
	{
		cv::Mat erode1,erode2,dilate1,dilate2;
		cv::Mat worefimg;
		cv::subtract(list_images[i],ref,worefimg);
		cv::erode(worefimg,erode1,strelem);
		cv::dilate(erode1,dilate2,strelem,cvPoint(-1,-1),2);
		cv::erode(dilate2,erode2,strelem);
		list_images[i]=erode2;
		//list_images[i].copyTo(list_images_cleaned[i]);
		if(show)
		{
			cv::imshow("showme",erode2);
			cv::waitKey(50);
		}
	}
}

void Tracker::track_particles(bool show)
{
	bgdepth=0;
	bgspots=0;
	cv::VideoWriter record("Contours.avi",CV_FOURCC('D','I','V','X'),30, list_images[0].size(), true);
	vector<vector<cv::Point> > contour;
	for(unsigned int i=1;i<list_images.size();i++)
	{
		cv::Mat img;
		if(list_images[i].channels()>1)
			cv::cvtColor(list_images[i],img,CV_RGB2GRAY);
		else
			img=list_images[i];
		cv::findContours(img,contour,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
		for(unsigned j=0;contour.size()>0 && j<contour.size();j++)
		{
			double area=cv::contourArea(contour[j]);
			if( area< img.rows*img.cols*0.001)
			{
				trackpoint tt;
				showcircle temp=tt.contourcentre(contour[j]);
				int xx=max(0,min(img.cols-1,temp.p.x));
				int yy=max(0,min(img.rows-1,temp.p.y));
				cv::Vec3b blah=list_images[i].at<cv::Vec3b>(yy,xx);
				bgdepth+=blah.val[0];
				bgspots++;
				contour.erase(contour.begin()+j);
				j--;
			}
		}
		contours.push_back(contour);
		cv::drawContours(list_images[i],contour,-1,CV_RGB(255,0,0),1);
		if(show)
		{
			cv::imshow("showme",list_images[i]);
			cv::waitKey(200);
			record<<list_images[i];
			record<<list_images[i];
			record<<list_images[i];
		}
		//list_images_contours.push_back(list_images[i]);
	}
	make_tracks(false,200);
}

void Tracker::optical_flow(bool show)
{
	cv::Mat previmg=list_images[0];
	vector<cv::Point2f>prevpts(100),nextpts(100);
	vector<unsigned char>status(100);
	vector<float>err(100);
	//cv::cvtColor(list_images[0],previmg,CV_RGB2GRAY);
	for(unsigned int i=1;i<list_images.size();i++)
	{
		cv::Mat grayimg=list_images[i];
		//cv::cvtColor(list_images[i],grayimg,CV_RGB2GRAY);
		cv::calcOpticalFlowPyrLK(previmg,grayimg,prevpts,nextpts,status,err);
		previmg=grayimg;
		if(show)
		{
			cv::Mat imgdraw=list_images[i-1];
			for(unsigned int j=0;j<prevpts.size();j++)
			{
				cv::line(imgdraw,prevpts[j],nextpts[j],CV_RGB(255,0,0));	
			}
			
			cv::imshow("showme",imgdraw);
			cv::waitKey(50);
		}
	}
}

void Tracker::make_tracks(bool show,int wait)
{
	cv::VideoWriter record("Particles.avi",CV_FOURCC('D','I','V','X'),30, list_images[0].size(), true);
	trackpoint tt;
	for(unsigned int i=0;i<contours.size();i++)
	{
		//cout<<i<<endl;
		if(contours[i].size()==0) 
		{
			vector<showcircle> b;
			tracks.push_back(b);
			continue;
		}
		cv::Mat showimage=tt.update(contours[i],list_images_org[i]);
		tracks.push_back(tt.centre_return());
		//all_contours.push_back(contours);
		record<<showimage;
		record<<showimage;
		record<<showimage;
		if(show)
		{
			cv::imshow("showme",showimage);
			cv::waitKey(wait);
		}
		
	}
}
trackpoint::trackpoint()
{

}
trackpoint::~trackpoint()
{

}
showcircle trackpoint::contourcentre(vector<cv::Point> c)
{
	showcircle showc;
	cv::Point2f midpoint=cvPoint(0,0);
	for(unsigned int i=0;i<c.size();i++)
	{
		midpoint.x+=c[i].x;
		midpoint.y+=c[i].y;
	}
	midpoint.x/=c.size();
	midpoint.y/=c.size();
	double dist=sqrt((c[0].x-midpoint.x)*(c[0].x-midpoint.x)+(c[0].y-midpoint.y)*(c[0].y-midpoint.y));
	for(unsigned int i=1;i<c.size();i++)
	{
		double newdist=sqrt((c[i].x-midpoint.x)*(c[i].x-midpoint.x)+(c[i].y-midpoint.y)*(c[i].y-midpoint.y));
		if(newdist > dist)
			dist=newdist;
	}
	showc.p=midpoint;
	showc.radius=dist;
	return(showc);
}
cv::Mat trackpoint::update(vector<vector<cv::Point> >contour,cv::Mat img)
{
	inputimg=img;
	//find nearest point from centre + motion
	vector<showcircle> tempcentres;
	for(unsigned int i=0;i<contour.size();i++)
	{
		tempcentres.push_back(contourcentre(contour[i]));
	}
	nearest(tempcentres,contour);
	for(unsigned int i=0;i<tempcentres.size();i++)
	{
		centres.push_back(tempcentres[i]);
		centres[centres.size()-1].contour.assign(contour[i].begin(),contour[i].end());
		motion.push_back(cvPoint(0,0));
		motionz.push_back(0.0);
		age.push_back(AGE_THRESH);
	}
	centres_p.clear();
	for(unsigned int i=0;i<centres.size();i++)
	{
		showcircle sc;
		sc.p=centres[i].p;
		sc.visible=false;
		sc.radius=centres[i].radius;
		cv::Point c(centres[i].p.x-motion[i].x,centres[i].p.y-motion[i].y);
		int xx=max(0,min(img.cols-1,c.x));
		int yy=max(0,min(img.rows-1,c.y));

		cv::Vec3b blah=img.at<cv::Vec3b>(yy,xx);
		sc.vec.val[0]=motion[i].x;sc.vec.val[1]=motion[i].y;sc.vec.val[2]=motionz[i];
		sc.z=(255-blah.val[0]);
		sc.age=age[i];
		sc.contour.assign(centres[i].contour.begin(),centres[i].contour.end());

		if(age[i]>AGE_THRESH - 2)
		{
			char num[5];
			cv::circle(img,c,3,CV_RGB(255,0,0),1);
			_itoa(i,num,10);
			//cv::putText(img,string(num),centres[i].p,CV_FONT_HERSHEY_SCRIPT_SIMPLEX,2,CV_RGB(0,0,255));
			sc.visible=true;
		}
		centres_p.push_back(sc);
	}
	return(img);
}

double dist(point3 p1,point3 p2)
{
	return ((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}

void trackpoint::nearest(vector<showcircle> &inputcentres, vector<vector<cv::Point> >&contours)
{
	for(unsigned int i=0;i<centres.size();i++)
	{
		int centre=-1;
		double maxdistance=DISTANCE_THRESH*DISTANCE_THRESH*DISTANCE_THRESH;
		
		for(unsigned int j=0;j<inputcentres.size();j++)
		{
			point3 nextpoint;
			nextpoint.x=centres[i].p.x+motion[i].x;
			nextpoint.y=centres[i].p.y+motion[i].y;
			nextpoint.z=centres[i].z+motionz[i];

			point3 inputpoint;
			inputpoint.x=inputcentres[j].p.x;
			inputpoint.y=inputcentres[j].p.y;
			inputpoint.z=inputcentres[j].z;
			double tdist=dist(nextpoint,inputpoint);
			if( tdist < DISTANCE_THRESH*DISTANCE_THRESH*DISTANCE_THRESH && tdist < maxdistance && age[i]>=0)
			{
				maxdistance=tdist;
				centre=j;
			}
		}
		
		if(centre>=0)
		{
			motion[i]=inputcentres[centre].p-centres[i].p;
			int xx=max(0,min(inputimg.cols-1,inputcentres[centre].p.x));
			int yy=max(0,min(inputimg.rows-1,inputcentres[centre].p.y));
			motionz[i]=inputimg.at<cv::Vec3b>(yy,xx).val[0] - motionz[i];
			centres[i]=inputcentres[centre];
			centres[i].contour.assign(contours[centre].begin(),contours[centre].end());
			inputcentres.erase(inputcentres.begin()+centre);
			contours.erase(contours.begin()+centre);
			age[i]=AGE_THRESH;

		}
		else
		{
			centres[i].p.x=centres[i].p.x+motion[i].x;
			centres[i].p.y=centres[i].p.y+motion[i].y;
		}
		age[i]--;
	}
}