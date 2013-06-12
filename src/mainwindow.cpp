#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	//set default path
	path="TrackedData";
	//default look so that user does not press any other button
    ui->setupUi(this);
	ui->AnalyzeButton->setEnabled(false);
	ui->CleanedRB->setEnabled(false);
	ui->ContoursRB->setEnabled(false);
	ui->DisplayWidget->setEnabled(false);
	ui->ExportButton->setEnabled(false);
	ui->FirstButton->setEnabled(false);
	ui->FrameSlider->setEnabled(false);
	ui->LastButton->setEnabled(false);
	//ui->LoadButton->setEnabled(false);
	ui->LoadProcButton->setEnabled(false);
	ui->MinusXButton->setEnabled(false);
	ui->MinusYButton->setEnabled(false);
	ui->MinusZButton->setEnabled(false);
	ui->NextButton->setEnabled(false);
	ui->OriginalRB->setEnabled(false);
	ui->PlayButton->setEnabled(false);
	ui->PlusXButton->setEnabled(false);
	ui->PlusYButton->setEnabled(false);
	ui->PlusZButton->setEnabled(false);
	ui->PrevButton->setEnabled(false);
	ui->SaveProcButton->setEnabled(false);
	ui->StopButton->setEnabled(false);
	ui->ThreeDRB->setEnabled(false);
	ui->DisplayWidget3D->setVisible(false);
	ui->loadModelButton->setEnabled(false);
	ui->showGreenBGBox->setEnabled(false);

	list=NULL;
	//make/check the required directories
	if(!QDir(QString(path.c_str())).exists())
		QDir().mkdir(QString(path.c_str()));
	//timer
	timer = new QTimer(this);
	timer_interval= DEFAULT_TIMER_INTERVAL;
	current_frame=0;
	//slider
	
	//connect the slots
	//buttons
	connect(ui->LoadButton,SIGNAL(clicked()),this,SLOT(load_button_click()));
	connect(ui->AnalyzeButton,SIGNAL(clicked()),this,SLOT(analyze()));
	//radio buttons
	connect(ui->OriginalRB,SIGNAL(toggled(bool)),this,SLOT(load_original(bool)));
	connect(ui->CleanedRB,SIGNAL(toggled(bool)),this,SLOT(load_cleaned(bool)));
	connect(ui->ContoursRB,SIGNAL(toggled(bool)),this,SLOT(load_contours(bool)));
	connect(ui->ThreeDRB,SIGNAL(toggled(bool)),this,SLOT(load_threedimmersive(bool)));
	//player buttons
	connect(ui->PlayButton,SIGNAL(clicked()),this,SLOT(play_click()));
	connect(ui->StopButton,SIGNAL(clicked()),this,SLOT(stop_click()));
	connect(ui->NextButton,SIGNAL(clicked()),this,SLOT(next_click()));
	connect(ui->PrevButton,SIGNAL(clicked()),this,SLOT(prev_click()));
	connect(ui->LastButton,SIGNAL(clicked()),this,SLOT(last_click()));
	connect(ui->FirstButton,SIGNAL(clicked()),this,SLOT(first_click()));
	connect(ui->FrameSlider,SIGNAL(valueChanged(int)),this,SLOT(slider_change(int)));
	//osg part
	osgviewer = new osgView;
	//osg widget display
	threed=false;
	//load data + save data
	connect(ui->LoadProcButton,SIGNAL(clicked()),this,SLOT(load_data()));
	connect(ui->SaveProcButton,SIGNAL(clicked()),this,SLOT(save_data()));
	//scene buttons
	connect(ui->PlusXButton,SIGNAL(clicked()),this,SLOT(plusX()));
	connect(ui->MinusXButton,SIGNAL(clicked()),this,SLOT(minusX()));
	connect(ui->PlusYButton,SIGNAL(clicked()),this,SLOT(plusY()));
	connect(ui->MinusYButton,SIGNAL(clicked()),this,SLOT(minusY()));
	connect(ui->PlusZButton,SIGNAL(clicked()),this,SLOT(plusZ()));
	connect(ui->MinusZButton,SIGNAL(clicked()),this,SLOT(minusZ()));
	connect(ui->showGreenBGBox,SIGNAL(toggled(bool)),this,SLOT(tractorplatecheckbox(bool)));
	connect(ui->loadModelButton,SIGNAL(clicked()),this,SLOT(loadmodel()));
	connect(ui->removeModelButton,SIGNAL(clicked()),this,SLOT(removemodel()));
	//model buttons
	connect(ui->modelplusX,SIGNAL(clicked()),this,SLOT(modelplusx()));
	connect(ui->modelminusX,SIGNAL(clicked()),this,SLOT(modelminusx()));
	connect(ui->modelplusY,SIGNAL(clicked()),this,SLOT(modelplusy()));
	connect(ui->modelminusY,SIGNAL(clicked()),this,SLOT(modelminusy()));
	connect(ui->modelplusZ,SIGNAL(clicked()),this,SLOT(modelplusz()));
	connect(ui->modelminusZ,SIGNAL(clicked()),this,SLOT(modelminusz()));
	connect(ui->RotPlusX,SIGNAL(clicked()),this,SLOT(modelrotplusx()));
	connect(ui->RotMinusX,SIGNAL(clicked()),this,SLOT(modelrotminusx()));
	connect(ui->RotPlusY,SIGNAL(clicked()),this,SLOT(modelrotplusy()));
	connect(ui->RotMinusY,SIGNAL(clicked()),this,SLOT(modelrotminusy()));
	connect(ui->RotPlusZ,SIGNAL(clicked()),this,SLOT(modelrotplusz()));
	connect(ui->RotMinusZ,SIGNAL(clicked()),this,SLOT(modelrotminusz()));
	connect(ui->ScaleButton,SIGNAL(clicked()),this,SLOT(modelscale()));
	connect(ui->SaveModButton,SIGNAL(clicked()),this,SLOT(savemodifiedmodel()));
	//VE Suite export
	connect(ui->ExportButton,SIGNAL(clicked()),this,SLOT(exportvtk()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::load_button_click()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty())
	{
		try
		{
			track= new Tracker(std::string(fileName.toLocal8Bit().constData()),true);
			QStringList filenameList=fileName.split("/");
			fileName=filenameList[filenameList.size()-1];
			fileName.chop(4);
			path+="/"+std::string(fileName.toLocal8Bit().constData());
			origpath=path+"/"+"Original";
			cleanedpath=path+"/"+"Cleaned";
			contpath=path+"/"+"Contours";
			immpath=path+"/"+"Immersive";
			std::vector<cv::Mat> imgs=track->get_orig_images();
			list = new QImage[imgs.size()];//make list
			//set up slider range
			frame_end=imgs.size();
			ui->FrameSlider->setMaximum(frame_end-1);
			ui->FrameSlider->setMinimum(0);

			if(!QDir(QString(path.c_str())).exists())
			{
				QDir().mkdir(QString(path.c_str()));
				if(!QDir(QString(origpath.c_str())).exists())
				{
					QDir().mkdir(QString(origpath.c_str()));
					for(int i=0;i<imgs.size();i++)
					{
						char num[5];
						sprintf(num,"%03d",i);
						string fpath=origpath+"/"+string(num)+PICFORMAT;
						cv::imwrite(fpath,imgs[i]);
					}
				}
			}
			ui->AnalyzeButton->setEnabled(true);
			ui->OriginalRB->setEnabled(true);
			ui->OriginalRB->setChecked(true);
			ui->FirstButton->setEnabled(true);
			ui->PrevButton->setEnabled(true);
			ui->NextButton->setEnabled(true);
			ui->LastButton->setEnabled(true);
			ui->PlayButton->setEnabled(true);
			ui->StopButton->setEnabled(true);
			ui->FrameSlider->setEnabled(true);
			ui->LoadProcButton->setEnabled(true);

		}
		catch (MyException& e)
		{
			QMessageBox msgBox;
			msgBox.setText(e.what());
			msgBox.exec();
		}
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose a movie file of the required data");
		msgBox.exec();
	}

}
QImage Mat2QImage(const cv::Mat3b &src) {
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y) {
                const cv::Vec3b *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < src.cols; ++x) {
                        destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                }
        }
        return dest;
}
//TODO:There is a smal bug here original doesnt remain original after processing
void MainWindow::load_original(bool value)
{
	if(!value)
		return;
	ui->DisplayWidget->setVisible(true);
	ui->DisplayWidget3D->setVisible(false);
	threed=false;
	current_frame=0;
	std::vector<cv::Mat> imgs=track->get_orig_images();
	for(unsigned int i=0;i<imgs.size();i++)
		list[i]=Mat2QImage(imgs[i]).copy();
	ui->FrameSlider->setValue(current_frame);
}
void MainWindow::load_cleaned(bool value)
{
	if(!value)
		return;
	ui->DisplayWidget->setVisible(true);
	ui->DisplayWidget3D->setVisible(false);
	threed=false;
	current_frame=0;
	std::vector<cv::Mat> imgs=track->get_current_images();
	for(unsigned int i=0;i<imgs.size();i++)
		list[i]=Mat2QImage(imgs[i]).copy();
	ui->FrameSlider->setValue(current_frame);
}
void MainWindow::load_contours(bool value)
{
	if(!value)
		return;
	ui->DisplayWidget->setVisible(true);
	ui->DisplayWidget3D->setVisible(false);
	threed=false;
	current_frame=0;
	std::vector<cv::Mat> imgs=track->get_current_images();
	for(unsigned int i=0;i<imgs.size();i++)
		list[i]=Mat2QImage(imgs[i]).copy();
	ui->FrameSlider->setValue(current_frame);
}
//TODO:Add the player stuff for threed also
void MainWindow::play_click()
{
	if(ui->PlayButton->text() == QString("Play")){
		ui->PlayButton->setText(QString("Pause"));
		connect(timer, SIGNAL(timeout()), this, SLOT(next_click()));
		timer->start(timer_interval);
	}
	else{
		timer->stop();
		disconnect(timer,0,0,0);
		ui->PlayButton->setText(QString("Play"));
	}
}
void MainWindow::stop_click()
{
	timer->stop();
	disconnect(timer,0,0,0);
	ui->PlayButton->setText(QString("Play"));
	current_frame=0;
	ui->FrameSlider->setValue(current_frame);
}
void MainWindow::next_click()
{
	current_frame++;
	if(current_frame==frame_end)
		current_frame=0;
	ui->FrameSlider->setValue(current_frame);

}
void MainWindow::prev_click()
{
	current_frame--;
	if(current_frame<0)
		current_frame=frame_end-1;
	ui->FrameSlider->setValue(current_frame);
	
}
void MainWindow::first_click()
{
	current_frame=0;
	ui->FrameSlider->setValue(current_frame);
}
void MainWindow::last_click()
{
	current_frame=frame_end-1;
	ui->FrameSlider->setValue(current_frame);
}
void MainWindow::slider_change(int dummy)
{
	current_frame=dummy;
	if(threed){
		ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
		ui->DisplayWidget3D->osgviewer->allowonce();	
	}
	else
		ui->DisplayWidget->drawImage(list[current_frame]);
	ui->FrameRangeLabel->setText(QString::number(current_frame)+QString("/")+QString::number(frame_end-1));
}
void MainWindow::analyze()
{
	track->clean_image(false);

	if(!QDir(QString(cleanedpath.c_str())).exists())
	{
		std::vector<cv::Mat> imgs=track->get_current_images();
		QDir().mkdir(QString(cleanedpath.c_str()));
		for(int i=0;i<imgs.size();i++)
		{
			char num[5];
			sprintf(num,"%03d",i);
			string fpath=cleanedpath+"/"+string(num)+PICFORMAT;
			cv::imwrite(fpath,imgs[i]);
		}
	}
	track->track_particles(false);
	if(!QDir(QString(contpath.c_str())).exists())
	{
		std::vector<cv::Mat> imgs=track->get_current_images();
		QDir().mkdir(QString(contpath.c_str()));
		for(int i=0;i<imgs.size();i++)
		{
			char num[5];
			sprintf(num,"%03d",i);
			string fpath=contpath+"/"+string(num)+PICFORMAT;
			cv::imwrite(fpath,imgs[i]);
		}
	}
	//if(!QDir(QString(immpath.c_str())).exists())
			//QDir().mkdir(QString(immpath.c_str()));
	ui->CleanedRB->setEnabled(true);
	ui->ContoursRB->setEnabled(true);
	ui->ThreeDRB->setEnabled(true);
	ui->SaveProcButton->setEnabled(true);
	ui->PlusXButton->setEnabled(true);
	ui->PlusYButton->setEnabled(true);
	ui->PlusZButton->setEnabled(true);
	ui->MinusXButton->setEnabled(true);
	ui->MinusYButton->setEnabled(true);
	ui->MinusZButton->setEnabled(true);
	ui->loadModelButton->setEnabled(true);
	ui->showGreenBGBox->setEnabled(true);
	ui->ExportButton->setEnabled(true);
	osgviewer->setmax(track->imgcols,track->imgrows,255,0);
	osgviewer->set_tracks(track->tracks);
	osgviewer->set_scene();
	//osgviewer->set_viewer();
}
void MainWindow::load_threedimmersive(bool value)
{
	if(!value)
		return;
	threed=true;
	ui->DisplayWidget->setVisible(false);
	ui->DisplayWidget3D->setVisible(true);
	ui->DisplayWidget3D->set_osgviewer(osgviewer);
	ui->DisplayWidget3D->draw_all();
}
void MainWindow::load_data(){
	string fname=path+"/AnalyzedData.dat";
	QFile fout(QString(fname.c_str()));
	if(!fout.exists()){
		QMessageBox msgBox;
		msgBox.setText("The data has not been analyzed yet");
		msgBox.exec();
		return;
	}
	double bgdepth;
	track->tracks=read_vector((char*)fname.c_str(),track->imgrows,track->imgcols,bgdepth);
	osgviewer->setmax(track->imgcols,track->imgrows,255,0);
	osgviewer->set_tracks(track->tracks);
	osgviewer->set_scene();
	if(QDir(QString(cleanedpath.c_str())).exists())
	{
		std::vector<cv::Mat> imgs;
		for(int i=0;i<track->get_orig_images().size();i++)
		{
			char num[5];
			sprintf(num,"%03d",i);
			string fpath=cleanedpath+"/"+string(num)+PICFORMAT;
			imgs.push_back(cv::imread(fpath));
		}
		track->set_cleaned_images(imgs);
	}
	if(QDir(QString(contpath.c_str())).exists())
	{
		std::vector<cv::Mat> imgs;
		for(int i=0;i<track->get_orig_images().size()-1;i++)
		{
			char num[5];
			sprintf(num,"%03d",i);
			string fpath=contpath+"/"+string(num)+PICFORMAT;
			imgs.push_back(cv::imread(fpath));
		}
		track->set_contours_images(imgs);
	}
	ui->CleanedRB->setEnabled(true);
	ui->ContoursRB->setEnabled(true);
	ui->ThreeDRB->setEnabled(true);
	ui->SaveProcButton->setEnabled(true);
	ui->PlusXButton->setEnabled(true);
	ui->PlusYButton->setEnabled(true);
	ui->PlusZButton->setEnabled(true);
	ui->MinusXButton->setEnabled(true);
	ui->MinusYButton->setEnabled(true);
	ui->MinusZButton->setEnabled(true);
	ui->loadModelButton->setEnabled(true);
	ui->showGreenBGBox->setEnabled(true);
	ui->ExportButton->setEnabled(true);
}
void MainWindow::save_data(){
	string fname=path+"/AnalyzedData.dat";
	QFile fout(QString(fname.c_str()));
	if(fout.exists()){
		QMessageBox msgBox;
		msgBox.setText("The data has already been saved");
		msgBox.exec();
		return;
	}
	save_vector(track->tracks,(char*)fname.c_str(),track->imgrows,track->imgcols,track->backgrounddepth());
}
void MainWindow::plusX(){
	ui->DisplayWidget3D->osgviewer->plusX();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::minusX(){
	ui->DisplayWidget3D->osgviewer->minusX();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::plusY(){
	ui->DisplayWidget3D->osgviewer->plusY();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::minusY(){
	ui->DisplayWidget3D->osgviewer->minusY();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::plusZ(){
	ui->DisplayWidget3D->osgviewer->plusZ();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::minusZ(){
	ui->DisplayWidget3D->osgviewer->minusZ();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelplusx(){
	ui->DisplayWidget3D->osgviewer->modelplusx(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelminusx(){
	ui->DisplayWidget3D->osgviewer->modelminusx(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelplusy(){
	ui->DisplayWidget3D->osgviewer->modelplusy(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelminusy(){
	ui->DisplayWidget3D->osgviewer->modelminusy(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelplusz(){
	ui->DisplayWidget3D->osgviewer->modelplusz(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelminusz(){
	ui->DisplayWidget3D->osgviewer->modelminusz(ui->scaleTextBox->text().toDouble());
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotplusx(){
	ui->DisplayWidget3D->osgviewer->modelrotplusx();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotminusx(){
	ui->DisplayWidget3D->osgviewer->modelrotminusx();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotplusy(){
	ui->DisplayWidget3D->osgviewer->modelrotplusy();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotminusy(){
	ui->DisplayWidget3D->osgviewer->modelrotminusy();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotplusz(){
	ui->DisplayWidget3D->osgviewer->modelrotplusz();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelrotminusz(){
	ui->DisplayWidget3D->osgviewer->modelrotminusz();
	ui->DisplayWidget3D->osgviewer->set_animationcount(current_frame+1);
	ui->DisplayWidget3D->osgviewer->allowonce();
}
void MainWindow::modelscale(){
	ui->DisplayWidget3D->osgviewer->modelscale(ui->scaleTextBox->text().toDouble());
}
void MainWindow::tractorplatecheckbox(bool value){
	ui->DisplayWidget3D->osgviewer->checktractorplate(value);
}
void MainWindow::loadmodel(){
	QString modelName = QFileDialog::getOpenFileName(this);
	string s;
	if (!modelName.isEmpty()){
		s=std::string(modelName.toLocal8Bit().constData());
		ui->DisplayWidget3D->osgviewer->load_model(s);
	}
	modelpath=s;

}
void MainWindow::savemodifiedmodel(){

	QString ss(modelpath.c_str());
	QStringList qs=ss.split(".");
	string s=qs.at(0).toLocal8Bit().constData()+string("_modified.")+qs.at(1).toLocal8Bit().constData();
	ui->DisplayWidget3D->osgviewer->save_model(s);
}
void MainWindow::removemodel(){
	ui->DisplayWidget3D->osgviewer->remove_model();
}
void MainWindow::exportvtk(){
	string fname=path+"/Export2VES";
	ui->DisplayWidget3D->osgviewer->export2vtk(fname);
}
void MyGLDrawer::drawImage(QImage& image)
{
	/*glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,num);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, 3, image.width(), image.height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,image.bits());*/
	QImage imagerotated=image.copy();
	imagerotated.scaled(QSize(this->width(),this->height()));
	buffer=imagerotated.mirrored();
	updateGL();
}
void MyGLDrawer::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*glTexCoord2f(0.0,0.0); glVertex2f(-1,-1);
	glTexCoord2f(0.0,1.0); glVertex2f(1,-1);
	glTexCoord2f(1.0,1.0); glVertex2f(1,1);
	glTexCoord2f(1.0,0.0); glVertex2f(-1,1);
	glDisable(GL_TEXTURE_2D);*/
	glDrawPixels(buffer.width(), buffer.height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer.bits());
}
