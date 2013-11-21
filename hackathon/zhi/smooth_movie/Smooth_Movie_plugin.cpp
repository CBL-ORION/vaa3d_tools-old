/* Smooth_Movie_plugin.cpp
 * This plugin can be used to generate a smooth movie by several points
 * 2013-11-21 : by Zhi Zhou
 */
 
#include "v3d_message.h"
#include <vector>
#include "Smooth_Movie_plugin.h"
using namespace std;
Q_EXPORT_PLUGIN2(Smooth_Movie, Smooth_Movie);

void MovieFromPoints(V3DPluginCallback2 &v3d, QWidget *parent);
static lookPanel *panel = 0;

QStringList Smooth_Movie::menulist() const
{
	return QStringList() 
        <<tr("Generate a movie by multiple anchor points")
		<<tr("about");
}

QStringList Smooth_Movie::funclist() const
{
    return QStringList()
        <<tr("func1")
        <<tr("func2")
        <<tr("help");
}

void Smooth_Movie::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("Generate a movie by multiple anchor points"))
	{
        MovieFromPoints(callback, parent);
	}else
	{
        v3d_msg(tr("This plugin can be used to generate a smooth movie by multiple anchor points. "
			"Developed by Zhi Zhou, 2013-11-21"));
	}
}

bool Smooth_Movie::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
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

void MovieFromPoints(V3DPluginCallback2 &v3d, QWidget *parent)
{
    v3dhandle curwin = v3d.currentImageWindow();
    if (!curwin)
    {
        v3d_msg("You don't have any image open in the main window.");
        return;
    }
    v3d.open3DWindow(curwin);

    if (panel)
    {
        panel->show();
        return;
    }
    else
    {
        panel = new lookPanel(v3d, parent);
        if (panel)
        {
            panel->show();
            panel->raise();
            panel->move(100,100);
            panel->activateWindow();
        }
    }
}

lookPanel::lookPanel(V3DPluginCallback2 &_v3d, QWidget *parent) :
    QDialog(parent), m_v3d(_v3d)
{
    QPushButton* Record     = new QPushButton("Record Points");
    QPushButton* Generate = new QPushButton("Generate");

    gridLayout = new QGridLayout();
    gridLayout->addWidget(Record, 1,0);
    gridLayout->addWidget(Generate,1,6);
    setLayout(gridLayout);
    setWindowTitle(QString("Generate a movie from anchor points"));

    connect(Record,     SIGNAL(clicked()), this, SLOT(_slot_record()));
    connect(Generate, SIGNAL(clicked()), this, SLOT(_slot_generate()));
}

lookPanel::~lookPanel()
{
   panel=0;
}

void lookPanel::_slot_record()
{
    curwin = m_v3d.currentImageWindow();
    m_v3d.open3DWindow(curwin);
    View3DControl *view = m_v3d.getView3DControl(curwin);
    m_v3d.open3DWindow(curwin);
    view->absoluteRotPose();
    int xRot = view->xRot();
    int yRot = view->yRot();
    int zRot = view->zRot();

    int xShift = view->xShift();
    int yShift = view->yShift();
    int zShift = view->zShift();

    int zoom = view->zoom();

    printf("paras (xrot,yrot,zrot,xshift,yshift,zshift,zoom) are (%d,%d,%d,%d,%d,%d,%d)\n",xRot,yRot,zRot,xShift,yShift,zShift);

}

void lookPanel::_slot_generate()
{
}
