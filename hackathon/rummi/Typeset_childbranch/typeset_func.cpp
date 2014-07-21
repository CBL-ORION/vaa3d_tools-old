/* typeset_func.cpp
 * This is a plugin to set segment type based on marker location
 * 2014-06-27 : by Surobhi Ganguly
 */

#include <v3d_interface.h>
#include "v3d_message.h"
#include "my_surf_objs.h"
#include "openSWCDialog.h"
#include "sort_func.h"
#include "sort_swc.h"
#include "typeset.h"
#include "typeset_func.h"
#include "typeset_plugin.h"
#include "customary_structs/vaa3d_neurontoolbox_para.h"
#include <vector>
#include <iostream>
//using namespace std;

//const QString title = QObject::tr("typeset Neuron");

bool export_list2file(QList<NeuronSWC> & lN, QString fileSaveName, QString FileSWCOpenName)
{
    QFile file(fileSaveName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream myfile(&file);
    myfile<<"# generated by Vaa3D Plugin typeset"<<endl;
    myfile<<"# source file(s): "<<FileSWCOpenName<<endl;
    myfile<<"# id,type,x,y,z,r,pid"<<endl;
    for (V3DLONG i=0;i<lN.size();i++)
        myfile << lN.at(i).n <<" " << lN.at(i).type << " "<< lN.at(i).x <<" "<<lN.at(i).y << " "<< lN.at(i).z << " "<< lN.at(i).r << " " <<lN.at(i).pn << "\n";
    file.close();
    cout<<"swc file "<<fileSaveName.toStdString()<<" has been generated, size: "<<lN.size()<<endl;
    return true;
}

int typeset_swc(V3DPluginCallback2 &callback, QWidget *parent)
{

    sort_menu(callback,parent); //sort first, then typeset

    v3d_msg("Be sure to open the correct (filename)_sort swc file");

    QString FileSWCOpenName;
    FileSWCOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open SWC File"),
            "",
            QObject::tr("Supported file (*.swc *.eswc)"
                ";;Neuron structure	(*.swc)"
                ";;Extended neuron structure (*.eswc)"
                ));

    NeuronTree nt;

    if (FileSWCOpenName.toUpper().endsWith(".SWC") || FileSWCOpenName.toUpper().endsWith(".ESWC"))
    {
        bool ok;

        nt = readSWC_file(FileSWCOpenName);
        if (!ok)
            return 0;
    }

    QList<ImageMarker> tmp_list;
    NeuronTree result = nt;



    double marker_readlocation = 0;
    marker_readlocation = QInputDialog::getDouble(parent, "Please set child branch type","Type: \n"
                                                         "1 - read from 3D window \n"
                                                         "2 - read from marker file ",0,0,2,1);

    if (marker_readlocation == 0 || marker_readlocation == 1)
    {
        v3dhandle current_3Dview = callback.currentImageWindow(); //get's current image window (3D that's open)
        LandmarkList current_3Dview_markers = callback.getLandmark(current_3Dview); //gives list of markers drawn

        for (int i=0;i<current_3Dview_markers.size();i++)//translate marker info from landmark list to tmp_list
        {
            LocationSimple t = current_3Dview_markers.at(i);

            ImageMarker pt;
            pt.x = t.x;
            pt.y = t.y;
            pt.z = t.z;
            tmp_list.push_back(pt);
        }
    }
    if (tmp_list.size() < 1 || marker_readlocation == 2) //if no markers were in view
    {
        QString FileMarkerOpenName;
        FileMarkerOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                "",
                QObject::tr("Supported file (*.marker)"));

        if(FileSWCOpenName.isEmpty() )// || FileMarkerOpenName.isEmpty())
            return 0;

        if (FileMarkerOpenName.toUpper().endsWith(".MARKER"))
        {
            //bool ok;
            tmp_list = readMarker_file(FileMarkerOpenName);
            //if (!ok)
            //    return 0;
        }
    }

    if (tmp_list.size() < 1)
    {
        v3d_msg("no markers in 3D window; plugin not run");
    }
    else
    {
        double settype;
        settype = QInputDialog::getDouble(parent, "Please set child branch type","Type:",0,0,4,1);

        result = typeset_marker(nt, tmp_list, settype);

        QString fileDefaultName = FileSWCOpenName+QString("_typeset.swc");
        //write new SWC to file
        QString fileSaveName = QFileDialog::getSaveFileName(0, QObject::tr("Save File"),
                fileDefaultName,
                QObject::tr("Supported file (*.swc)"
                    ";;Neuron structure	(*.swc)"
                    ));
        if (!export_list2file(result.listNeuron,fileSaveName,FileSWCOpenName))
        {
            v3d_msg("fail to write the output swc file.");
            return 0;
        }


        return 1;
    }

//    }

}

bool typeset_swc(const V3DPluginArgList & input, V3DPluginArgList & output)
{
    cout<<"Welcome to typeset_swc"<<endl;
    vector<char*>* inlist = (vector<char*>*)(input.at(0).p);
    vector<char*>* outlist = NULL;
    vector<char*>* paralist = NULL;

    if(input.size() != 2)
    {
        printf("Please specify both input file and segment type.\n");
        return false;
    }
    paralist = (vector<char*>*)(input.at(1).p);
    if (paralist->size()!=1)
    {
        printf("Please specify only one parameter - the resampling step length.\n");
        return false;
    }

    QString FileSWCOpenName = QString(inlist->at(0));
    QString FileMarkerOpenName;
    QString fileSaveName;

    if (output.size()==0)
    {
        printf("No outputfile specified.\n");
        fileSaveName = FileSWCOpenName + "_typesetd.swc";
    }
    else if (output.size()==1)
    {
        outlist = (vector<char*>*)(output.at(0).p);
        fileSaveName = QString(outlist->at(0));
    }
    else
    {
        printf("You have specified more than 1 output file.\n");
        return false;
    }

    NeuronTree nt;
    QList<ImageMarker> tmp_list;

    if (FileSWCOpenName.toUpper().endsWith(".SWC") || FileSWCOpenName.toUpper().endsWith(".ESWC"))
        nt = readSWC_file(FileSWCOpenName);

    double settype = 0;

    if (FileMarkerOpenName.toUpper().endsWith(".marker"))
    {
        bool ok;
        tmp_list = readMarker_file(FileMarkerOpenName);
        if (!ok)
            return 0;
    }

    NeuronTree result = typeset_marker(nt,tmp_list,settype);

    if (!export_list2file(result.listNeuron, fileSaveName, FileSWCOpenName))
    {
        printf("fail to write the output swc file.\n");
        return false;
    }

    return true;
}

bool typeset_swc_toolbox(const V3DPluginArgList & input)
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

    NeuronTree result = typeset_marker(nt,tmp_list,settype);

    QString fileDefaultName = FileSWCOpenName+QString("_")+FileMarkerOpenName+QString("_typeset.swc");
    //write new SWC to file
    QString fileSaveName = QFileDialog::getSaveFileName(0, QObject::tr("Save File"),
            fileDefaultName,
            QObject::tr("Supported file (*.swc)"
                ";;Neuron structure	(*.swc)"
                ));
    if (!export_list2file(result.listNeuron,fileSaveName,FileSWCOpenName))
    {
        v3d_msg("fail to write the output swc file.");
        return false;
    }

    return true;

}
