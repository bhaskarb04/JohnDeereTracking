#ifndef OSG_VIEW_JD
#define OSG_VIEW_JD

#include <stdlib.h>
//#include "glut.h"
//OSG headers
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Camera>
#include <osg/Point>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Image>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/CameraManipulator>
#include <osgUtil/SmoothingVisitor>
#include <osgViewer/Viewer>
//VTK headers
#include <vtkSmartPointer.h>
#include <vtkXMLDataSetWriter.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolygon.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkSphereSource.h>
#include <vtkSphere.h>
#include <vtkVertex.h>

//Tracking header
#include "Tracker.h"

#define TIMER_ 0.010
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600
#define LOOP true
#define FILE_PATH "H:\\JDTracking\\Data\\"
#define SCALE 26
#define WITH_MODEL true
#define WITH_MODEL_PLATE true
#define BG_ERROR 0
#define ZSCALE 1
#define ZTRANSLATE 0
#define WITH_HEAD true
#define NO_LOOPS 1
#define TRANS_STEP 500
#define TYPE_POLYGONS 1
#define TYPE_SPHERES 2

class osgView : public osg::Referenced
{
	int xmax,ymax,zmax,totalelements,maxsize,animationcount,animationend,totalpoints;
	double zscale,ztranslate;
	double xtrans_particle,ytrans_particle,ztrans_particle;
	double xtrans_object,ytrans_object,ztrans_object;
	double bgwall;
	int loopme;
	osg::ref_ptr<osg::Vec4Array> colorlist;
	vector<osg::ref_ptr<osg::Vec3Array> > tracks;
	osg::ref_ptr<osg::PositionAttitudeTransform> transform;
	osg::ref_ptr<osg::Geode> axes,flow;
	osg::ref_ptr<osg::PositionAttitudeTransform> tractor;
	osg::ref_ptr<osg::Transform> tractorplate;
	bool set_track,drawA;
	vector<vector<bool> >visible;
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Image> image;
	vector<vector<vector<osg::Vec3d> > >contourlist;
	vector<vector<osg::Vec3d>> motionvector;
	bool pause_flag,allow_once;
	osg::ref_ptr<osg::Group> root;
	
	//osg::ref_ptr<osg::ref_ptr<osg::Vec3Array> >tracks;

	osg::ref_ptr<osg::Geode> drawAxes();
	osg::ref_ptr<osg::Geode> drawTractorplate();
	osg::ref_ptr<osg::PositionAttitudeTransform> drawTractor();
	void drawFlowAnimation();
	void makeColorlist(vector<vector<showcircle> > tracks);
	void getvideo();
	void correct_for_age();
public:
	osgView();
	~osgView();
	void setmax(int,int,int,double);
	void set_tracks(vector<vector<showcircle> > t);
	void update(float time);
	void draw();
	void set_scene();
	void set_viewer();
	osg::ref_ptr<osg::Group> return_scene(){return root;}
	osgViewer::Viewer return_view(){return viewer;}
	void allowonce(){allow_once=true;}
	void pause_on(){pause_flag=true;}
	void pause_off(){pause_flag=false;}
	void reset_animationcount(){animationcount=1;}
	void animationcountplus1(){}
	void animationcountminus1(){animationcount=(animationcount-2<1?animationend-2:animationcount-1);}
	void end_animationcount(){animationcount=animationend-2;}
	void set_animationcount(int num){animationcount=num>=animationend?animationend-1:num;}
	vector<cv::Mat> osgVideo;
	void plusX();
	void minusX();
	void plusY();
	void minusY();
	void plusZ();
	void minusZ();
	void modelplusx(double);
	void modelminusx(double);
	void modelplusy(double);
	void modelminusy(double);
	void modelplusz(double);
	void modelminusz(double);
	void modelrotplusx();
	void modelrotminusx();
	void modelrotplusy();
	void modelrotminusy();
	void modelrotplusz();
	void modelrotminusz();
	void modelscale(double);
	void checktractorplate(bool);
	void save_model(string );
	void load_model(string );
	void remove_model();
	void export2vtk(string file_name ,int type=TYPE_SPHERES);
};

class ParticleCallback : public osg::NodeCallback 
{
public:
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::ref_ptr<osgView> ParticleData = 
         dynamic_cast<osgView*> (node->getUserData() );
      if(ParticleData)
      {
         ParticleData->update(TIMER_);
      }
      traverse(node, nv); 
   }
};

#endif