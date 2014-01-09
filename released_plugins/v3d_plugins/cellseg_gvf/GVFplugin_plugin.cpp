/* GVFplugin_plugin.cpp
 * Implementation of Gradient Vector Flow cell body segmentation
 * 2014.01.06 : by BRL
 */
 
#include "v3d_message.h"
#include <vector>
#include "dialog_watershed_para.h"
#include "volimg_proc.h"
#include "img_definition.h"
#include "GVFplugin_plugin.h"
#include "FL_gvfCellSeg.h"

#define USHORTINT16 unsigned short int

using namespace std;
Q_EXPORT_PLUGIN2(gvf_cellseg, GVFplugin);
 

QStringList GVFplugin::menulist() const
{
	return QStringList() 
        <<tr("Gradient vector flow based Segmentation")
        <<tr("About");
}

QStringList GVFplugin::funclist() const
{
	return QStringList()
        <<tr("gvf_segmentation")
        <<tr("help");
}

void GVFplugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("Gradient vector flow based Segmentation"))
    {


        // the GVF function wants a pointer to a Vol3DSimple, which I haven't seen before.
        // this code below generates it  (take from plugin_FL_cellseg)


        // check what's up with the current window: is there a valid image pointer?
        v3dhandle curwin = callback.currentImageWindow();
        if (!curwin)
        {
            v3d_msg("Please open an image.");
            return;
        }



        Image4DSimple* subject = callback.getImage(curwin);
        QString m_InputFileName = callback.getImageName(curwin);

        if (!subject)
        {
            QMessageBox::information(0, "", QObject::tr("No image is open."));
            return;
        }
        if (subject->getDatatype()!=V3D_UINT8)
        {
            QMessageBox::information(0, "", QObject::tr("This demo program only supports 8-bit data. Your current image data type is not supported."));
            return;
        }

        V3DLONG sz0 = subject->getXDim();
        V3DLONG sz1 = subject->getYDim();
        V3DLONG sz2 = subject->getZDim();
        V3DLONG sz3 = subject->getCDim();

        Image4DProxy<Image4DSimple> pSub(subject);

        V3DLONG channelsz = sz0*sz1*sz2;

        float *pLabel = 0;
        unsigned char *pData = 0;



        gvfsegPara segpara;  // set these fields one at a time:

        segpara.diffusionIteration=  5;
        segpara.fusionThreshold = 10;
        segpara.minRegion = 10;
        segpara.sigma = 3;  // doesn't seem to be used in the actual function?


        //input parameters
        bool ok1;
        int c=1;

        if (sz3>1) //only need to ask if more than one channel
        {
            c = QInputDialog::getInteger(parent, "Channel",
                                             "Choose channel for segmentation:",
                                             1, 1, sz3, 1, &ok1);
            c = c-1;
            if (!ok1)
                return;
        }

        // read in parameters

        segpara.diffusionIteration = QInputDialog::getInteger(parent, "Diffusion Iterations",
                                         "Choose Number of Diffusion Iterations:",
                                         5, 1, 10, 1, &ok1);
        if (!ok1)
            return;


        segpara.fusionThreshold = QInputDialog::getInteger(parent, "Fusion Threshold",
                                         "Choose Fusion Threshold :",
                                         2, 1, 10, 1, &ok1);
        if (!ok1)
            return;


        segpara.minRegion= QInputDialog::getInteger(parent, "Minimum Region",
                                         "Choose Minimum Region Size (voxels):",
                                         10, 1, 1000, 1, &ok1);
        if (!ok1)
            return;


        // allocate memory for the images

        Vol3DSimple <unsigned char> * tmp_inimg = 0;
        Vol3DSimple <USHORTINT16> * tmp_outimg = 0;
        try
        {
            tmp_inimg = new Vol3DSimple <unsigned char> (sz0, sz1, sz2);
            tmp_outimg = new Vol3DSimple <USHORTINT16> (sz0, sz1, sz2);
        }
        catch (...)
        {
            v3d_msg("Unable to allocate memory for processing.");
            if (tmp_inimg) {delete tmp_inimg; tmp_inimg=0;}
            if (tmp_outimg) {delete tmp_outimg; tmp_outimg=0;}
            return;
        }

        //copy image data into our new memory
        memcpy((void *)tmp_inimg->getData1dHandle(), (void *)subject->getRawDataAtChannel(c), sz0*sz1*sz2);

        //now do computation

        //bool b_res = gvfCellSeg(img3d, outimg3d, segpara);
        bool b_res = gvfCellSeg(tmp_inimg, tmp_outimg, segpara);

        // clear out temporary space
        if (tmp_inimg) {delete tmp_inimg; tmp_inimg=0;}


        if (!b_res)
        {
            v3d_msg("image segmentation  using gvfCellSeg()  failed \n");
        }
        else
        {   // now display the results

       // parameters for the new image data
            V3DLONG new_sz0 = tmp_outimg->sz0();
            V3DLONG new_sz1 = tmp_outimg->sz1();
            V3DLONG new_sz2 = tmp_outimg->sz2();
            V3DLONG new_sz3 = 1;
            V3DLONG tunits = new_sz0*new_sz1*new_sz2*new_sz3;


            //
            USHORTINT16 * outvol1d = new USHORTINT16 [tunits];

         //   USHORTINT16 * tmpImg_d1d = (USHORTINT16 *)(tmp_outimg->getData1dHandle());

            memcpy((void *)outvol1d, (void *)tmp_outimg->getData1dHandle(), tunits*sizeof(USHORTINT16));
            if (tmp_outimg) {delete tmp_outimg; tmp_outimg=0;} //free the space immediately for better use of memory

            Image4DSimple p4DImage;
            p4DImage.setData((unsigned char*)outvol1d, sz0, sz1, sz2, 1, V3D_UINT16);

            v3dhandle newwin = callback.newImageWindow();
            callback.setImage(newwin, &p4DImage);
            callback.setImageName(newwin, QString("Segmented Image"));
            callback.updateImageWindow(newwin);
        }

        return;
    }
	else
	{
        v3d_msg(tr("A plugin for cell segmentation using Gradient Vector Flow. "
            "Developed by Brian Long, using the source code developed by Fuhui Long, Tianming Liu, and Hanchuan Peng (2010-2014)"));
	}
}

bool GVFplugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{


    if (func_name == tr("gvf_segmentation"))
	{
       /* int c=1;
        gvfsegPara segpara;  // default values
        segpara.diffusionIteration = 5;
        segpara.fusionThreshold = 3;
        segpara.minRegion = 10;

        vector<char*> infiles, inparas, outfiles;
        if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
        if(input.size() >= 2)
        {
            inparas = *((vector<char*> *)input.at(1).p);
        if(inparas.size() >= 1) c = atoi(inparas.at(0));
        if(inparas.size() >= 2) segpara.diffusionIteration = atoi(inparas.at(1));
        if(inparas.size() >= 3) segpara.fusionThreshold = atoi(inparas.at(2));
        if(inparas.size() >= 4) segpara.minRegion = atoi(inparas.at(3));
        if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);
        }

        Image4DSimple *inimg = callback.loadImage(infiles);
        if (!inimg || !inimg->valid())
        {
            v3d_msg("Fail to open the image file.", 0);
            return false;
        }

        V3DLONG sz0 = inimg->getXDim();

        V3DLONG sz1  = inimg->getYDim();

        V3DLONG sz2  = inimg->getZDim();

        V3DLONG maxc  = inimg->getCDim();

if (maxc==1)
{
    c=1;
}


        // allocate memory for the images

        Vol3DSimple <unsigned char> * tmp_inimg = 0;
        Vol3DSimple <USHORTINT16> * tmp_outimg = 0;
        try
        {
            tmp_inimg = new Vol3DSimple <unsigned char> (sz0, sz1, sz2);
            tmp_outimg = new Vol3DSimple <USHORTINT16> (sz0, sz1, sz2);
        }
        catch (...)
        {
            v3d_msg("Unable to allocate memory for processing.");
            if (tmp_inimg) {delete tmp_inimg; tmp_inimg=0;}
            if (tmp_outimg) {delete tmp_outimg; tmp_outimg=0;}
            return;
        }

        //copy image data into our new memory

        memcpy((void *)tmp_inimg->getData1dHandle(), (void *)subject->getRawDataAtChannel(c), sz0*sz1*sz2);

        //now do computation

        //bool b_res = gvfCellSeg(img3d, outimg3d, segpara);
        bool b_res = gvfCellSeg(tmp_inimg, tmp_outimg, segpara);

        // clear out temporary space
        if (tmp_inimg) {delete tmp_inimg; tmp_inimg=0;}


        // now write to out image:


*/

       v3d_msg("To be implemented.");
	}
	else if (func_name == tr("help"))
	{
		v3d_msg("To be implemented.");
	}
	else return false;

	return true;
}

