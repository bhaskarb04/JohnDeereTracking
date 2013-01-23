#pragma once
#include "OSGView.h"
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <QObject>

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
	Q_OBJECT
public:
	ViewerWidget(QWidget * parent);
	void set_osgviewer(osgView* osgviewer_){osgviewer=osgviewer_;osgviewer->pause_on();}
	void draw_all();
	QWidget* addViewWidget( osg::Camera* camera, osg::Node* scene );
	osg::Camera* createCamera( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
	virtual void paintEvent( QPaintEvent* event ){ frame(); }
	osgView *osgviewer;
private slots:
	void updateosgviewer();
protected:
    QTimer _timer;
	QGridLayout* grid;
};