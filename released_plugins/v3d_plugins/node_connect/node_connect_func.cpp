/* node_connect_func.cpp
 * This is a plugin that takes two marker point inputs, and connects the two swc nodes corresponding to the same location.
 * 2014-08-08 : by SurobhiGanguly
 */

#include <v3d_interface.h>
#include "v3d_message.h"
#include "my_surf_objs.h"
#include "openSWCDialog.h"
#include "customary_structs/vaa3d_neurontoolbox_para.h"
#include <vector>
#include <iostream>
#include "node_connect_func.h"
#include "basic_surf_objs.h"
#include <math.h>

//using namespace std;

static LandmarkList current_3Dview_markers;
struct Point;
struct Point
{
    double n,x,y,z,r,p;
    //V3DLONG type;
    double type;
    //Point* p;
    V3DLONG childNum;
    vector<int> child_ids;
};
typedef vector<Point*> Tree;


bool export_list2file_v2(QList<NeuronSWC> & lN, QString fileSaveName, QString FileSWCOpenName)
{
    QFile file(fileSaveName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream myfile(&file);
    myfile<<"# generated by Vaa3D Plugin node_connect"<<endl;
    myfile<<"# source file(s): "<<FileSWCOpenName<<endl;
    myfile<<"# id,type,x,y,z,r,pid"<<endl;
    for (V3DLONG i=0;i<lN.size();i++)
        myfile << lN.at(i).n <<" " << lN.at(i).type << " "<< lN.at(i).x <<" "<<lN.at(i).y << " "<< lN.at(i).z << " "<< lN.at(i).r << " " <<lN.at(i).pn << "\n";
    file.close();
    cout<<"swc file "<<fileSaveName.toStdString()<<" has been generated, size: "<<lN.size()<<endl;
    return true;
}

static int file_count = 0;
static QString last_save_name;
static QString FileSWCOpenName;
static QList <NeuronSWC> nt_list_tosave;

int connect_swc(V3DPluginCallback2 &callback, QList<ImageMarker> tmp_list)
{
    v3d_msg("Be sure to open the correct the swc file - NEW VERSION 1");

    NeuronTree nt;

        OpenSWCDialog * openDlg = new OpenSWCDialog(0, &callback);
        if (!openDlg->exec())
            return 0;

        FileSWCOpenName = openDlg->file_name;

        nt = openDlg->nt;

    //QList<ImageMarker> tmp_list;
    //NeuronTree result = nt;
    NeuronTree new_result;
    new_result=nt;



    if (tmp_list.size() < 1 || tmp_list.size()%2 != 0)//this probably won't happen...
    {
        v3d_msg("must have an even number of markers to use connect function; plugin not run");
        return 0;
    }
    else //run the major part of the plugin
    {
        Tree tree;
        Tree tree_add;

        for (V3DLONG i=0;i<nt.listNeuron.size();i++) //gets swc file input
        {
            NeuronSWC s = nt.listNeuron[i];
            Point* pt = new Point;
            pt->n = s.n;
            pt->x = s.x;
            pt->y = s.y;
            pt->z = s.z;
            pt->r = s.r;
            pt ->type = s.type;
            pt->p = s.parent;
            pt->childNum = 0;
            tree.push_back(pt);
        }
        //v3d_msg("beebeep");

        Point* parent_info;
        Point* child_info;
        Point* newpoint;
        double xnow,ynow,znow,dx,dy,dz,distance,min;
        double window=20;
        int min_place;
        int check = 0;
        bool parent_val = false;


        //bool child_val = false;
        //v3d_msg(QString("tmp_list.size() = %2").arg(tmp_list.size()));

        for (V3DLONG j=0; j<tmp_list.size(); j+=2) //iterates through every other marker
        {
            //for marker1
            xnow = tmp_list[j].x;
            ynow = tmp_list[j].y;
            znow = tmp_list[j].z;
            min = window;
            check = 0;

            for (V3DLONG i=0;i<tree.size();i++) //calculates distance from current marker to each node point
            {
                dx = tree.at(i)->x - xnow;
                dy = tree.at(i)->y- ynow;
                dz = tree.at(i)->z - znow;
                distance = sqrt(pow(dx,2) + pow(dy,2) + pow(dz,2));


                if (distance < min)
                {
                    min = distance;
                    min_place = i;
                    check = 1;
                }
            } //if (check == 1){v3d_msg("found min value!");}

            if (check == 1)
            {
                if (tree.at(min_place)->p == -1)
                {
                    parent_info = tree.at(min_place);
                    parent_val = true;
                }
                else
                {
                    child_info = tree.at(min_place);
                    //child_val = true;
                }
            }

            if (min >= window)//shouldn't happen too frequently
            {
                v3d_msg("marker "+QString("%5").arg(j+1)+" does not fall within range of swc node, and was skipped");
            }

            //for marker2
            xnow = tmp_list[j+1].x;
            ynow = tmp_list[j+1].y;
            znow = tmp_list[j+1].z;
            min = window;
            check = 0;


            for (V3DLONG i=0;i<tree.size();i++) //calculates distance from current marker to each node point
            {
                dx = tree.at(i)->x - xnow;
                dy = tree.at(i)->y- ynow;
                dz = tree.at(i)->z - znow;
                distance = sqrt(pow(dx,2) + pow(dy,2) + pow(dz,2));


                if (distance < min)
                {
                    min = distance;
                    min_place = i;
                    check = 1;
                }
            } //if (check == 1){v3d_msg("found min value!");}

            if (check == 1)
            {
                if (parent_val)
                    child_info = tree.at(min_place);
                else
                    parent_info = tree.at(min_place);


            }
            if (min >= window)//shouldn't happen too frequently
            {
                v3d_msg("marker "+QString("%5").arg(j+1)+" does not fall within range of swc node, and was skipped");
            }


            newpoint = child_info;
            newpoint->p = parent_info->n;

            //v3d_msg(QString("parent: %1, %2, %3,      %4").arg(parent_info->x).arg(parent_info->y).arg(parent_info->z).arg(parent_info->p));
            //v3d_msg(QString("parent: %1, %2, %3,      %4").arg(child_info->x).arg(child_info->y).arg(child_info->z).arg(child_info->p));
            //v3d_msg(QString("parent: %1, %2, %3,      %4").arg(newpoint->x).arg(newpoint->y).arg(newpoint->z).arg(parent_info->p));

            tree_add.push_back(newpoint);
            parent_val = false; //resets so that parent_info can be assigned something new

        }

        /*
        for(V3DLONG i=0;i<nt.listNeuron.size();i++)
        {
                NeuronSWC s_old = nt.listNeuron[i];
                Point* pt_old = new Point;
                pt_old->n = s_old.n;
                pt_old->x = s_old.x;
                pt_old->y = s_old.y;
                pt_old->z = s_old.z;
                pt_old->r = s_old.r;
                pt_old ->type = s_old.type;
                pt_old->p = s_old.parent;
                pt_old->childNum = 0;
                new_result.listNeuron.push_back(pt_old);
        }*/



        for (V3DLONG i=0;i<tree_add.size();i++) // NEED THIS FOR LOOP EVENTUALLY
            {

                NeuronSWC s_new;
                Point* p3 = tree_add[i];
                s_new.n = p3->n; //i+1;
                s_new.pn = p3->p;
                //if (p3->p==NULL) s_new.pn = -1;
                //else
                //    s_new.pn = index_map[p3->p]+1;
                //if (p3->p==p) printf("There is loop in the tree!\n");
                s_new.x = p3->x;
                s_new.y = p3->y;
                s_new.z = p3->z;
                s_new.r = p3->r;
                s_new.type = p3->type;

                //result.listNeuron.push_back(s_new);
                new_result.listNeuron.push_back(s_new);

            }

        //result = nt;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //nt_shared = result;

        QString fileDefaultName;
        if (file_count==0)
        {
            //FileSWCOpenName.chop(4);
            fileDefaultName = FileSWCOpenName+QString("_%1_connect.swc").arg(file_count);
        }
        if (file_count>0)
        {
            FileSWCOpenName.chop(14);
            fileDefaultName = FileSWCOpenName+QString("_%1_connect.swc").arg(file_count);
        }
        //write new SWC to file
        QString fileSaveName = QFileDialog::getSaveFileName(0, QObject::tr("Save File"),
                fileDefaultName,
                QObject::tr("Supported file (*.swc)"
                    ";;Neuron structure	(*.swc)"
                    ));
        last_save_name = fileSaveName;
//        nt_list_tosave = result.listNeuron;
        nt_list_tosave = new_result.listNeuron;
        if (!export_list2file_v2(nt_list_tosave,fileSaveName,FileSWCOpenName))
        {
            v3d_msg("fail to write the output swc file.");
            return 0;
        }
        file_count++;
        reload_SWC(callback);
        return 1;
    }
}

//void final_typeset_save()
//{
//    QString final_save_name = last_save_name+QString("_finaltypeset.swc");
//    QString fileSaveName = QFileDialog::getSaveFileName(0, QObject::tr("Save File"),
//            final_save_name,
//            QObject::tr("Supported file (*.swc)"
//                ";;Neuron structure	(*.swc)"
//                ));
//    if (!export_list2file_v2(nt_list_tosave,fileSaveName,FileSWCOpenName))
//    {
//        v3d_msg("fail to write the output swc file.");
//        return;
//    }
//    return;
//}

void reload_SWC(V3DPluginCallback2 &callback) //pushes most recent swc file onto image if image is open
{
    v3dhandle current_window = callback.currentImageWindow();

    NeuronTree nt = readSWC_file(last_save_name);
    callback.setSWC(current_window, nt);

    callback.updateImageWindow(current_window);
    //v3d_msg("reloaded");
}

QList<ImageMarker> get_markers(V3DPluginCallback2 &callback)
{

    QList<ImageMarker> tmp_list;

    v3dhandleList list_windows = callback.getImageWindowList();

    if (list_windows.size() < 1)
    {
        v3d_msg("No image open. Must load marker file.");

        QString FileMarkerOpenName;
        FileMarkerOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                "",
                    QObject::tr("Supported file (*.marker)"));
        if (FileMarkerOpenName.isEmpty())
            return tmp_list;
        else if (FileMarkerOpenName.toUpper().endsWith(".MARKER"))
        {
            tmp_list = readMarker_file(FileMarkerOpenName);
        }
    }
    else
    {
        v3dhandle current_window = callback.currentImageWindow();
        current_3Dview_markers = callback.getLandmark(current_window); //gives list of markers drawn


        for (V3DLONG i=0;i<current_3Dview_markers.size();i++)//translate marker info from landmark list to tmp_list
        {
            LocationSimple t = current_3Dview_markers.at(i);

            ImageMarker pt;
            pt.n = i+1; //marker number
            pt.x = t.x;
            pt.y = t.y;
            pt.z = t.z;
            tmp_list.push_back(pt);
        }
    }
    return tmp_list;

}

//bool typeset_swc(const V3DPluginArgList & input, V3DPluginArgList & output)
//{
//    cout<<"Welcome to typeset_swc"<<endl;
//    vector<char*>* inlist = (vector<char*>*)(input.at(0).p);
//    vector<char*>* outlist = NULL;
//    vector<char*>* paralist = NULL;

//    if(input.size() != 2)
//    {
//        printf("Please specify both input file and segment type.\n");
//        return false;
//    }
//    paralist = (vector<char*>*)(input.at(1).p);
//    if (paralist->size()!=1)
//    {
//        printf("Please specify only one parameter - the resampling step length.\n");
//        return false;
//    }

//    QString FileSWCOpenName = QString(inlist->at(0));
//    QString FileMarkerOpenName;
//    QString fileSaveName;

//    if (output.size()==0)
//    {
//        printf("No outputfile specified.\n");
//        fileSaveName = FileSWCOpenName + "_typeset.swc";
//    }
//    else if (output.size()==1)
//    {
//        outlist = (vector<char*>*)(output.at(0).p);
//        fileSaveName = QString(outlist->at(0));
//    }
//    else
//    {
//        printf("You have specified more than 1 output file.\n");
//        return false;
//    }

//    NeuronTree nt;
//    QList<ImageMarker> tmp_list;

//    if (FileSWCOpenName.toUpper().endsWith(".SWC") || FileSWCOpenName.toUpper().endsWith(".ESWC"))
//        nt = readSWC_file(FileSWCOpenName);

//    double settype = 0;

//    if (FileMarkerOpenName.toUpper().endsWith(".marker"))
//    {
//        bool ok;
//        tmp_list = readMarker_file(FileMarkerOpenName);
//        if (!ok)
//            return 0;
//    }

//    NeuronTree result = typeset_marker(nt,tmp_list,settype);

//    if (!export_list2file_v2(result.listNeuron, fileSaveName, FileSWCOpenName))
//    {
//        printf("fail to write the output swc file.\n");
//        return false;
//    }

//    return true;
//}

bool swc_toolbox(const V3DPluginArgList & input)
{
    vaa3d_neurontoolbox_paras * paras = (vaa3d_neurontoolbox_paras *)(input.at(0).p);
    NeuronTree nt = paras->nt;
    QString FileSWCOpenName = nt.file;
    QString FileMarkerOpenName;


    bool ok;
    double settype = 0;
    QList<ImageMarker> tmp_list;
    tmp_list = readMarker_file(FileMarkerOpenName);

    if (!ok)
        return true;

    //NeuronTree result = typeset_marker(nt,tmp_list,settype);

    QString fileDefaultName = FileSWCOpenName+QString("_")+FileMarkerOpenName+QString("_typeset.swc");
    //write new SWC to file
    QString fileSaveName = QFileDialog::getSaveFileName(0, QObject::tr("Save File"),
            fileDefaultName,
            QObject::tr("Supported file (*.swc)"
                ";;Neuron structure	(*.swc)"
                ));
//    if (!export_list2file_v2(result.listNeuron,fileSaveName,FileSWCOpenName))
//    {
//        v3d_msg("fail to write the output swc file.");
//        return false;
//    }

    return true;

}

