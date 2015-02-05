	/* neuronPicker_plugin.cpp
 * 
 * 2014-12-01 : by Xiang Li, Hanbo Chen
 */
 
#include "v3d_message.h"
#include <vector>
#include "neuronPicker_plugin.h"
#include <math.h>
#include "string"
#include "sstream"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <basic_landmark.h>
using namespace std;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#define const_length_histogram 90
#define const_max_voxelValue 255

static neuronPickerDialog * npdiag = 0;
void finishnpdiag()
{
    if (npdiag)
    {
        delete npdiag;
        npdiag=0;
    }
}

Q_EXPORT_PLUGIN2(neuronPicker, neuronPicker);

QStringList neuronPicker::menulist() const
{
	return QStringList() 
        <<tr("neuronPicker")
		<<tr("about");
}

QStringList neuronPicker::funclist() const
{
	return QStringList()
		<<tr("func1")
		<<tr("func2")
		<<tr("help");
}

void neuronPicker::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
	if (menu_name == tr("neuronPicker"))
	{
        if(npdiag){
            npdiag->show();
            npdiag->raise();
            npdiag->activateWindow();
        }else{
            npdiag = new neuronPickerDialog(&callback);
            npdiag->show();
            npdiag->raise();
            npdiag->activateWindow();
        }
    }
	else
	{
		v3d_msg(tr(". "
			"Developed by Xiang Li, Hanbo Chen, 2014-12-01"));
	}
}

bool neuronPicker::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	vector<char*> infiles, inparas, outfiles;
	if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
	if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
	if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);

	if (func_name == tr("func1"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("func2"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("help"))
	{
		v3d_msg("To be implemented.");
	}
	else return false;

	return true;
}

// func convert2UINT8
void neuronPickerDialog::convert2UINT8(unsigned short *pre1d, unsigned char *pPost, V3DLONG imsz)
{
	unsigned short* pPre = (unsigned short*)pre1d;
    double max_v=0, min_v = 4095;
	for(V3DLONG i=0; i<imsz; i++)
	{
		if(max_v<pPre[i]) max_v = pPre[i];
		if(min_v>pPre[i]) min_v = pPre[i];
	}
	max_v -= min_v;
	if(max_v>0)
	{
		for(V3DLONG i=0; i<imsz; i++)
		{
            pPost[i] = (unsigned char) 255.0*(double)(pPre[i] - min_v)/max_v;
		}
	}
	else
	{
		for(V3DLONG i=0; i<imsz; i++)
		{
			pPost[i] = (unsigned char) pPre[i];
		}
	}
}
void neuronPickerDialog::convert2UINT8(float *pre1d, unsigned char *pPost, V3DLONG imsz)
{
	float* pPre = (float*)pre1d;
	float max_v=0, min_v = 65535;
	for(V3DLONG i=0; i<imsz; i++)
	{
		if(max_v<pPre[i]) max_v = pPre[i];
		if(min_v>pPre[i]) min_v = pPre[i];
	}
	max_v -= min_v;
	if(max_v>0)
	{
		for(V3DLONG i=0; i<imsz; i++)
		{
            pPost[i] = (unsigned char) 255.0*(double)(pPre[i] - min_v)/(double)max_v;
		}
	}
	else
	{
		for(V3DLONG i=0; i<imsz; i++)
		{
			pPost[i] = (unsigned char) pPre[i];
		}
	}
}

//Hanbo
//dialog
neuronPickerDialog::neuronPickerDialog(V3DPluginCallback2 * cb)
{
    callback = cb;

    fname_input="";
    fname_outbase="";
    image1Dc_in=0;
    image1D_out=0;
    image1D_h=0;
    intype=0;
    sz_img[0]=sz_img[1]=sz_img[2]=sz_img[3]=0;
    LList.clear();

    creat();
    checkButtons();
}

void neuronPickerDialog::reject()
{
    //release memory
    if(npdiag){
        if(image1Dc_in != 0){
            delete []image1Dc_in; image1Dc_in=0;
        }
        if(image1D_out != 0){
            neuronPickerMain::memory_free_uchar1D(image1D_out);
            image1D_out=0;
        }
        if(image1D_h != 0){
            neuronPickerMain::memory_free_int1D(image1D_h);
            image1D_h=0;
        }
        if(image1D_v != 0){
            neuronPickerMain::memory_free_uchar1D(image1D_v);
            image1D_v=0;
        }
    }
    finishnpdiag();
}

void neuronPickerDialog::creat()
{
    QGridLayout *gridLayout = new QGridLayout();

    //I/O zone
    QLabel* label_load = new QLabel(QObject::tr("Input Image:"));
    gridLayout->addWidget(label_load,0,0,1,1);
    edit_load = new QLineEdit();
    edit_load->setText(""); edit_load->setReadOnly(true);
    gridLayout->addWidget(edit_load,0,1,1,4);
    btn_load = new QPushButton("...");
    gridLayout->addWidget(btn_load,0,5,1,1);

    QLabel* label_output = new QLabel(QObject::tr("Output Prefix:"));
    gridLayout->addWidget(label_output,1,0,1,1);
    edit_output = new QLineEdit();
    edit_output->setText(""); edit_output->setReadOnly(true);
    gridLayout->addWidget(edit_output,1,1,1,4);
    btn_output = new QPushButton("...");
    gridLayout->addWidget(btn_output,1,5,1,1);

    connect(btn_load, SIGNAL(clicked()), this, SLOT(load()));
    connect(btn_output, SIGNAL(clicked()), this, SLOT(output()));

    //marker zone
    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    line_1->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_1,4,0,1,6);


    btn_update = new QPushButton("Load Markers =>");
    gridLayout->addWidget(btn_update,5,0,1,1);
    cb_marker = new QComboBox();
    gridLayout->addWidget(cb_marker,5,1,1,1);
    btn_extract = new QPushButton("Extract Neuron");
    gridLayout->addWidget(btn_extract,5,2,1,1);
    btn_save = new QPushButton("Accept, Save, and Next");
    gridLayout->addWidget(btn_save,5,3,1,2);
    btn_next = new QPushButton("Reject and Next");
    gridLayout->addWidget(btn_next,5,5,1,1);

    connect(btn_update, SIGNAL(clicked()), this, SLOT(loadMarkers()));
    connect(btn_extract, SIGNAL(clicked()), this, SLOT(extract()));
    connect(btn_save, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(btn_next, SIGNAL(clicked()), this, SLOT(skip()));
    connect(cb_marker, SIGNAL(currentIndexChanged(int)), this, SLOT(syncMarkers(int)));

    //extract parameters
    spin_bgthr = new QSpinBox();
    spin_bgthr->setRange(0,255); spin_bgthr->setValue(20);
    spin_distance = new QSpinBox();
    spin_distance->setRange(0,100000); spin_distance->setValue(5);
    spin_huedis = new QSpinBox();
    spin_huedis->setRange(0,359); spin_huedis->setValue(40);
    QLabel* label_0 = new QLabel("background threshold (0~255):");
    gridLayout->addWidget(label_0,6,0,1,2);
    gridLayout->addWidget(spin_bgthr,6,2,1,1);
    QLabel* label_1 = new QLabel("neighbor(cubic) mask size: ");
    gridLayout->addWidget(label_1,6,3,1,2);
    gridLayout->addWidget(spin_distance,6,5,1,1);
    QLabel* label_2 = new QLabel("hue distance threshold (0~360): ");
    gridLayout->addWidget(label_2,7,0,1,2);
    gridLayout->addWidget(spin_huedis,7,2,1,1);

    //other
    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_2,10,0,1,6);

    btn_quit = new QPushButton("Quit");
    gridLayout->addWidget(btn_quit,11,5,1,1);

    connect(btn_quit, SIGNAL(clicked()), this, SLOT(reject()));

    setLayout(gridLayout);
}

bool neuronPickerDialog::load()
{
    fname_input = QFileDialog::getOpenFileName(0, QObject::tr("Choose the input image "),
                                               QDir::currentPath(),
                                               QObject::tr("Images (*.raw *.tif *.lsm *.v3dpbd *.v3draw);;All(*)"));
    if(fname_input.isEmpty()){
        return false;
    }

    qDebug()<<"NeuronPicker: is going to load "<<fname_input;
    qDebug()<<"NeuronPicker: reset environment";
    {//reset previous content first
        if(image1Dc_in != 0){
            delete []image1Dc_in; image1Dc_in=0;
        }
        if(image1D_out != 0){
            neuronPickerMain::memory_free_uchar1D(image1D_out);
            image1D_out=0;
        }
        if(image1D_h != 0){
            neuronPickerMain::memory_free_int1D(image1D_h);
            image1D_h=0;
        }
        if(image1D_v != 0){
            neuronPickerMain::memory_free_uchar1D(image1D_v);
            image1D_v=0;
        }
        fname_outbase="";
        intype=0;
        sz_img[0]=sz_img[1]=sz_img[2]=sz_img[3]=0;
        LList.clear();
        cb_marker->clear();
        qDebug()<<"NeuronPicker: reset input window";
    }

    qDebug()<<"NeuronPicker: load new image";
    if(!simple_loadimage_wrapper(*callback, fname_input.toStdString().c_str(), image1Dc_in, sz_img, intype))
    {
      v3d_msg("load image "+fname_input+" error!");
      return false;
    }
    qDebug()<<"\t>>read image file "<< fname_input <<" complete.";
    qDebug()<<"\t\timage size: [w="<<sz_img[0]<<", h="<<sz_img[1]<<", z="<<sz_img[2]<<", c="<<sz_img[3]<<"]";
    qDebug()<<"\t\tdatatype: "<<intype;

    if(sz_img[3]<3){
        v3d_msg("Currently this program only supports image with 3 channels (RGB).", 0);
        return false;
    }else if(sz_img[3]>3){
        sz_img[3]=3;
        qDebug()<<"More than 3 channels were loaded. The first 3 channel will be applied for analysis.";
    }

    V3DLONG size_tmp=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
    if(intype!=1)
    {
        if (intype == 2) //V3D_UINT16;
        {
            neuronPickerDialog::convert2UINT8((unsigned short*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else if(intype == 4) //V3D_FLOAT32;
        {
            neuronPickerDialog::convert2UINT8((float*)image1Dc_in, image1Dc_in, size_tmp);
        }
        else
        {
            v3d_msg("Currently this program only supports UINT8, UINT16, and FLOAT32 data type.", 0);
            return false;
        }
    }

    fname_outbase=fname_input+".extract";
    edit_load->setText(fname_input);
    edit_output->setText(fname_outbase);

    qDebug()<<"NeuronPicker: update visualization windows";
    updateInputWindow();
    updateOutputWindow();
    checkButtons();

    //initialize other stuffs for calculation
    qDebug()<<"NeuronPicker: initialize veriables";
    V3DLONG size_page=sz_img[0]*sz_img[1]*sz_img[2];
    image1D_out=neuronPickerMain::memory_allocate_uchar1D(size_page);
    image1D_h=neuronPickerMain::memory_allocate_int1D(size_page);
    image1D_v=neuronPickerMain::memory_allocate_uchar1D(size_page);
    neuronPickerMain::initChannels_rgb(image1Dc_in,image1D_h,image1D_v, sz_img, spin_bgthr->value());
    preBgthr=spin_bgthr->value();
}

void neuronPickerDialog::output()
{
    QString fname_output = QFileDialog::getSaveFileName(0, QObject::tr("Choose the output folder and prefix"),
                                               fname_outbase,
                                               "");

    if(!fname_output.isEmpty()){
        fname_outbase=fname_output;
    }
    edit_output->setText(fname_outbase);
    checkButtons();
}

int neuronPickerDialog::loadMarkers()
{
    LList.clear();
    QStringList cb_items;
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
        {
            LandmarkList LList_in = callback->getLandmark(v3dhandleList_current[i]);
            for(int i=0; i<LList_in.size(); i++){
                LList.append(LList_in.at(i));
                LList[i].color.r=196;
                LList[i].color.g=LList[i].color.b=0;
                cb_items.append("marker: " + QString::number(i+1));
            }

            break;
        }
    }
    poss_landmark=neuronPickerMain::landMarkList2poss(LList, sz_img[0], sz_img[0]*sz_img[1]);
    cb_marker->clear();
    cb_marker->addItems(cb_items);
    if(LList.size()>0){
        extract();
    }
    checkButtons();
    return(LList.size());
}

void neuronPickerDialog::syncMarkers(int c)
{
    qDebug()<<"cojoc: syncronize markers";
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
        {
            for(int i=0; i<LList.size(); i++){
                if(i==cb_marker->currentIndex()){
                    LList[i].color.b=0;
                }else{
                    LList[i].color.b=196;
                }
            }
            callback->setLandmark(v3dhandleList_current[i],LList);
            callback->pushObjectIn3DWindow(v3dhandleList_current[i]);
        }
    }
}

void neuronPickerDialog::extract()
{
    if(LList.size()<1 || cb_marker->count()<1 || cb_marker->currentIndex()+1>LList.size() ){
        return;
    }
    if(preBgthr != spin_bgthr->value()){
        neuronPickerMain::initChannels_rgb(image1Dc_in,image1D_h,image1D_v, sz_img, spin_bgthr->value());
        preBgthr=spin_bgthr->value();
    }
    int idx_landmark=cb_marker->currentIndex();
    V3DLONG pos_landmark=poss_landmark[idx_landmark];
    memset(image1D_out, 0, sz_img[0]*sz_img[1]*sz_img[2]*sizeof(unsigned char));
    qDebug()<<"start extracting";
//    V3DLONG size_page = sz_img[0]*sz_img[1]*sz_img[2];
    neuronPickerMain::extract(image1D_h, image1D_v, image1D_out, pos_landmark, spin_distance->value(), spin_huedis->value(), sz_img);
    //visualizationImage1D(image1D_tmp, sz_img[0], sz_img[1], sz_img[2], 1, *call, QString(name_currentWindow+QString("%1").arg(idx_landmark)));
    qDebug()<<"push for visualization";
    updateOutputWindow();
}

void neuronPickerDialog::saveFile()
{
    int idx_landmark=cb_marker->currentIndex();
    V3DLONG sz_img_sub[4];
    sz_img_sub[0]=sz_img[0];
    sz_img_sub[1]=sz_img[1];
    sz_img_sub[2]=sz_img[2];
    sz_img_sub[3]=1;
    QString fname_output=fname_outbase + "_" + QString::number(LList.at(idx_landmark).x) + "_" +
            QString::number(LList.at(idx_landmark).y) + "_" + QString::number(LList.at(idx_landmark).z) + ".v3dpbd";
    if(!simple_saveimage_wrapper(*callback, qPrintable(fname_output),image1D_out,sz_img_sub,1)){
        v3d_msg("failed to save file to " + fname_output);
        return;
    }else{
        qDebug()<<"NeuronPicker: "<<fname_output;
    }
    LList[idx_landmark].color.r = 0;
    LList[idx_landmark].color.g = 255;
    if(idx_landmark+1<cb_marker->count()){
        cb_marker->setCurrentIndex(idx_landmark+1);
        extract();
    }else{
        v3d_msg("Reach the end of marker list.");
    }
    checkButtons();
}

void neuronPickerDialog::skip()
{
    int idx_landmark=cb_marker->currentIndex();
    if(idx_landmark+1<cb_marker->count()){
        cb_marker->setCurrentIndex(idx_landmark+1);
        extract();
    }else{
        v3d_msg("Reach the end of marker list.");
    }
    checkButtons();
}


void neuronPickerDialog::checkButtons()
{
    if(image1Dc_in==0){
        btn_update->setEnabled(false);
        btn_extract->setEnabled(false);
        btn_save->setEnabled(false);
        btn_next->setEnabled(false);
    }else{
        bool winfound=false;
        v3dhandleList v3dhandleList_current=callback->getImageWindowList();
        for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
        {
            if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
            {
                winfound=true;
            }
        }
        if(winfound){
            btn_update->setEnabled(true);
        }
        if(cb_marker->count()>0){
            btn_extract->setEnabled(true);
            if(fname_outbase.isEmpty() || image1D_out==0){
                btn_save->setEnabled(false);
                btn_next->setEnabled(false);
            }else{
                btn_save->setEnabled(true);
                btn_next->setEnabled(true);
            }
        }else{
            btn_extract->setEnabled(false);
            btn_next->setEnabled(false);
            btn_save->setEnabled(false);
        }
    }
}

void neuronPickerDialog::updateInputWindow()
{
    //search in open windows
    bool winfound=false;
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
        {
            winfound=true;
        }
    }
    if(image1Dc_in != 0){ //image loaded
        //generate a copy and show it
        qDebug()<<"cojoc: image4d: "<<sz_img[0]<<":"<<sz_img[1]<<":"<<sz_img[2]<<":"<<sz_img[3];
        Image4DSimple image4d;
        image4d.setFileName(fname_input.toStdString().c_str());
        V3DLONG size_page = sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
        unsigned char* image1D_input=neuronPickerMain::memory_allocate_uchar1D(size_page);
        memcpy(image1D_input, image1Dc_in, size_page*sizeof(unsigned char));
        image4d.setData(image1D_input, sz_img[0], sz_img[1], sz_img[2], sz_img[3], V3D_UINT8);
        if(!winfound){ //open a window if none is open
            qDebug()<<"cojoc: creat window";
            v3dhandle v3dhandle_main=callback->newImageWindow();
            //update the image
            callback->setImage(v3dhandle_main, &image4d);
            callback->setImageName(v3dhandle_main, NAME_INWIN);
            callback->updateImageWindow(v3dhandle_main);
            callback->open3DWindow(v3dhandle_main);
        }else{
            qDebug()<<"cojoc: refresh content";
            v3dhandleList_current=callback->getImageWindowList();
            for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
            {
                if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
                {
                    //update the image
                    if(!callback->setImage(v3dhandleList_current[i], &image4d)){
                        v3d_msg("Failed to update input image");
                    }
                    callback->updateImageWindow(v3dhandleList_current[i]);
                    //callback->open3DWindow(v3dhandleList_current[i]);
                    callback->pushImageIn3DWindow(v3dhandleList_current[i]);
                }
            }
        }
    }else if(winfound){
        //no image loaded, but there is a window? clean up the contents
        //clean up landmarks
        LandmarkList LList_empty;
        //clean up window
        Image4DSimple image4d;
        unsigned char *image1D_input=neuronPickerMain::memory_allocate_uchar1D(1);
        image1D_input[0]=0;
        image4d.setData(image1D_input,1,1,1,1,V3D_UINT8);
        v3dhandleList_current=callback->getImageWindowList();
        for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
        {
            if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_INWIN))
            {
                callback->setLandmark(v3dhandleList_current[i], LList_empty);
                callback->setImage(v3dhandleList_current[i], &image4d);
                callback->updateImageWindow(v3dhandleList_current[i]);
                callback->pushImageIn3DWindow(v3dhandleList_current[i]);
            }
        }
    }
}

void neuronPickerDialog::updateOutputWindow()
{
    //search in open windows
    bool winfound=false;
    v3dhandleList v3dhandleList_current=callback->getImageWindowList();
    for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
    {
        if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
        {
            winfound=true;
        }
    }
    if(image1D_out != 0){ //image loaded
        //generate a copy and show it
        Image4DSimple image4d;
        V3DLONG size_page = sz_img[0]*sz_img[1]*sz_img[2];
        unsigned char* image1D_input=neuronPickerMain::memory_allocate_uchar1D(size_page);
        memcpy(image1D_input, image1D_out, size_page*sizeof(unsigned char));
        image4d.setData(image1D_input, sz_img[0], sz_img[1], sz_img[2], 1, V3D_UINT8);

        if(!winfound){ //open a window if none is open
            v3dhandle v3dhandle_main=callback->newImageWindow();
            callback->setImage(v3dhandle_main, &image4d);
            callback->setImageName(v3dhandle_main, NAME_OUTWIN);
            callback->updateImageWindow(v3dhandle_main);
            callback->open3DWindow(v3dhandle_main);
        }else{
            //update the image
            for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
            {
                if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
                {
                    callback->setImage(v3dhandleList_current[i], &image4d);
                    callback->updateImageWindow(v3dhandleList_current[i]);
                    //callback->open3DWindow(v3dhandleList_current[i]);
                    callback->pushImageIn3DWindow(v3dhandleList_current[i]);
                }
            }
        }
    }else if(winfound){
        //no image loaded, but there is a window? clean up the window content
        Image4DSimple image4d;
        unsigned char *image1D_input=neuronPickerMain::memory_allocate_uchar1D(1);
        image1D_input[0]=0;
        image4d.setData(image1D_input,1,1,1,1,V3D_UINT8);
        for (V3DLONG i=0;i<v3dhandleList_current.size();i++)
        {
            if(callback->getImageName(v3dhandleList_current[i]).contains(NAME_OUTWIN))
            {
                callback->setImage(v3dhandleList_current[i], &image4d);
                callback->updateImageWindow(v3dhandleList_current[i]);
                //callback->close3DWindow(v3dhandleList_current[i]);
                callback->pushImageIn3DWindow(v3dhandleList_current[i]);
            }
        }
    }
}

unsigned char * neuronPickerMain::memory_allocate_uchar1D(const V3DLONG i_size)
{
    unsigned char *ptr_result;
    ptr_result=(unsigned char *) calloc(i_size, sizeof(unsigned char));
    return(ptr_result);
}
void neuronPickerMain::memory_free_uchar1D(unsigned char *ptr_input)
{
    free(ptr_input);
}
int * neuronPickerMain::memory_allocate_int1D(const V3DLONG i_size)
{
    int *ptr_result;
    ptr_result=(int *) calloc(i_size, sizeof(int));
    return(ptr_result);
}
void neuronPickerMain::memory_free_int1D(int *ptr_input)
{
    free(ptr_input);
}
vector<V3DLONG> neuronPickerMain::landMarkList2poss(LandmarkList LandmarkList_input, V3DLONG _offset_Y, V3DLONG _offest_Z)
{
    vector<V3DLONG> poss_result;
    V3DLONG count_landmark=LandmarkList_input.count();
    for (V3DLONG idx_input=0;idx_input<count_landmark;idx_input++)
    {
        poss_result.push_back(landMark2pos(LandmarkList_input.at(idx_input), _offset_Y, _offest_Z));
    }
    return poss_result;
}
V3DLONG neuronPickerMain::landMark2pos(LocationSimple Landmark_input, V3DLONG _offset_Y, V3DLONG _offset_Z)
{
    float x=0;
    float y=0;
    float z=0;
    Landmark_input.getCoord(x, y, z);
    return (xyz2pos(x-1, y-1, z-1, _offset_Y, _offset_Z));
}
vector<V3DLONG> neuronPickerMain::pos2xyz(const V3DLONG _pos_input, const V3DLONG _offset_Y, const V3DLONG _offset_Z)
{
    vector<V3DLONG> pos3_result (3, -1);
    pos3_result[2]=floor(_pos_input/(double)_offset_Z);
    pos3_result[1]=floor((_pos_input-pos3_result[2]*_offset_Z)/(double)_offset_Y);
    pos3_result[0]=_pos_input-pos3_result[2]*_offset_Z-pos3_result[1]*_offset_Y;
    return pos3_result;
}
V3DLONG neuronPickerMain::xyz2pos(const V3DLONG _x, const V3DLONG _y, const V3DLONG _z, const V3DLONG _offset_Y, const V3DLONG _offset_Z)
{
    return _z*_offset_Z+_y*_offset_Y+_x;
}

void neuronPickerMain::initChannels_rgb(unsigned char *image1Dc, int *image1D_h,unsigned char *image1D_v, V3DLONG sz_img[4], const int bg_thr)
{
    memset(image1D_h, -1, sz_img[0]*sz_img[1]*sz_img[2]*sizeof(int));
    V3DLONG page=sz_img[0]*sz_img[1]*sz_img[2];

    //rgb 2 hsv, obtain foreground and histogram
    vector<V3DLONG> hhist(const_length_histogram,0);
    double hist_scale=360/const_length_histogram;
    for(V3DLONG idx=0; idx<page; idx++){
        unsigned char V=MAX(image1Dc[idx],image1Dc[idx+page]);
        V=MAX(V,image1Dc[idx+page*2]);
        image1D_v[idx]=V;
        //filter by value
        if(V<(unsigned char)bg_thr){
            continue;
        }
        //get hue
        image1D_h[idx]=rgb2hue(image1Dc[idx], image1Dc[idx+page], image1Dc[idx+page*2]);
        if(image1D_h[idx]>=0){
            //histogram of hue
            int hbin=image1D_h[idx]/hist_scale;
            hbin=MIN(hbin,const_length_histogram-1);
            hhist[hbin]++;
        }else{//this should not happen
            qDebug()<<"hue is smaller than 0:"<<image1D_h[idx];
        }
    }

    //shift the histogram
    V3DLONG hmin_v=hhist[0];
    int idx_shift=0;
    for(int i=1; i<const_length_histogram-1; i++){
        if(hmin_v>(hhist[i]+hhist[i-1]+hhist[i+1])){
            hmin_v=hhist[i]+hhist[i-1]+hhist[i+1];
            idx_shift=i;
        }
    }

    //shift the hue
    int hue_shift=idx_shift*hist_scale;
    for(V3DLONG idx=0; idx<page; idx++){
        if(image1D_h[idx]<0){
            continue;
        }
        if(image1D_h[idx]>=hue_shift){
            image1D_h[idx]-=hue_shift;
        }else{
            image1D_h[idx]+=360-hue_shift;
        }
    }
}

void neuronPickerMain::extract(int *image1D_h, unsigned char *image1D_v, unsigned char *image1D_out, V3DLONG _pos_input, int cubSize, int colorSpan, V3DLONG sz_img[4])
{
    vector<V3DLONG> seeds;
    V3DLONG delta=cubSize/2;
    V3DLONG v_mean=0, v_count=0;
    V3DLONG x,y,z,pos;
    V3DLONG y_offset=sz_img[0];
    V3DLONG z_offset=sz_img[0]*sz_img[1];
    vector<V3DLONG> coord;
    coord=pos2xyz(_pos_input, y_offset, z_offset);
    x=coord[0];
    y=coord[1];
    z=coord[2];
    qDebug()<<"==========NeuronPicker: seed:"<<x<<":"<<y<<":"<<z<<":"<<delta;
    //find the average hue in seed regions
    for(V3DLONG dx=MAX(x-delta,0); dx<=MIN(sz_img[0]-1,x+delta); dx++){
        for(V3DLONG dy=MAX(y-delta,0); dy<=MIN(sz_img[1]-1,y+delta); dy++){
            for(V3DLONG dz=MAX(z-delta,0); dz<=MIN(sz_img[2]-1,z+delta); dz++){
                pos=xyz2pos(dx,dy,dz,y_offset,z_offset);
//                qDebug()<<"==========NeuronPicker: "<<dx<<":"<<dy<<":"<<dz<<":"<<image1D_h[pos];
                if(image1D_h[pos]<0)    continue;
                v_mean+=image1D_h[pos];
                v_count++;
            }
        }
    }
    if(v_count<=0){
        qDebug()<<"==========NeuronPicker: an empty region!";
        return;
    }
    v_mean/=v_count;

    unsigned char* image1D_mask=memory_allocate_uchar1D(sz_img[0]*sz_img[1]*sz_img[2]);
    memset(image1D_mask, 0, sz_img[0]*sz_img[1]*sz_img[2]*sizeof(unsigned char));

    //populate the init seed regions
    for(V3DLONG dx=MAX(x-delta,0); dx<=MIN(sz_img[0]-1,x+delta); dx++){
        for(V3DLONG dy=MAX(y-delta,0); dy<=MIN(sz_img[1]-1,y+delta); dy++){
            for(V3DLONG dz=MAX(z-delta,0); dz<=MIN(sz_img[2]-1,z+delta); dz++){
                pos=xyz2pos(dx,dy,dz,y_offset,z_offset);
//                qDebug()<<"==========NeuronPicker: "<<dx<<":"<<dy<<":"<<dz<<":"<<image1D_h[pos]<<":"<<v_mean;
                if(image1D_h[pos]<0)    continue;
                if(MAX(image1D_h[pos],v_mean)-MIN(image1D_h[pos],v_mean)>colorSpan) continue;
                image1D_out[pos]=image1D_v[pos];
                image1D_mask[pos]=1;
                seeds.push_back(pos);
            }
        }
    }

    qDebug()<<"==========NeuronPicker: found "<<seeds.size()<<" initial seeds";

    //seed grow
    V3DLONG sid=0;
    while(sid<seeds.size()){
        coord=pos2xyz(seeds[sid], y_offset, z_offset);
        x=coord[0];
        y=coord[1];
        z=coord[2];
        //find the average hue in seed regions
        for(V3DLONG dx=MAX(x-delta,0); dx<=MIN(sz_img[0]-1,x+delta); dx++){
            for(V3DLONG dy=MAX(y-delta,0); dy<=MIN(sz_img[1]-1,y+delta); dy++){
                for(V3DLONG dz=MAX(z-delta,0); dz<=MIN(sz_img[2]-1,z+delta); dz++){
                    pos=xyz2pos(dx,dy,dz,y_offset,z_offset);
                    if(image1D_h[pos]<0)    continue;
                    if(image1D_mask[pos]>0) continue;
                    if(MAX(image1D_h[pos],v_mean)-MIN(image1D_h[pos],v_mean)>colorSpan) continue;
                    image1D_out[pos]=image1D_v[pos];
                    image1D_mask[pos]=1;
                    seeds.push_back(pos);
                }
            }
        }
        sid++;
    }

    qDebug()<<"=========NeuronPicker: finally found "<<seeds.size()<<" voxels";

    memory_free_uchar1D(image1D_mask);
}

int neuronPickerMain::rgb2hue(const unsigned char R, const unsigned char G, const unsigned char B)
{
    float r = (float)R/255.0;
    float g = (float)G/255.0;
    float b = (float)B/255.0;
    float maxColor=-1, minColor=1e6;
    int maxChannel=-1;
    int hue=-1;
    if(maxColor<r){
        maxColor=r;
        maxChannel=0;
    }
    if(maxColor<g){
        maxColor=g;
        maxChannel=1;
    }
    if(maxColor<b){
        maxColor=b;
        maxChannel=2;
    }
    if(minColor>r){
        minColor=r;
    }
    if(minColor>g){
        minColor=g;
    }
    if(minColor>b){
        minColor=b;
    }

    if(maxChannel==0){
        float tmp=(g-b)/(maxColor-minColor);
        if(tmp<0)
            tmp=tmp+6;
        hue=60*tmp;
    }
    if(maxChannel==1){
        hue=60.0*(((b-r)/(maxColor-minColor))+2.0);
    }
    if(maxChannel==2){
        hue=60.0*(((r-g)/(maxColor-minColor))+4.0);
    }
    if(maxColor-minColor<1e-5)
        hue=-1;
    return hue;
}
