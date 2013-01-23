#include "OSGQTWidget.h"

ViewerWidget::ViewerWidget(QWidget* parent) : QWidget(parent)
{
	setThreadingModel(osgViewer::ViewerBase::CullDrawThreadPerContext);
	grid = new QGridLayout;
	setLayout( grid );
}

void ViewerWidget::draw_all()
{
	QWidget* widget1 = addViewWidget( createCamera(0,0,100,100), osgviewer->return_scene());
	grid->addWidget( widget1, 0, 0 );
	connect( &_timer, SIGNAL(timeout()), this, SLOT(updateosgviewer()) );
	_timer.start( TIMER_ );
}
QWidget* ViewerWidget::addViewWidget( osg::Camera* camera, osg::Node* scene )
{
	osgViewer::View* view = new osgViewer::View;
	view->setCamera( camera );
	addView( view );
        
	view->setSceneData( scene );
	view->addEventHandler( new osgViewer::StatsHandler );
	view->setCameraManipulator( new osgGA::TrackballManipulator );
        
	osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
	return gw ? gw->getGLWidget() : NULL;
}
osg::Camera* ViewerWidget::createCamera( int x, int y, int w, int h, const std::string& name, bool windowDecoration )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();
        
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits.get()) );
        
	camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
	camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
	camera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
	camera->setViewMatrixAsLookAt(osg::Vec3d(-10000,5000,100000),osg::Vec3d(0,0,0),osg::Vec3d(0,-1,0));
	return camera.release();
}
void ViewerWidget::updateosgviewer()
{
	osgviewer->update(TIMER_);
	cv::waitKey(300);
	update();
}