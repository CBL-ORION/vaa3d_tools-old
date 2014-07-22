/* typeset_plugin.cpp
 * Typeset child brances based on node marker
 * 2014-06-26 : by Surobhi Ganguly
 */
 
#include "v3d_message.h"
#include <v3d_interface.h>
#include "my_surf_objs.h"
#include "openSWCDialog.h"
//#include "sort_func.h"
//#include "sort_swc.h"
//#include "typeset.h"
#include "typeset_func.h"
#include "typeset_plugin.h"


//#include <vector>
//#include <stdio.h>
//#include <stdlib.h>
//#include <QAbstractSlider>
//#include <iostream>
//#include <sstream>
#include <QObject>

//using namespace std;

Q_EXPORT_PLUGIN2(typeset, TypesetPlugin)

//controlPanel* controlPanel::panel = 0;
static controlPanel *panel = 0;

void TypesetWindow(V3DPluginCallback2 &v3d, QWidget *parent);

#define MYFLOAT double

void finishSyncPanel()
{
    if (panel)
    {
        delete panel;
        panel=0;
    }
}

QStringList TypesetPlugin::menulist() const
{
    return QStringList()
        <<tr("typeset")
        <<tr("directions")
        <<tr("about");
}

QStringList TypesetPlugin::funclist() const
{
    return QStringList()
        <<tr("typeset_swc")
        <<tr("help");
}

void TypesetPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("typeset"))
    {
        TypesetWindow(callback,parent);
        //typeset_swc(callback,parent);
    }
//    else if (menu_name == tr("directions"))
//    {
//        v3d_msg(tr("1. Trace neuron. \n"
//                   "2. Create marker files. Each marker file will set each branch type.\n"
//                   "3. Run plugin and follow directions in opening swc and marker files. "));
//    }
    else
    {
        v3d_msg(tr("This plugin sets branch type of traced neuron \n"
            "Developed by Surobhi Ganguly, 7/2014"));
    }
}

bool TypesetPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    if (func_name == tr("typeset_swc"))
    {
        return typeset_swc(input, output);
    }
    else if (func_name == tr("TOOLBOXtypeset"))
    {
        typeset_swc_toolbox(input);
        return true;
    }
    if (func_name == tr("help"))
    {
        printf("\n\n(version 1.0) typeset points in a swc file indicate beginning of branch of type to be set. Developed by Surobhi Ganguly 2014-07-14\n");
        printf("usage:\n");
        printf("\t-f <function_name>:     typeset_swc\n");
        printf("\t-i <input_file_name>:   input .swc\n");
        printf("\t-o <output_file_name>:  (not required) typesetd swc file. DEFAULT: 'inputName_typesetd.swc'\n");
        printf("\t-p <step_length>:       step length for resampling.\n");
        printf("Demo: v3d -x typeset_swc -f typeset_swc -i test.swc -o test_typesetd.swc -p 1\n\n");
    }
    return false;
}


void TypesetWindow(V3DPluginCallback2 &v3d, QWidget *parent)
{
    if (panel)
    {
        panel->show();
        return;
    }
    else
    {
        panel = new controlPanel(v3d, parent); //this shouldn't happen.

        if (panel)
        {
            panel->show();
            panel->raise();
            panel->move(100,100);
            panel->activateWindow();
        }
    }
//    if (controlPanel::panel)
//    {
//        controlPanel::panel->show();
//        return;
//    }
//    else
//    {
//        controlPanel* p = new controlPanel(v3d, parent);
//        //controlPanel* p = new controlPanel(v3d, parent);
//        if (p)
//        {
//     //       panel->setAttribute(Qt::WA_QuitOnClose);
//      //      panel->setAttribute(Qt::WA_DeleteOnClose);
//            p->show();
//            p->raise();
//            p->move(50,50);
//            p->activateWindow();
//        }
//    }
}

void MyComboBox::enterEvent(QEvent *e)
{
    updateList();
    QComboBox::enterEvent(e);
}

void MyComboBox::updateList()
{
    if (!m_v3d)
        return;

    //need to add a few more lines to get the current highligh file and then always highlight it unless literally changed
    QString lastDisplayfile = currentText();

    // now re-check the currently opened windows
    v3dhandleList cur_list_triview = m_v3d->getImageWindowList();
    QList <V3dR_MainWindow *> cur_list_3dviewer = m_v3d->getListAll3DViewers();

    QStringList items;
    int i;

    for (i=0; i<cur_list_triview.size(); i++)
        items << m_v3d->getImageName(cur_list_triview[i]);

    for (i=0; i<cur_list_3dviewer.count(); i++)
    {
        QString curname = m_v3d->getImageName(cur_list_3dviewer[i]).remove("3D View [").remove("]");
        bool b_found=false;
        for (int j=0; j<cur_list_triview.size(); j++)
            if (curname==m_v3d->getImageName(cur_list_triview[j]))
            {
                b_found=true;
                break;
            }

        if (!b_found)
            items << m_v3d->getImageName(cur_list_3dviewer[i]);
    }

    //update the list now
    clear();
    addItems(items);

    //search if the lastDisplayfile exists, if yes, then highlight it (set as current),
    //otherwise do nothing (i.e. in this case the list will highlight the 1st one which is new)

    int curDisplayIndex=-1; //-1 for invalid index
    for (i=0; i<items.size(); i++)
        if (items[i]==lastDisplayfile)
        {
            curDisplayIndex = i;
            break;
        }

    if (curDisplayIndex>=0)
        setCurrentIndex(curDisplayIndex);

    //
    update();

    return;
}


controlPanel::controlPanel(V3DPluginCallback2 &_v3d, QWidget *parent) :
    QDialog(parent), m_v3d(_v3d)
{
    QPushButton* btn_Record = new QPushButton("Add an Anchor Point");
    QPushButton* btn_Preview = new QPushButton("Preview and Save Movie");
    QPushButton* btn_Show = new QPushButton("Show Selected Anchor Point");
    QPushButton* btn_Delete = new QPushButton("Delete Selected Anchor Point");
    QPushButton* btn_Upload = new QPushButton("Upload to Youtube");
    QPushButton* btn_Save = new QPushButton("Save Anchor-point file");
    QPushButton* btn_Load = new QPushButton("Load Anchor-point file");
    QPushButton* btn_Up = new QPushButton("Move Up");
    QPushButton* btn_Down = new QPushButton("Move Down");

    box_SampleRate = new QSpinBox();
    QLabel* SampleName = new QLabel(QObject::tr("Sampling Rate:"));

    //potential bugs for the following two sentences
    list_triview = m_v3d.getImageWindowList();
    list_3dviewer = m_v3d.getListAll3DViewers();

    combo_surface = new MyComboBox(&m_v3d);
    combo_surface->updateList();

    label_surface = new QLabel(QObject::tr("Window List: "));

    gridLayout = new QGridLayout();
    gridLayout->addWidget(label_surface, 1,0,1,5);
    gridLayout->addWidget(combo_surface, 2,0,1,5);
    gridLayout->addWidget(btn_Record, 8,0,1,2);
    gridLayout->addWidget(btn_Up, 8,2,1,1);
    gridLayout->addWidget(btn_Down, 8,4,1,1);
    gridLayout->addWidget(btn_Preview,5,6,1,3);
    gridLayout->addWidget(btn_Show,10,0,1,2);
    gridLayout->addWidget(btn_Delete,9,0,1,2);
    gridLayout->addWidget(btn_Save,9,2,1,3);
    gridLayout->addWidget(btn_Load,10,2,1,3);
    gridLayout->addWidget(btn_Upload,11,0,1,5);
    gridLayout->addWidget(SampleName, 4,6,1,1);
    gridLayout->addWidget(box_SampleRate, 4,7,1,2);

    box_SampleRate->setMaximum(1000);
    box_SampleRate->setMinimum(2);
    box_SampleRate->setValue(30);

    list_anchors = new QListWidget();
    gridLayout->addWidget(list_anchors,3,0,5,5);

    setLayout(gridLayout);
    setWindowTitle(QString("ZMovieMaker"));

    connect(btn_Show, SIGNAL(clicked()), this, SLOT(_slot_show()));
    connect(btn_Delete, SIGNAL(clicked()), this, SLOT(_slot_delete()));
    connect(btn_Record, SIGNAL(clicked()), this, SLOT(_slot_record()));
    connect(btn_Preview, SIGNAL(clicked()), this, SLOT(_slot_preview()));
    connect(btn_Save, SIGNAL(clicked()), this, SLOT(_slot_save()));
    connect(btn_Load, SIGNAL(clicked()), this, SLOT(_slot_load()));
    connect(btn_Upload, SIGNAL(clicked()), this, SLOT(_slot_upload()));
    connect(box_SampleRate, SIGNAL(valueChanged(double)), this, SLOT(update()));
    connect(btn_Up, SIGNAL(clicked()), this, SLOT(_slot_up()));
    connect(btn_Down, SIGNAL(clicked()), this, SLOT(_slot_down()));


    connect(list_anchors, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(_slot_show_item(QListWidgetItem *)));
}

controlPanel::~controlPanel()
{
     if(panel){delete panel; panel=0;}

}


//void controlPanel::_slot_record()
//{
//    CHECK_WINDOWS;
//    view->absoluteRotPose();
//    MYFLOAT xRot = view->xRot();
//    MYFLOAT yRot = view->yRot();
//    MYFLOAT zRot = view->zRot();
//    MYFLOAT xShift = view->xShift();
//    MYFLOAT yShift = view->yShift();
//    MYFLOAT zShift = view->zShift();
//    MYFLOAT zoom = view->zoom();
//    MYFLOAT xCut0 = view->xCut0();
//    MYFLOAT xCut1 = view->xCut1();
//    MYFLOAT yCut0 = view->yCut0();
//    MYFLOAT yCut1 = view->yCut1();
//    MYFLOAT zCut0 = view->zCut0();
//    MYFLOAT zCut1 = view->zCut1();
//    MYFLOAT frontCut = view->frontCut();
//    bool  channelB = view->channelB();
//    bool  channelR = view->channelR();
//    bool  channelG = view->channelG();
//    int   showSurf = view->isShowSurfObjects();
//    int xClip0 = view->xClip0();
//    int xClip1 = view->xClip1();
//    int yClip0 = view->yClip0();
//    int yClip1 = view->yClip1();
//    int zClip0 = view->zClip0();
//    int zClip1 = view->zClip1();
//    int timePoint = view->volumeTimePoint();

//    QString curstr = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25").arg(xRot).arg(yRot).arg(zRot).arg(xShift).arg(yShift).arg(zShift).arg(zoom).arg(xCut0).arg(xCut1).arg(yCut0).arg(yCut1).arg(zCut0).arg(zCut1).arg(channelR).arg(channelG).arg(channelB).arg(showSurf).arg(xClip0).arg(xClip1).arg(yClip0).arg(yClip1).arg(zClip0).arg(zClip1).arg(frontCut).arg(timePoint);
//    curstr = curstr.prepend(QString("").setNum(list_anchors->count()+1) + ": [ ");
//    curstr = curstr.append(" ]");
//    list_anchors->addItem(new QListWidgetItem(curstr));
//}


//void controlPanel::_slot_preview()
//{
//    CHECK_WINDOWS;

//    if(list_anchors->count()<=0)
//    {
//        v3d_msg("Please define at least one archor point.");
//        return;
//    }

//    if (!box_SampleRate)
//        return;

//    int  N = box_SampleRate->text().toInt();

//    MYFLOAT xRot, yRot, zRot,
//            xShift, yShift, zShift,
//            zoom,
//            xCut0, xCut1,
//            yCut0, yCut1,
//            zCut0, zCut1,
//            frontCut;
//    int showSurf, showSurf_last,
//        timePoint,timePoint_last;
//    bool channelR, channelG, channelB,
//            channelR_last, channelG_last, channelB_last;
//    MYFLOAT xRot_last, yRot_last,zRot_last,
//            xShift_last,yShift_last,zShift_last,
//            zoom_last,
//            xCut0_last,xCut1_last,
//            yCut0_last,yCut1_last,
//            zCut0_last,zCut1_last,
//            frontCut_last;
//    int xClip0,xClip1,yClip0,
//            yClip1,zClip0,zClip1;
//    int xClip0_last,xClip1_last,
//            yClip0_last,yClip1_last,
//            zClip0_last,zClip1_last;

//    // added by Hanchuan Peng, 2013-Dec-14 for debugging
//    MYFLOAT xShift_current;
//    MYFLOAT yShift_current;
//    MYFLOAT zShift_current;
//    MYFLOAT zoom_current;
//    MYFLOAT channel_current;
//    MYFLOAT xClip0_current;
//    MYFLOAT xClip1_current;
//    MYFLOAT yClip0_current;
//    MYFLOAT yClip1_current;
//    MYFLOAT zClip0_current;
//    MYFLOAT zClip1_current;
//    MYFLOAT xCut0_current;
//    MYFLOAT xCut1_current;
//    MYFLOAT yCut0_current;
//    MYFLOAT yCut1_current;
//    MYFLOAT zCut0_current;
//    MYFLOAT zCut1_current;
//    MYFLOAT frontCut_current;
//    MYFLOAT timePoint_current;
//    //

//    MYFLOAT q1[4],q2[4],q_sample[4];
//    MYFLOAT Rot_current[3];
//    QRegExp rx("(\\ |\\,|\\.|\\:|\\t)");
//    for(int row = 0; row < list_anchors->count(); row++)
//    {
//        QString currentPoint = list_anchors->item(row)->text();
//        QStringList currentParas = currentPoint.split(rx);

//        GET_PARA;

//        if(row==0)
//        {
//            SET_3DVIEW;
//        }
//        else
//        {
//#ifdef __ZMAKE_DEBUG__
//            QStringList tmpParaLists;
//#endif

//            for (int i=1; i<=N; i++)
//            {
//                INTERPOLATION_PARA;

//#ifdef __ZMAKE_DEBUG__

//                QString tmps, tmpt;

//                tmpt.setNum(i).append(": [ ");

//                tmps.setNum((int)Rot_current[0]);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)Rot_current[1]);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)Rot_current[2]);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)xShift_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)yShift_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zShift_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zoom_current);                  tmpt.append(tmps).append(" ");
//                tmps.setNum((int)xCut0_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)xCut1_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)yCut0_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)yCut1_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zCut0_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zCut1_current);                 tmpt.append(tmps).append(" ");
//                tmps.setNum((int)1);                             tmpt.append(tmps).append(" ");  //set to 1 for tentative testing
//                tmps.setNum((int)1);                             tmpt.append(tmps).append(" ");  //set to 1 for tentative testing
//                tmps.setNum((int)1);                             tmpt.append(tmps).append(" ");  //set to 1 for tentative testing
//                tmps.setNum((int)2);                             tmpt.append(tmps).append(" ");     //2 for showSurf. This is only debug purpose as it seems Zhi's code has some bug for this field
//                tmps.setNum((int)xClip0_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)xClip1_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)yClip0_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)yClip1_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zClip0_current);                tmpt.append(tmps).append(" ");
//                tmps.setNum((int)zClip1_current);                tmpt.append(tmps).append(" ");


//                tmpt.append("]");

//                tmpParaLists << tmpt;
//#endif

//            }

//#ifdef __ZMAKE_DEBUG__
//            _saveAnchorFile("/home/zhi/Desktop/apfdebug.apftxt", tmpParaLists, true);
//#endif

//        }

//        UPDATE_PARA;
//    }

//    //now really save movie

//    if(QMessageBox::Yes == QMessageBox::question (0, "", QString("Save as movie frames?"),
//                                                  QMessageBox::Yes, QMessageBox::No))
//    {

//        QFileDialog d(this);
//        d.setWindowTitle(tr("Choose output dir:"));
//        d.setFileMode(QFileDialog::Directory);
//        if(d.exec())
//        {
//            QString selectedFile=(d.selectedFiles())[0];
//            int framenum = 0;
//            for(int row = 0; row < list_anchors->count(); row++)
//            {
//                QString currentPoint = list_anchors->item(row)->text();
//                QStringList currentParas = currentPoint.split(rx);

//                GET_PARA;

//                if (row==0)
//                {
//                    SET_3DVIEW;

//                    SCREENSHOT_SAVEFRAMES;
//                }
//                else
//                {
//                    for (int i=1; i<=N; i++)
//                    {
//                        INTERPOLATION_PARA;

//                        SCREENSHOT_SAVEFRAMES;
//                    }
//                }

//                UPDATE_PARA;

//            }

//            v3d_msg(QString("Video frames have now been saved in the folder [%1], please use the Movie Converter plugin to convert to a single movie (e.g. avi, mpg).").arg(selectedFile));

//        }
//    }

//    return;
//}

//void controlPanel::_slot_delete()
//{
//    CHECK_WINDOWS;

//    if(list_anchors->currentRow()==-1)
//    {
//        v3d_msg("Please select a valid archor point.");
//        return;
//    }

//    list_anchors->takeItem(list_anchors->currentRow());

//    UPDATE_LIST_INDEX
//}

//void controlPanel::_slot_show_item(QListWidgetItem *item)
//{
//    CHECK_WINDOWS;

//    if (!item) return;
//    QString currentPoint = item->text();
//    QRegExp rx("(\\ |\\,|\\.|\\:|\\t)");
//    QStringList currentParas = currentPoint.split(rx);

//    MYFLOAT xRot, yRot,zRot,xShift,yShift,zShift,zoom,xCut0,xCut1,yCut0,yCut1,zCut0,zCut1,frontCut;
//    bool channelR,channelG,channelB;
//    int showSurf;
//    int xClip0,xClip1,yClip0,yClip1,zClip0,zClip1,timePoint;

//    GET_PARA;

//    SET_3DVIEW;
//}

//void controlPanel::_slot_show()
//{
//    CHECK_WINDOWS;

//    if(list_anchors->currentRow()==-1)
//    {
//        v3d_msg("Please select a valid archor point.");
//        return;
//    }

//    if (list_anchors)
//        _slot_show_item(list_anchors->currentItem());
//    else
//        return;
//}

//void controlPanel::_slot_upload()
//{
//    v3d_msg("To be implemented!");
//}

//bool _saveAnchorFile(QString filename, QStringList ParaLists, bool b_append)
//{
//    if (filename.isEmpty() || ParaLists.size()<=0)
//    {
//        v3d_msg("Fail to invoke _saveAnchorFile(), invalid parameter or empty list.",0);
//        return false;
//    }

//    ofstream myfile;
//    if (b_append)
//        myfile.open (filename.toLatin1(),ios::out | ios::app); //need fix! need to check if myfile.open is successful! noted by PHC, 20131214
//    else
//        myfile.open (filename.toLatin1(),ios::out | ios::trunc); //need fix! need to check if myfile.open is successful! noted by PHC, 20131214
//    QRegExp rx("(\\ |\\,|\\.|\\:|\\t)");
//    for(int row = 0; row < ParaLists.size(); row++)
//    {
//        QStringList currentParas = ParaLists.at(row).split(rx);
//        if (currentParas.size()<27)
//        {
//            v3d_msg(QString("Format error in splitting paralist for row [%1] which is [%2]. Ignore this row!").arg(row).arg(ParaLists.at(row)));
//            continue;
//        }
//        myfile << currentParas.at(3).toFloat();myfile << "  ";
//        myfile << currentParas.at(4).toFloat();myfile << "  ";
//        myfile << currentParas.at(5).toFloat();myfile << "  ";
//        myfile << currentParas.at(6).toFloat();myfile << "  ";
//        myfile << currentParas.at(7).toFloat();myfile << "  ";
//        myfile << currentParas.at(8).toFloat();myfile << "  ";
//        myfile << currentParas.at(9).toFloat();myfile << "  ";
//        myfile << currentParas.at(10).toFloat();myfile << "  ";
//        myfile << currentParas.at(11).toFloat();myfile << "  ";
//        myfile << currentParas.at(12).toFloat();myfile << "  ";
//        myfile << currentParas.at(13).toFloat();myfile << "  ";
//        myfile << currentParas.at(14).toFloat();myfile << "  ";
//        myfile << currentParas.at(15).toFloat();myfile << "  ";
//        myfile << currentParas.at(16).toFloat();myfile << "  ";
//        myfile << currentParas.at(17).toFloat();myfile << "  ";
//        myfile << currentParas.at(18).toFloat();myfile << "  ";
//        myfile << currentParas.at(19).toFloat();myfile << "  ";
//        myfile << currentParas.at(20).toFloat();myfile << "  ";
//        myfile << currentParas.at(21).toFloat();myfile << "  ";
//        myfile << currentParas.at(22).toFloat();myfile << "  ";
//        myfile << currentParas.at(23).toFloat();myfile << "  ";
//        myfile << currentParas.at(24).toFloat();myfile << "  ";
//        myfile << currentParas.at(25).toFloat();myfile << "  ";
//        myfile << currentParas.at(26).toFloat();myfile << "  ";
//        myfile << currentParas.at(27).toFloat();
//        myfile << endl;
//    }
//    myfile.close();

//    return true;
//}



//bool controlPanel::saveAnchorFile(QString filename)
//{
//    if (filename.isEmpty() || !list_anchors || list_anchors->count()<=0)
//    {
//        v3d_msg("Fail to invoke saveAnchorFile(), maybe the file name is invalid or invalid list_anchors pointer or empty anchor point list.",0);
//        return false;
//    }

//    QStringList paraLists;
//    for(int row = 0; row < list_anchors->count(); row++)
//        paraLists << list_anchors->item(row)->text();

//    return _saveAnchorFile(filename, paraLists, false);
//}

//void controlPanel::_slot_save()
//{
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Anchor Point File"),
//                                                    "",
//                                                    tr("Anchor Point File (*.apftxt *.txt *.apf)"));

//    if (!(fileName.isEmpty()))
//        saveAnchorFile(fileName);
//}

//void controlPanel::_slot_load()
//{

//    QString fileOpenName = QFileDialog::getOpenFileName(this, QObject::tr("Open File"),
//                                                        "",
//                                                        QObject::tr("Anchor Point File (*.apftxt *.txt *.apf)"
//                                                                    ));
//    if(fileOpenName.isEmpty())
//    {
//        return;
//    }

//    if (fileOpenName.size()>0)
//    {
//        list_anchors->clear();
//        ifstream ifs(fileOpenName.toLatin1());
//        string points;
//        MYFLOAT xRot, yRot,zRot,xShift,yShift,zShift,zoom,xCut0,xCut1,yCut0,yCut1,zCut0,zCut1,frontCut;
//        bool channelR,channelG,channelB;
//        int showSurf,timePoint;
//        int xClip0,xClip1,yClip0,yClip1,zClip0,zClip1;
//        while(ifs && getline(ifs, points))
//        {
//            std::istringstream iss(points);
//            iss >> xRot >> yRot >> zRot >>
//                   xShift >> yShift >> zShift >>
//                   zoom >>
//                   xCut0 >> xCut1 >>
//                   yCut0 >> yCut1 >>
//                   zCut0 >> zCut1 >>
//                   channelR >> channelG >> channelB >>
//                   showSurf >>
//                   xClip0 >> xClip1 >> yClip0 >>
//                   xClip1 >> zClip0 >> zClip1 >>
//                   frontCut >>
//                   timePoint;
//            QString curstr = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25").arg(xRot).arg(yRot).arg(zRot).arg(xShift).arg(yShift).arg(zShift).arg(zoom).arg(xCut0).arg(xCut1).arg(yCut0).arg(yCut1).arg(zCut0).arg(zCut1).arg(channelR).arg(channelG).arg(channelB).arg(showSurf).arg(xClip0).arg(xClip1).arg(yClip0).arg(yClip1).arg(zClip0).arg(zClip1).arg(frontCut).arg(timePoint);
//            curstr = curstr.prepend(QString("").setNum(list_anchors->count()+1) + ": [ ");
//            curstr = curstr.append(" ]");
//            list_anchors->addItem(new QListWidgetItem(curstr));

//        }
//    }
//    return;
//}

//void controlPanel::_slot_up()
//{
//    CHECK_WINDOWS;

//    if(list_anchors->currentRow()==-1)
//    {
//        v3d_msg("Please select a valid archor point.");
//        return;
//    }

//    if(list_anchors->currentRow() > 0)
//    {
//        int currentIndex = list_anchors->currentRow();
//        QListWidgetItem *currentItem = list_anchors->takeItem(currentIndex);
//        list_anchors->insertItem(currentIndex-1, currentItem);
//        list_anchors->setCurrentRow(currentIndex-1);

//       UPDATE_LIST_INDEX
//    }

//}

//void controlPanel::_slot_down()
//{
//    CHECK_WINDOWS;

//    if(list_anchors->currentRow()==-1)
//    {
//        v3d_msg("Please select a valid archor point.");
//        return;
//    }

//    if(list_anchors->currentRow() < list_anchors->count()-1)
//    {
//        int currentIndex = list_anchors->currentRow();
//        QListWidgetItem *currentItem = list_anchors->takeItem(currentIndex);
//        list_anchors->insertItem(currentIndex+1, currentItem);
//        list_anchors->setCurrentRow(currentIndex+1);

//       UPDATE_LIST_INDEX
//    }

//}

//void angles_to_quaternions(MYFLOAT q[], MYFLOAT xRot, MYFLOAT yRot,MYFLOAT zRot)
//{
//    MYFLOAT xRot_Rad = xRot * (pi/180.0);  // if(xRot_Rad>pi) xRot_Rad -= 2*pi;
//    MYFLOAT yRot_Rad = yRot * (pi/180.0);  // if(yRot_Rad>pi) yRot_Rad -= 2*pi;
//    MYFLOAT zRot_Rad = zRot * (pi/180.0);  // if(zRot_Rad>pi) zRot_Rad -= 2*pi;

//   /* q[0] = cos(xRot_Rad/2)*cos(yRot_Rad/2)*cos(zRot_Rad/2)+sin(xRot_Rad/2)*sin(yRot_Rad/2)*sin(zRot_Rad/2);
//    q[1] = sin(xRot_Rad/2)*cos(yRot_Rad/2)*cos(zRot_Rad/2)-cos(xRot_Rad/2)*sin(yRot_Rad/2)*sin(zRot_Rad/2);
//    q[2] = cos(xRot_Rad/2)*sin(yRot_Rad/2)*cos(zRot_Rad/2)+sin(xRot_Rad/2)*cos(yRot_Rad/2)*sin(zRot_Rad/2);
//    q[3] = cos(xRot_Rad/2)*cos(yRot_Rad/2)*sin(zRot_Rad/2)-sin(xRot_Rad/2)*sin(yRot_Rad/2)*cos(zRot_Rad/2);*/

//    q[0] = -sin(xRot_Rad/2)*sin(yRot_Rad/2)*sin(zRot_Rad/2)+ cos(xRot_Rad/2)*cos(yRot_Rad/2)*cos(zRot_Rad/2);
//    q[1] = sin(xRot_Rad/2)*cos(yRot_Rad/2)*cos(zRot_Rad/2)+sin(yRot_Rad/2)*sin(zRot_Rad/2)*cos(xRot_Rad/2);
//    q[2] = -sin(xRot_Rad/2)*sin(zRot_Rad/2)*cos(yRot_Rad/2)+sin(yRot_Rad/2)*cos(xRot_Rad/2)*cos(zRot_Rad/2);
//    q[3] = sin(xRot_Rad/2)*sin(yRot_Rad/2)*cos(zRot_Rad/2)+sin(zRot_Rad/2)*cos(xRot_Rad/2)*cos(yRot_Rad/2);

//    return;

//}

//void angles_to_quaternions_3DRotation(MYFLOAT q[], MYFLOAT xRot, MYFLOAT yRot,MYFLOAT zRot)
//{
//    MYFLOAT xRot_Rad = xRot * (pi/180.0);  //if(xRot_Rad>pi) xRot_Rad -= 2*pi;
//    MYFLOAT yRot_Rad = yRot * (pi/180.0);  //if(yRot_Rad>pi) yRot_Rad -= 2*pi;
//    MYFLOAT zRot_Rad = zRot * (pi/180.0);  //if(zRot_Rad>pi) zRot_Rad -= 2*pi;

//    MYFLOAT R[3][3];

//    const MYFLOAT cosAngle1 = cosf( xRot_Rad ),  sinAngle1 = sinf( xRot_Rad );
//    const MYFLOAT cosAngle2 = cosf( yRot_Rad ),  sinAngle2 = sinf( yRot_Rad );
//    const MYFLOAT cosAngle3 = cosf( zRot_Rad ),  sinAngle3 = sinf( zRot_Rad );

//    // Repeated calculations (for efficiency)
//    MYFLOAT s1c3 = sinAngle1 * cosAngle3;
//    MYFLOAT s3c1 = sinAngle3 * cosAngle1;
//    MYFLOAT s1s3 = sinAngle1 * sinAngle3;
//    MYFLOAT c1c3 = cosAngle1 * cosAngle3;

//    const int i = 0;
//    const int j = 1;
//    const int k = 2;

//    R[i][i] =  cosAngle2 * cosAngle3;
//    R[i][j] = -sinAngle3 * cosAngle2;
//    R[i][k] =  sinAngle2;
//    R[j][i] =  s3c1 + sinAngle2 * s1c3;
//    R[j][j] =  c1c3 - sinAngle2 * s1s3;
//    R[j][k] = -sinAngle1 * cosAngle2;
//    R[k][i] =  s1s3 - sinAngle2 * c1c3;
//    R[k][j] =  s1c3 + sinAngle2 * s3c1;
//    R[k][k] =  cosAngle1 * cosAngle2;

//    const MYFLOAT tr = R[i][i] + R[j][j] + R[k][k];
//    if( tr >= R[0][0]  &&  tr >= R[1][1]  &&  tr >= R[2][2] )
//    {
//        q[0] = 1 + tr;
//        q[1] = R[2][1] - R[1][2];
//        q[2] = R[0][2] - R[2][0];
//        q[3] = R[1][0] - R[0][1];

//        // Check if R[0][0] is largest along the diagonal
//    }
//    else if( R[0][0] >= R[1][1]  &&  R[0][0] >= R[2][2]  )
//    {
//        q[0] = R[2][1] - R[1][2];
//        q[1] = 1 - (tr - 2*R[0][0]);
//        q[2] = R[0][1]+R[1][0];
//        q[3] = R[0][2]+R[2][0];

//        // Check if R[1][1] is largest along the diagonal
//    }
//    else if( R[1][1] >= R[2][2] )
//    {
//        q[0] = R[0][2] - R[2][0];
//        q[1] = R[0][1] + R[1][0];
//        q[2] = 1 - (tr - 2*R[1][1]);
//        q[3] = R[1][2] + R[2][1];

//        // R[2][2] is largest along the diagonal
//    }
//    else
//    {
//        q[0] = R[1][0] - R[0][1];
//        q[1] = R[0][2] + R[2][0];
//        q[2] = R[1][2] + R[2][1];
//        q[3] = 1 - (tr - 2*R[2][2]);
//    }

//    // Scale to unit length
//    int tmpi; //change i to tmpi as 'i' has ben defined above.
//    MYFLOAT scale = 0.0;
//    for (tmpi = 0; tmpi < 4; tmpi++)
//        scale += q[tmpi] * q[tmpi];
//    scale = 1.0/std::sqrt(scale);
//    if( q[0] < 0 )  scale = -scale;   // canonicalize
//    for (tmpi = 0; tmpi < 4; tmpi++)
//        q[tmpi] *= scale;

//    return;
//}

//void slerp_zhi(MYFLOAT q1[], MYFLOAT q2[], MYFLOAT t, MYFLOAT q_sample[])
//{
//    MYFLOAT cos_omega = dot_multi_normalized(q1, q2);
//    MYFLOAT omega, theta;
//    MYFLOAT c1,  c2;
//    bool flag = false;

//    omega = acos(cos_omega);
//    if (cos_omega<0)
//    {
//        flag = true;
//        omega = omega - pi;
//    }

//    theta = t*omega;
//    double sin_omega = sin(omega);
//    if (fabs(sin_omega)<0.001) //use linear interpolation when the angle is close to 0
//    {
//        c1 = 1.0-t;
//        c2 = t;
//    }
//    else
//    {
//        c1 = sin(omega - theta)/sin_omega;
//        c2 = sin(theta)/sin_omega;
//    }

//    if (flag)
//        c2 = -c2; //equivalent to negative of one end of q1 or q2 (in this case, it is q2) for the interpolation below

//    printf("slerp result is (cos_t=%f, omega=%f, flag=%d, angle=%f, c1=%f, c2=%f)\n", cos_omega, omega, int(flag), omega/pi*180.0, c1, c2);

//    MYFLOAT scale = 0;
//    for(int i= 0; i<4;i++)
//    {
//        q_sample[i] = c1*q1[i] + c2*q2[i];
//        scale += q_sample[i] * q_sample[i];
//    }

//    scale = 1.0/std::sqrt(scale);

//    for (int i = 0; i < 4; i++)
//    {
//        q_sample[i] *= scale;
//    }

//    printf("current t=%f, current angle with p1=%f, current angle with p2=%f. sum angle=%f. \n\n",
//           t, acos(dot_multi_normalized(q1, q_sample))/pi*180.0, acos(dot_multi_normalized(q_sample, q2))/pi*180.0,
//           acos(dot_multi_normalized(q1, q_sample))/pi*180.0 + acos(dot_multi_normalized(q_sample, q2))/pi*180.0);

//    return;
//}

//void quaternions_to_angles(MYFLOAT Rot_current[], MYFLOAT q_sample[])
//{
//  /*  MYFLOAT rot_x = atan2f(2.0*(q_sample[0]*q_sample[1]+q_sample[2]*q_sample[3]), 1.0-2.0*(q_sample[1]*q_sample[1]+q_sample[2]*q_sample[2]));
//    MYFLOAT rot_y = asinf(2.0*(q_sample[0]*q_sample[2]-q_sample[3]*q_sample[1]));
//    MYFLOAT rot_z = atan2f(2.0*(q_sample[0]*q_sample[3]+q_sample[1]*q_sample[2]), 1.0-2.0*(q_sample[2]*q_sample[2]+q_sample[3]*q_sample[3]));*/

//    MYFLOAT rot_x = atan2f(-2.0*(q_sample[2]*q_sample[3])+2.0*(q_sample[0]*q_sample[1]), 1.0-2.0*(q_sample[1]*q_sample[1]+q_sample[2]*q_sample[2]));
//    MYFLOAT rot_y = asinf(2.0*(q_sample[1]*q_sample[3]+q_sample[0]*q_sample[2]));
//    MYFLOAT rot_z = atan2f(-2.0*(q_sample[1]*q_sample[2])+2*(q_sample[0]*q_sample[3]), 1.0-2.0*(q_sample[2]*q_sample[2]+q_sample[3]*q_sample[3]));

//    Rot_current[0] = rot_x * (180.0/pi); // if(Rot_current[0]<0) Rot_current[0] = 360.0 + Rot_current[0];
//    Rot_current[1] = rot_y * (180.0/pi);  // if(Rot_current[0]<0) Rot_current[1] = 360.0 + Rot_current[1];
//    Rot_current[2] = rot_z * (180.0/pi);  //if(Rot_current[0]<0) Rot_current[2] = 360.0 + Rot_current[2];
//}

//void quaternions_to_angles_3DRotation(MYFLOAT Rot_current[], MYFLOAT q[])
//{
//    MYFLOAT q00 = q[0]*q[0], q11=q[1]*q[1], q22=q[2]*q[2], q33=q[3]*q[3];
//    MYFLOAT q01 = q[0]*q[1], q02=q[0]*q[2], q03=q[0]*q[3];
//    MYFLOAT q12 = q[1]*q[2], q13=q[1]*q[3], q23=q[2]*q[3];

//    MYFLOAT R[3][3];

//    int i = 0;
//    int j = 1;
//    int k = 2;

//    R[i][i] = q00+q11-q22-q33;
//    R[i][j] = 2*(q12-q03);
//    R[i][k] = 2*(q13+q02);
//    R[j][i] = 2*(q12+q03);
//    R[j][j] = q00-q11+q22-q33;
//    R[j][k] = 2*(q23-q01);
//    R[k][i] = 2*(q13-q02);
//    R[k][j] = 2*(q23+q01);
//    R[k][k] = q00-q11-q22+q33;

//    MYFLOAT rot_x = atan2f(R[1][2],R[2][2]);
//    MYFLOAT rot_y = atan2f(-R[0][2], -sqrt(R[0][0]*R[0][0]+R[0][1]*R[0][1]));
//    MYFLOAT rot_z = atan2f(sinf(rot_x)* R[2][0] - cosf(rot_x)*R[1][0], cosf(rot_x)*R[1][1] - sinf(rot_x)*R[2][1]);

//    Rot_current[0] = rot_x * (180.0/pi);
//    Rot_current[1] = rot_y * (180.0/pi);
//    Rot_current[2] = rot_z * (180.0/pi);

//    return;
//}



//MYFLOAT dot_multi(MYFLOAT q1[], MYFLOAT q2[])
//{
//    MYFLOAT result = 0;

//    for(int i= 0; i<4;i++)
//    {
//        result += q1[i] * q2[i];
//    }

//    return result;
//}

//MYFLOAT dot_multi_normalized(MYFLOAT q1[], MYFLOAT q2[])
//{
//    MYFLOAT result = 0.0, r1=0.0, r2=0.0;

//    for(int i= 0; i<4;i++)
//    {
//        result += q1[i] * q2[i];
//        r1 += q1[i]*q1[i];
//        r2 += q2[i]*q2[i];
//    }

//    return result/sqrt(r1*r2);
//}