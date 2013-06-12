#include "OSGView.h"

osgView::osgView()
{
	set_track=false;
	drawA=false;
	zscale=zscale=ZSCALE;
	ztranslate=ZTRANSLATE;
	pause_flag=false;
	allow_once=false;
	loopme=NO_LOOPS;
	xtrans_particle=ytrans_particle=ztrans_particle=0;
}

osg::ref_ptr<osg::Geode> osgView::drawAxes()
{
	osg::ref_ptr<osg::Geode> return_axes=new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
	(*vertices)[0].set( 0.0f, 0.0f, 0.0f); //origin
	(*vertices)[1].set( -100.0f*SCALE, 0.0f, 0.0f); //X
	(*vertices)[2].set( 0.0f, 0.0f, 0.0f); //origin
	(*vertices)[3].set( 0.0f, 100.0f*SCALE, 0.0f); //Y
	(*vertices)[4].set( 0.0f, 0.0f, 0.0f); //origin
	(*vertices)[5].set( 0.0f, 0.0f, 100.0f*SCALE); //Z

	osg::ref_ptr<osg::DrawArrays>indices =new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6);
	
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(6);
	(*colors)[0].set(1.0,0.0,0.0,0.0);
	(*colors)[1].set(1.0,0.0,0.0,0.0);
	(*colors)[2].set(0.0,1.0,0.0,0.0);
	(*colors)[3].set(0.0,1.0,0.0,0.0);
	(*colors)[4].set(0.0,0.0,1.0,0.0);
	(*colors)[5].set(0.0,0.0,1.0,0.0);
	
	geom->setVertexArray( vertices.get() );

	geom->setColorArray(colors.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	
	geom->addPrimitiveSet( indices.get() );
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	
	return_axes->addDrawable(geom);
	return(return_axes.release());
}

osg::ref_ptr<osg::Geode> osgView::drawTractorplate()
{
	osg::ref_ptr<osg::Box> tractor= new osg::Box(osg::Vec3d(-xmax/2,ymax/2,bgwall),xmax,ymax,10);
	osg::ref_ptr<osg::ShapeDrawable> tractorDrawable = new osg::ShapeDrawable(tractor);
	tractorDrawable->setColor(osg::Vec4(0,1,0,0));
	osg::ref_ptr<osg::Geode> tractorGeode = new osg::Geode();
	tractorGeode->addDrawable(tractorDrawable);
	//osg::Transform *savetp=new osg::Transform;
	//savetp->addChild(tractorGeode);
	//osgDB::writeNodeFile(*savetp,"ReferencePlane.osg");
	return(tractorGeode.release());
}

osg::ref_ptr<osg::PositionAttitudeTransform> osgView::drawTractor()
{
	string s;
	if(!WITH_HEAD)
		s=string(FILE_PATH) + "Combinewithouthead.ive";
	else
		s=string(FILE_PATH) + "Combinewithouthead.ive";
	osg::ref_ptr<osg::PositionAttitudeTransform> mytransform = new osg::PositionAttitudeTransform();
	mytransform->addChild(osgDB::readNodeFile(s)); 
	/*mytransform->setAttitude(osg::Quat(osg::DegreesToRadians(90.0f),osg::Vec3d(1.0,0.0,0.0),
									   osg::DegreesToRadians(90.0f),osg::Vec3d(0.0,1.0,0.0),
									   osg::DegreesToRadians(0.0f),osg::Vec3d(0.0,0.0,1.0)));
	mytransform->setPosition(osg::Vec3d(xtrans_object,ytrans_object,ztrans_object));*/
	
	return(mytransform);
}
void osgView::draw()
{
	//drawing declarations
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Geode> axes,flow;	

	//call drawing methods
	axes=drawAxes();
	//if(set_track)
		//flow=drawFlow();

	//link to root
	root->addChild(axes);
	//if(set_track)
		//root->addChild(flow);
	root->addChild(drawTractor());
	//set to viewer and draw
	viewer.home();
	viewer.setSceneData( root );
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.run();
}

void osgView::update(float time)
{
	if(pause_flag && !allow_once)
		return;
	allow_once=false;
	osg::ref_ptr<osg::Geode> new_flow=new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(totalelements);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(totalelements);
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	stateset->setAttribute(new osg::Point(3.0f),osg::StateAttribute::ON);

	int count=0;
	if(transform->getNumChildren() > 1)
		transform->removeChild(1,transform->getNumChildren()-1);
	for(unsigned int i=0;i<animationcount;i++)
	{
		if(tracks[i]->getNumElements())
		{
			for(unsigned int j=0;j<tracks[i]->getNumElements();j++)
			{
				if(!visible[i][j])
					continue;
				(*vertices)[count]=(*tracks[i])[j];
				(*vertices)[count].set(-(*vertices)[count].x()*SCALE+xtrans_particle,
					(*vertices)[count].y()*SCALE+ytrans_particle,
					(*vertices)[count].z()*SCALE*zscale+ztranslate+ztrans_particle);
				(*colors)[count]=(*colorlist)[j];
				count++;
				if(i == animationcount - 1)
				{	
					osg::ref_ptr<osg::Sphere> cogs = new osg::Sphere((*vertices)[count-1],5.0*SCALE);
					osg::ref_ptr<osg::ShapeDrawable> cogDrawable = new osg::ShapeDrawable(cogs);
					osg::ref_ptr<osg::Geode> cogGeode = new osg::Geode();
					cogGeode->addDrawable(cogDrawable);
					transform->addChild(cogGeode);
					osg::ref_ptr<osg::Geode> cont_flow=new osg::Geode;
					osg::ref_ptr<osg::Geometry> contgeom = new osg::Geometry;
					osg::ref_ptr<osg::Vec3Array> contvertices = new osg::Vec3Array(contourlist[i][j].size());
					osg::ref_ptr<osg::Vec4Array> contcolor= new osg::Vec4Array;
					contcolor->push_back((*colorlist)[j]);
					for(unsigned int k=0;k<contourlist[i][j].size();k++)
					{
						(*contvertices)[k]=contourlist[i][j][k];
						(*contvertices)[k].set(-(*contvertices)[k].x()*SCALE+xtrans_particle,
							(*contvertices)[k].y()*SCALE+ytrans_particle,
							(*contvertices)[k].z()*SCALE*zscale+ztranslate+ztrans_particle);
					}
					osg::ref_ptr<osg::DrawArrays>contindices =new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,contourlist[i][j].size());
					contgeom->setVertexArray( contvertices.get() );
					contgeom->setColorArray(contcolor);
					contgeom->setColorBinding(osg::Geometry::BIND_OVERALL);
					contgeom->addPrimitiveSet(contindices.get());
					
					//stateset->setAttribute(new osg::point( 3.0f ),osg::StateAttribute::ON);
					contgeom->setStateSet(stateset);
					cont_flow->addDrawable(contgeom);
					transform->addChild(cont_flow);
				}
			}

		}
	}
	osg::ref_ptr<osg::DrawArrays>indices =new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,totalelements);
	geom->setVertexArray( vertices.get() );
	geom->setColorArray(colors.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet( indices.get() );
	
	//stateset->setAttribute(new osg::point( 3.0f ),osg::StateAttribute::ON);
	geom->setStateSet(stateset);
	new_flow->addDrawable(geom);
	transform->setChild(0,new_flow);
	animationcount++;
	if(animationcount == animationend && LOOP)
	{
		animationcount=1;
	}
	if(animationcount == animationend && !LOOP)
	{
		if(--loopme)
			animationcount=1;
		else
			viewer.setDone(true);
	}
}

void osgView::set_scene()
{
	osg::Camera *cam= new osg::Camera();
	root = new osg::Group();
	tractor = new osg::PositionAttitudeTransform();
	tractorplate = new osg::Transform();
	xtrans_object=-xmax/2;
	ytrans_object=ymax/2;
	ztrans_object=-10000;
	//Lighting
	osg::Light *light=new osg::Light();
	light->setAmbient(osg::Vec4d(0.5,0.5,0.5,0.0));
	//light->setConstantAttenuation(0.5);
	light->setDiffuse(osg::Vec4d(1.0, 1.0, 1.0, 0.0));
	light->setPosition(osg::Vec4d(-10000,-5000,0,1.0));
	osg::LightSource * lightsource = new osg::LightSource();
	lightsource->setLight(light);

	//call drawing methods
	axes=drawAxes();
	if(WITH_MODEL_PLATE)
	tractorplate->addChild(drawTractorplate());
	//if(WITH_MODEL);
		//tractor->addChild(drawTractor());
	if(set_track)
		drawFlowAnimation();

	//link to root
	root->addChild(axes);
	root->addChild(lightsource);
	if(WITH_MODEL_PLATE)
		root->addChild(tractorplate);
	if(WITH_MODEL)
		root->addChild(tractor);
	if(set_track)
	{
		root->addChild(transform->asPositionAttitudeTransform());
	}
}
void osgView::plusX()
{
	xtrans_particle+=TRANS_STEP;
}
void osgView::minusX()
{
	xtrans_particle-=TRANS_STEP;
}
void osgView::plusY()
{
	ytrans_particle+=TRANS_STEP;
}
void osgView::minusY()
{
	ytrans_particle-=TRANS_STEP;
}
void osgView::plusZ()
{
	ztrans_particle+=TRANS_STEP;
}
void osgView::minusZ()
{
	ztrans_particle-=TRANS_STEP;
}
void osgView::modelplusx(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x()+step,vec.y(),vec.z());
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelminusx(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x()-step,vec.y(),vec.z());
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelplusy(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x(),vec.y()+step,vec.z());
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelminusy(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x(),vec.y()-step,vec.z());
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelplusz(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x(),vec.y(),vec.z()+step);
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelminusz(double step)
{
	osg::Vec3d vec=tractor->asPositionAttitudeTransform()->getPosition();
	vec.set(vec.x(),vec.y(),vec.z()-step);
	tractor->asPositionAttitudeTransform()->setPosition(vec);
}
void osgView::modelrotplusx()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(45.0),osg::Vec3d(1,0,0));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelrotminusx()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(-45.0),osg::Vec3d(1,0,0));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelrotplusy()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(45.0),osg::Vec3d(0,1,0));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelrotminusy()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(-45.0),osg::Vec3d(0,1,0));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelrotplusz()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(45.0),osg::Vec3d(0,0,1));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelrotminusz()
{
	osg::Quat q1=tractor->asPositionAttitudeTransform()->getAttitude();
	q1*=osg::Quat((double)osg::DegreesToRadians(-45.0),osg::Vec3d(0,0,1));
	tractor->asPositionAttitudeTransform()->setAttitude(q1);
}
void osgView::modelscale(double scale)
{
	osg::Vec3d scaleorig=tractor->asPositionAttitudeTransform()->getScale();
	scaleorig=scaleorig*scale;
	tractor->asPositionAttitudeTransform()->setScale(scaleorig);
}
void osgView::save_model(string fname)
{
	osgDB::writeNodeFile(*tractor,fname);
}
void osgView::set_viewer()
{
	//set to viewer and draw
	viewer.setSceneData( root );
	//viewer.getCamera()->setProjectionMatrixAsPerspective(45.0, 1.0, 0.5, 1000); 
	viewer.setUpViewInWindow(10,10,WINDOW_WIDTH,WINDOW_HEIGHT,0);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getCameraManipulator()->setHomePosition(osg::Vec3d(-10000,5000,100000),osg::Vec3d(-xmax/2,ymax/2,0),osg::Vec3d(0,-1,0));
	//viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3d(200,200,200),osg::Vec3d(0,0,0),osg::Vec3d(0,1,0));
	//viewer.setCamera(cam);
	viewer.home();
	viewer.realize();
}
void osgView::drawFlowAnimation()
{
	transform = new osg::PositionAttitudeTransform();
	osg::ref_ptr<osg::Geode> temp = new osg::Geode();
	transform->addChild(temp);
	transform->setPosition(osg::Vec3(0,0,0));
	transform->addChild(temp);
	transform->setPosition(osg::Vec3(0,0,0));
	//transform[1].addChild(temp);
	//transform[1].setPosition(osg::Vec3(0,0,0));
	return;
}
osgView::~osgView()
{

}

void osgView::setmax(int x,int y,int z,double depth)
{
	xmax=x*SCALE;
	ymax=y*SCALE;
	zmax=z*SCALE;
	bgwall=(depth-BG_ERROR)*SCALE*zscale;
}
void osgView::makeColorlist(vector<vector<showcircle> > tracks)
{
	colorlist = new osg::Vec4Array;
	maxsize=0;totalelements=0;
	for(unsigned int i=0;i<tracks.size();i++)
	{
		totalelements+=tracks[i].size();
		maxsize=(maxsize<tracks[i].size())?tracks[i].size():maxsize;
	}
	
	int rr=0,gg=0,bb=0;
	osg::Vec4 c;
	for(unsigned int i=0;i<maxsize;i++)
	{
		rr=rr%256;
		gg=gg%256;
		bb=bb%256;
		if(rr%256==0 && gg%256==0 && bb%256==0)
		{
			rr=rand()%256;gg=rand()%256;bb=rand()%256;
		}
		
		c.set(float(rr)/255,float(gg)/255,float(bb)/255,0.0);
		colorlist->push_back(c);
		rr+=10;
		gg+=20;
		bb+=30;
	}
}
void osgView::set_tracks(vector<vector<showcircle> > t)
{
	makeColorlist(t);
	set_track=true;
	totalpoints=0;
	for (unsigned int i=0;i<t.size();i++)
	{
		osg::ref_ptr<osg::Vec3Array> track = new osg::Vec3Array;
		osg::Vec3 temp;
		vector<osg::Vec3d> mvector;
		vector<bool>framev;
		vector<vector<osg::Vec3d> >c1;
		for(unsigned int j=0;j<t[i].size();j++)
		{
			vector<osg::Vec3d> c2;
			osg::Vec3d mv; 
			bool tv=t[i][j].visible;
			framev.push_back(tv);
			double x,y,z;
			x=(double)t[i][j].p.x;
			y=(double)t[i][j].p.y;
			z=t[i][j].z;
			temp.set(x,y,z);
			mv.set(t[i][j].vec.val[0],t[i][j].vec.val[1],t[i][j].vec.val[2]);
			mvector.push_back(mv);
			track->push_back(temp);
			for(int k=0;k<t[i][j].contour.size();k++)
			{
				x=(double)t[i][j].contour[k].x;
				y=(double)t[i][j].contour[k].y;
				z=t[i][j].z;
				c2.push_back(osg::Vec3d(x,y,z));
				totalpoints++;
			}
			c1.push_back(c2);
		}
		motionvector.push_back(mvector);
		contourlist.push_back(c1);
		visible.push_back(framev);
		tracks.push_back(track);
	}
	animationcount=1;
	animationend=t.size();
}

void osgView::correct_for_age()
{
	for(int j=0;j<maxsize;j++)
	{
		int c=0;
		for(int i=0;i<animationend;i++)
		{
			if(j>visible[i].size()-1)
				continue;
			if(visible[i][j])
				c++;
		}
		if(c<AGE_THRESH+2)
		{
			for(int i=0;i<animationend;i++)
			{
				if(j>visible[i].size()-1)
					continue;
				visible[i][j]=false;
			}
		}
	}
}

void osgView::getvideo()
{
	image->readPixels(0,0,WINDOW_WIDTH,WINDOW_HEIGHT, GL_BGR,GL_UNSIGNED_BYTE); 
	//uchar *data=(uchar*)malloc(WINDOW_WIDTH*WINDOW_HEIGHT*3);
	//glReadPixels(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,data);
	cv::Mat glCapture(WINDOW_HEIGHT,WINDOW_WIDTH,CV_8UC3);
	memcpy(glCapture.data,image->data(),WINDOW_WIDTH*WINDOW_HEIGHT*3);
	cv::flip(glCapture,glCapture,0);
	osgVideo.push_back(glCapture);
	//viewer.getCameraManipulator()->setHomePosition(osg::Vec3d(1200,1200,1200),osg::Vec3d(0,0,0),osg::Vec3d(0,-1,0));
	//cv::imshow("asa",glCapture);
	//cv::waitKey(10);
}
void osgView::checktractorplate(bool yn){
	if(yn)
		tractorplate->addChild(drawTractorplate());
	else
		tractorplate->removeChild(0,1);
}
void osgView::load_model(string modelname){
	tractor->removeChild(0,1);
	osg::ref_ptr<osg::PositionAttitudeTransform> mytransform = new osg::PositionAttitudeTransform();
	mytransform->addChild(osgDB::readNodeFile(modelname));
	mytransform->setAttitude(osg::Quat(osg::DegreesToRadians(90.0f),osg::Vec3d(1.0,0.0,0.0),
									   osg::DegreesToRadians(90.0f),osg::Vec3d(0.0,1.0,0.0),
									   osg::DegreesToRadians(0.0f),osg::Vec3d(0.0,0.0,1.0)));
	mytransform->setPosition(osg::Vec3d(xtrans_object,ytrans_object,ztrans_object));
	tractor->addChild(mytransform);
}
void osgView::remove_model(){
	tractor->removeChild(0,1);
}
void osgView::export2vtk(string fname,int type){

	for(unsigned int i=0;i<contourlist.size();i++){
		std::cout<<i<<std::endl;
		//setup the variables
		char num[10];
		sprintf(num,"%03d",i);
		int pointcount=0;
		vector<vector<float>> centres;
		vector<float> repdiam;
		vtkPoints* polygonPoints = vtkPoints::New();
		vtkPolyData* polydata = vtkPolyData::New();
		vtkFloatArray* Colors = vtkFloatArray::New();
		vtkFloatArray* PointID = vtkFloatArray::New();
		vtkFloatArray* Centres = vtkFloatArray::New();
		vtkFloatArray* RepDiam = vtkFloatArray::New();
		vtkFloatArray* MotionVector = vtkFloatArray::New();
		vtkUnsignedCharArray *colors= vtkUnsignedCharArray::New();
		colors->Allocate(contourlist[i].size()*3);
		colors->SetNumberOfComponents(3);
		//name the variables
		Colors->SetName("Colors");
		PointID->SetName("PointID");
		Centres->SetName("Centres");
		RepDiam->SetName("Diameter");
		MotionVector->SetName("MotionVector");

		//counting how many points per frame
		for(unsigned int j=0;j<contourlist[i].size();j++){
			if(!visible[i][j])
				continue;
			float sumx=0,sumy=0,sumz=0;
			for(unsigned int k=0;k<contourlist[i][j].size();k++){
				pointcount++;
				sumx+=contourlist[i][j][k].x()*-SCALE;
				sumy+=contourlist[i][j][k].y()*SCALE;
				sumz+=contourlist[i][j][k].z()*SCALE;
			}
			vector<float>tempc(3);
			tempc[0]=sumx/contourlist[i][j].size();tempc[1]=sumy/contourlist[i][j].size();tempc[2]=sumz/contourlist[i][j].size();
			centres.push_back(tempc);
		}
		if(type==TYPE_POLYGONS)
			polygonPoints->SetNumberOfPoints( pointcount );
		if(type==TYPE_SPHERES)
			polygonPoints->SetNumberOfPoints( centres.size() );
		pointcount=0;
		int sepcount=0;
		for(unsigned int j=0;j<contourlist[i].size();j++){
			if(!visible[i][j])
				continue;
			float maxdiam=0;
			for(unsigned int k=0;k<contourlist[i][j].size();k++){
				if(type==TYPE_POLYGONS)
					polygonPoints->InsertPoint(pointcount++,contourlist[i][j][k].x()*-SCALE,contourlist[i][j][k].y()*SCALE,contourlist[i][j][k].z()*SCALE);
				float diam=sqrt((centres[sepcount][0]-contourlist[i][j][k].x()*-SCALE)*(centres[sepcount][0]-contourlist[i][j][k].x()*-SCALE) + 
								(centres[sepcount][1]-contourlist[i][j][k].y()*SCALE)*(centres[sepcount][1]-contourlist[i][j][k].y()*SCALE) +
								(centres[sepcount][2]-contourlist[i][j][k].z()*SCALE)*(centres[sepcount][2]-contourlist[i][j][k].z()*SCALE));
				maxdiam=(maxdiam < diam?diam:maxdiam);
			}
			if(type==TYPE_SPHERES)
					polygonPoints->InsertPoint(sepcount,centres[sepcount][0],centres[sepcount][1],centres[sepcount][2]);
			sepcount++;
			repdiam.push_back(maxdiam);
		}
		char numtype[5];
		sprintf(numtype,"%d",type);
		string filename=fname+string("_")+string(num)+string(".vtp");
		if(type==TYPE_POLYGONS){
			polydata->Allocate(pointcount);
			polydata->SetPoints(polygonPoints);
		}
		if(type==TYPE_SPHERES){
			polydata->Allocate(centres.size());
			polydata->SetPoints(polygonPoints);
		}
		Colors->Allocate(contourlist[i].size()*3);
		Colors->SetNumberOfComponents(3);
		PointID->Allocate(contourlist[i].size());
		PointID->SetNumberOfComponents(1);
		Centres->Allocate(contourlist[i].size()*3);
		Centres->SetNumberOfComponents(3);
		RepDiam->Allocate(contourlist[i].size());
		RepDiam->SetNumberOfComponents(1);
		MotionVector->Allocate(centres.size()*3);
		MotionVector->SetNumberOfComponents(3);
		sepcount=0;
		pointcount=0;
		for(unsigned int j=0;j<contourlist[i].size();j++){
			if(!visible[i][j])
				continue;
			unsigned char tempcolor[3]={(int)((*colorlist)[j][0]*255),(int)((*colorlist)[j][1]*255),(int)((*colorlist)[j][2]*255)};
			colors->InsertNextTupleValue(tempcolor);
			float *tempfloat=new float[3];
			tempfloat[0]=(*colorlist)[j][0];tempfloat[1]=(*colorlist)[j][1];tempfloat[2]=(*colorlist)[j][2];
			Colors->InsertNextTuple(tempfloat);
			float temppid=(float)j;
			PointID->InsertNextTuple(&temppid);
			float *tempcentres=new float[3];
			tempcentres[0]=centres[sepcount][0];tempcentres[1]=centres[sepcount][1];tempcentres[2]=centres[sepcount][2];
			Centres->InsertNextTuple(tempcentres);
			RepDiam->InsertNextTuple(&repdiam[sepcount]);
			float *tempmotion=new float[3];
			tempmotion[0]=motionvector[i][j].x()*SCALE;tempmotion[1]=motionvector[i][j].y()*SCALE;tempmotion[2]=motionvector[i][j].z()*SCALE;
			MotionVector->InsertNextTuple(tempmotion);

			if(type==TYPE_POLYGONS){
				vtkPolygon* aPolygon = vtkPolygon::New();
				aPolygon->GetPointIds()->SetNumberOfIds( contourlist[i][j].size() );
				for(unsigned int k=0;k<contourlist[i][j].size();k++){
					aPolygon->GetPointIds()->SetId( k, pointcount++ );	
				}
				polydata->InsertNextCell( aPolygon->GetCellType(), aPolygon->GetPointIds() );
				aPolygon->Delete();
			}
			if(type==TYPE_SPHERES){
				vtkVertex* aVertex = vtkVertex::New();
				aVertex->GetPointIds()->SetNumberOfIds(1);
				aVertex->GetPointIds()->SetId( 0, sepcount);
				polydata->InsertNextCell( aVertex->GetCellType(), aVertex->GetPointIds() );
				aVertex->Delete();
			}
			delete[] tempfloat;
			delete[] tempcentres;
			delete[] tempmotion;
			sepcount++;
		}
		polydata->GetCellData()->SetScalars(colors);
		polydata->GetCellData()->AddArray(Colors);
		polydata->GetCellData()->AddArray(PointID);
		polydata->GetCellData()->AddArray(Centres);
		polydata->GetCellData()->AddArray(RepDiam);
		polydata->GetCellData()->AddArray(MotionVector);
		vtkXMLDataSetWriter* writer = vtkXMLDataSetWriter::New();
		writer->SetDataModeToAscii();
		writer->SetFileName(filename.c_str() );
		writer->SetInput( polydata );
		writer->Write();

		//Delete all the variables
		writer->Delete();
		polydata->Delete();
		polygonPoints->Delete();
		colors->Delete();
		Colors->Delete();
		PointID->Delete();
		Centres->Delete();
		RepDiam->Delete();
		MotionVector->Delete();
		for(unsigned t=0;t<centres.size();t++){
			if(centres[t].size())
				centres[t].clear();
		}
		centres.clear();
		repdiam.clear();
	}
}