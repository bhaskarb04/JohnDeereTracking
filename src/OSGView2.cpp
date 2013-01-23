#include "OSGView2.h"

osgView::osgView()
{
	set_track=false;
	drawA=false;
	zscale=zscale=ZSCALE;
	ztranslate=ZTRANSLATE;
	pause_flag=false;
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
	if(pause_flag)
		return;

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
	}
}



void osgView::update2(float time)
{
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	stateset->setAttribute(new osg::Point(3.0f),osg::StateAttribute::ON);

	transform->removeChild(0,transform->getNumChildren()-1);
	for(unsigned int j=0;j<maxsize;j++)
	{
		osg::ref_ptr<osg::Geode> new_flow=new osg::Geode;
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(animationcount*2 - 1);
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(animationcount*2 - 1);
		int count=0;
		for(unsigned int i=0;i<animationcount;i++)
		{
			if(!tracks[i]->getNumElements() || j > tracks[i]->getNumElements()-1 || !visible[i][j])
				continue;
			(*vertices)[count]=(*tracks[i])[j];
			(*vertices)[count].set(-(*vertices)[count].x()*SCALE,(*vertices)[count].y()*SCALE,(*vertices)[count].z()*SCALE);
			(*colors)[count]=(*colorlist)[j];
			count++;
			if(count>1)
			{
				(*vertices)[count]=(*tracks[i])[j];
				(*vertices)[count].set(-(*vertices)[count].x()*SCALE,(*vertices)[count].y()*SCALE,(*vertices)[count].z()*SCALE);
				(*colors)[count]=(*colorlist)[j];
				count++;
			}
			if(i == animationcount - 1)
			{	
				osg::ref_ptr<osg::Sphere> cogs = new osg::Sphere((*vertices)[count-1]*SCALE,5.0);
				osg::ref_ptr<osg::ShapeDrawable> cogDrawable = new osg::ShapeDrawable(cogs);
				osg::ref_ptr<osg::Geode> cogGeode = new osg::Geode();
				cogGeode->addDrawable(cogDrawable);
				transform->addChild(cogGeode);
			}
		}
		osg::ref_ptr<osg::DrawArrays>indices =new osg::DrawArrays(osg::PrimitiveSet::LINES,2,count-2);
		geom->setVertexArray( vertices.get() );
		geom->setColorArray(colors.get());
		geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		geom->addPrimitiveSet( indices.get() );
		geom->setStateSet(stateset);
		new_flow->addDrawable(geom);
		transform->addChild(new_flow);
	}
	animationcount++;
	if(animationcount == animationend && LOOP)
		animationcount=1;
}

void osgView::drawAnimation()
{
	osgViewer::Viewer viewer;
	osg::Camera *cam= new osg::Camera();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	myKeyboardEventHandler* myFirstEventHandler = new myKeyboardEventHandler(this);
	tractor = new osg::Transform();
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
	if(WITH_MODEL);
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
	
	//set to viewer and draw
	viewer.addEventHandler(myFirstEventHandler); 
	viewer.setSceneData( root );
	//viewer.getCamera()->setProjectionMatrixAsPerspective(45.0, 1.0, 0.5, 1000); 
	viewer.setUpViewInWindow(10,10,WINDOW_WIDTH,WINDOW_HEIGHT,0);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getCameraManipulator()->setHomePosition(osg::Vec3d(-10000,5000,100000),osg::Vec3d(-xmax/2,ymax/2,0),osg::Vec3d(0,-1,0));
	//viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3d(200,200,200),osg::Vec3d(0,0,0),osg::Vec3d(0,1,0));
	//viewer.setCamera(cam);
	viewer.home();
	viewer.realize();
	//viewer.getCamera()->setFinalDrawCallback(
	image=new osg::Image();
	viewer.getCamera()->attach(osg::Camera::COLOR_BUFFER, image);
	string fname="file";
	string ending=".ive";
	char num[5];
	int count=0;
	while(!viewer.done())
	{
		viewer.frame();
		if(set_track)
		{
			if(!LOOP)
				getvideo();
			update(TIMER_);
			sprintf(num,"%d",count);
			string final=fname+string(num)+ending;
			osgDB::writeNodeFile(*transform,final,new osgDB::Options);
			count++;
		}
		
	}

}
void osgView::drawAnimation2()
{
	osgViewer::Viewer viewer;
	osg::Camera *cam= new osg::Camera();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	myKeyboardEventHandler* myFirstEventHandler = new myKeyboardEventHandler(this);
	tractor = new osg::Transform();
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
	ParticleCallback *ndc=new ParticleCallback;
	root->setUpdateCallback(ndc);
	//call drawing methods
	axes=drawAxes();
	if(WITH_MODEL_PLATE)
	tractorplate->addChild(drawTractorplate());
	if(WITH_MODEL);
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
	
	//set to viewer and draw
	viewer.addEventHandler(myFirstEventHandler); 
	viewer.setSceneData( root );
	//viewer.getCamera()->setProjectionMatrixAsPerspective(45.0, 1.0, 0.5, 1000); 
	viewer.setUpViewInWindow(10,10,WINDOW_WIDTH,WINDOW_HEIGHT,0);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getCameraManipulator()->setHomePosition(osg::Vec3d(-10000,5000,100000),osg::Vec3d(-xmax/2,ymax/2,0),osg::Vec3d(0,-1,0));
	//viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3d(200,200,200),osg::Vec3d(0,0,0),osg::Vec3d(0,1,0));
	//viewer.setCamera(cam);
	viewer.home();
	viewer.realize();
	//viewer.getCamera()->setFinalDrawCallback(
	image=new osg::Image();
	viewer.getCamera()->attach(osg::Camera::COLOR_BUFFER, image);
	while(!viewer.done())
	{
		viewer.frame();
		if(set_track)
		{
			if(!LOOP)
				getvideo();
			//update(TIMER_);
		}
		
	}
}
osgView::~osgView()
{

}

void osgView::_setmax(int x,int y,int z,double depth)
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
void osgView::_settracks(vector<vector<showcircle> > t)
{
	makeColorlist(t);
	set_track=true;
	for (unsigned int i=0;i<t.size();i++)
	{
		osg::ref_ptr<osg::Vec3Array> track = new osg::Vec3Array;
		osg::Vec3 temp;
		vector<bool>framev;
		vector<vector<osg::Vec3d> >c1;
		for(unsigned int j=0;j<t[i].size();j++)
		{
			vector<osg::Vec3d> c2;
			bool tv=t[i][j].visible;
			framev.push_back(tv);
			double x,y,z;
			x=(double)t[i][j].p.x;
			y=(double)t[i][j].p.y;
			z=t[i][j].z;
			temp.set(x,y,z);
			track->push_back(temp);
			for(int k=0;k<t[i][j].contour.size();k++)
			{
				x=(double)t[i][j].contour[k].x;
				y=(double)t[i][j].contour[k].y;
				z=t[i][j].z;
				c2.push_back(osg::Vec3d(x,y,z));
			}
			c1.push_back(c2);
		}
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

osg::ref_ptr<osg::Geode> osgView::drawFlow()
{
	osg::ref_ptr<osg::Geode> return_flow=new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(totalelements);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(totalelements);
	int count=0;

	for(unsigned int i=0;i<tracks.size();i++)
	{
		if(tracks[i]->getNumElements())
		{
			for(unsigned int j=0;j<tracks[i]->getNumElements();j++)
			{
				(*vertices)[count]=(*tracks[i])[j];
				(*colors)[count]=(*colorlist)[j];
				count++;
			}

		}
	}
	osg::ref_ptr<osg::DrawArrays>indices =new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,totalelements);

	geom->setVertexArray( vertices.get() );
	geom->setColorArray(colors.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geom->addPrimitiveSet( indices.get() );
	osg::StateSet* stateset = new osg::StateSet;
	//stateset->setAttribute(new osg::point( 3.0f ),osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geom->setStateSet(stateset);
	return_flow->addDrawable(geom);
	return (return_flow.release());
}
void osgView::drawFlowAnimation()
{
	transform = new osg::PositionAttitudeTransform();
	osg::ref_ptr<osg::Geode> temp = new osg::Geode();
	transform->addChild(temp);
	transform->setPosition(osg::Vec3(0,0,0));
	transform->addChild(temp);
	transform->setPosition(osg::Vec3(0,0,0));
	return;
}
void osgView::getvideo()
{
	image->readPixels(0,0,WINDOW_WIDTH,WINDOW_HEIGHT, GL_BGR,GL_UNSIGNED_BYTE); 
	cv::Mat glCapture(WINDOW_HEIGHT,WINDOW_WIDTH,CV_8UC3);
	memcpy(glCapture.data,image->data(),WINDOW_WIDTH*WINDOW_HEIGHT*3);
	cv::flip(glCapture,glCapture,0);
	osgVideo.push_back(glCapture);
}


myKeyboardEventHandler::myKeyboardEventHandler(osgView* t)
{
	ptr=t;
}

bool myKeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
 {
   switch(ea.getEventType())
   {
	case(osgGA::GUIEventAdapter::KEYDOWN):
      {
         switch(ea.getKey())
         {
         case 's':
			 ptr->zscale=max(1.0,ptr->zscale-0.1);
			std::cout << " depth scaled by " << ptr->zscale<< std::endl;
            return false;
            break;
		case 'S':
			ptr->zscale=ptr->zscale+0.1;
			std::cout << " depth scaled by " << ptr->zscale<< std::endl;
            return false;
            break;
		case 'm':
			if(!ptr->tractor->getNumChildren())
			{
				ptr->tractor->addChild(ptr->drawTractor());
				ptr->ztranslate=ZTRANSLATE;
				ptr->tractorplate->removeChild(0,1);
			}
			else
			{
				ptr->tractor->removeChild(0,1);
				ptr->ztranslate=0;
				ptr->tractorplate->addChild(ptr->drawTractorplate());
			}
            return false;
            break;
		case 'p':
			ptr->pause_flag=!ptr->pause_flag;
			break;
		case 'z':
			ptr->ztrans_particle+=TRANS_STEP;
			break;
		case 'Z':
			ptr->ztrans_particle-=TRANS_STEP;
			break;
		case (osgGA::GUIEventAdapter::KEY_Left):
				ptr->xtrans_particle+=TRANS_STEP;
				break;
		case (osgGA::GUIEventAdapter::KEY_Right):
				ptr->xtrans_particle-=TRANS_STEP;
				break;
		case(osgGA::GUIEventAdapter::KEY_Up):
				ptr->ytrans_particle-=TRANS_STEP;
				break;
		case(osgGA::GUIEventAdapter::KEY_Down):
				ptr->ytrans_particle+=TRANS_STEP;
				break;
		 } 
      }
   default:
      return false;
   }

}

void OSGViewer::run()
{
	osgViewer::Viewer viewer;
	ParticleCallback *ndc=new ParticleCallback;
	osg::Camera *cam= new osg::Camera();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	myKeyboardEventHandler* myFirstEventHandler = new myKeyboardEventHandler(this->osgview);
	osgview->tractor = new osg::Transform();
	osgview->tractorplate = new osg::Transform();
	osgview->xtrans_object=-osgview->xmax/2;
	osgview->ytrans_object=osgview->ymax/2;
	osgview->ztrans_object=-10000;
	//Lighting
	osg::Light *light=new osg::Light();
	light->setAmbient(osg::Vec4d(0.5,0.5,0.5,0.0));
	//light->setConstantAttenuation(0.5);
	light->setDiffuse(osg::Vec4d(1.0, 1.0, 1.0, 0.0));
	light->setPosition(osg::Vec4d(-10000,-5000,0,1.0));
	osg::LightSource * lightsource = new osg::LightSource();
	lightsource->setLight(light);
	//call drawing methods
	osgview->axes=osgview->drawAxes();
	if(WITH_MODEL_PLATE)
	osgview->tractorplate->addChild(osgview->drawTractorplate());
	if(WITH_MODEL);
		//osgview->tractor->addChild(osgview->drawTractor());
	if(osgview->set_track)
		osgview->drawFlowAnimation();
	
	//link to root
	root->addChild(osgview->axes);
	root->addChild(lightsource);
	if(WITH_MODEL_PLATE)
		root->addChild(osgview->tractorplate);
	if(WITH_MODEL)
		root->addChild(osgview->tractor);
	if(osgview->set_track)
	{
		//root->addChild(osgview->transform->asPositionAttitudeTransform());
		root->setUserData(osgview);
		root->setUpdateCallback(ndc);
	}
	//set to viewer and draw
	viewer.addEventHandler(myFirstEventHandler); 
	viewer.setSceneData( root );
	//viewer.getCamera()->setProjectionMatrixAsPerspective(45.0, 1.0, 0.5, 1000); 
	viewer.setUpViewInWindow(10,10,WINDOW_WIDTH,WINDOW_HEIGHT,0);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getCameraManipulator()->setHomePosition(osg::Vec3d(-10000,5000,100000),
		osg::Vec3d(-osgview->xmax/2,osgview->ymax/2,0),osg::Vec3d(0,-1,0));
	//viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3d(200,200,200),osg::Vec3d(0,0,0),osg::Vec3d(0,1,0));
	//viewer.setCamera(cam);
	viewer.home();
	viewer.realize();
	//viewer.getCamera()->setFinalDrawCallback(
	image=new osg::Image();
	viewer.getCamera()->attach(osg::Camera::COLOR_BUFFER, image);
	while(!viewer.done())
	{
		viewer.frame();
		if(osgview->set_track)
		{
			//if(!LOOP)
				//osgview->getvideo();
			//update(TIMER_);
		}
		if(osgview->animationcount == osgview->animationend && !LOOP)
			break;
	}
}