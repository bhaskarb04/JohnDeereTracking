#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Tracker.h"
#include "RWVector.h"
#include "OSGView.h"
#include "OSGQTWidget.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QProgressBar>
#include <QGLWidget>
#include <osgViewer/CompositeViewer>
#include <osgQt/GraphicsWindowQt>

#define DEFAULT_TIMER_INTERVAL 33
#define PICFORMAT ".bmp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
	Tracker *track;
	std::string path;
	string origpath;
	string cleanedpath;
	string contpath;
	string immpath;
	string modelpath;
	QImage *list;
	QTimer *timer;
	int timer_interval,current_frame,frame_end;
	osgView *osgviewer;
	bool threed;
private slots:
	void load_button_click();
	void load_original(bool);
	void load_cleaned(bool);
	void load_contours(bool);
	void load_threedimmersive(bool);
	void play_click();
	void stop_click();
	void next_click();
	void prev_click();
	void first_click();
	void last_click();
	void slider_change(int);
	void analyze();
	void load_data();
	void save_data();
	void plusX();
	void minusX();
	void plusY();
	void minusY();
	void plusZ();
	void minusZ();
	void modelplusx();
	void modelminusx();
	void modelplusy();
	void modelminusy();
	void modelplusz();
	void modelminusz();
	void modelrotplusx();
	void modelrotminusx();
	void modelrotplusy();
	void modelrotminusy();
	void modelrotplusz();
	void modelrotminusz();
	void modelscale();
	void tractorplatecheckbox(bool);
	void loadmodel();
	void savemodifiedmodel();
	void removemodel();

};

class MyGLDrawer : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     MyGLDrawer(QWidget *parent)
         : QGLWidget(parent) {}

 protected:
	 GLuint num;
	 QImage buffer;
     void initializeGL()
     {
		// Set up the rendering context, define display lists etc.:
		glClearColor(1.0, 1.0, 1.0, 0.0);
		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glEnable(GL_DEPTH_TEST);
		glOrtho(-1,1,-1,1,-1,1);
		glGenTextures(1,&num);*/
     }
	 void paintGL();
 public:
	 void drawImage(QImage& image);

 };

#endif // MAINWINDOW_H
