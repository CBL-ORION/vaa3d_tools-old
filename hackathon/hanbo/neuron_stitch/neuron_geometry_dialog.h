/****************************************************************************
**
** neuron_geometry_dialog.h
** referance: surfaceobj_geometry_dialog.h
** by Hanbo Chen 2014.10.8
**
****************************************************************************/

#ifndef NEURON_GEOMETRY_DIALOG_H
#define NEURON_GEOMETRY_DIALOG_H

#include <QDialog>

#include "ui_neuron_geometry_dial.h"
#include <v3d_interface.h>


class NeuronGeometryDialog: public QDialog, private Ui_NeuronGeometryDialog
{
    Q_OBJECT

public:
    NeuronGeometryDialog(V3DPluginCallback2 * callback, V3dR_MainWindow* v3dwin);

public:
    //handles
    V3dR_MainWindow * v3dwin;
    V3DPluginCallback2 * callback;

protected:
    //changes
    double cur_shift_x[2], cur_shift_y[2], cur_shift_z[2],
           cur_scale_x[2], cur_scale_y[2], cur_scale_z[2],
           cur_gscale_x[2], cur_gscale_y[2], cur_gscale_z[2],
           cur_scale_r[2],
           cur_rotate_x[2], cur_rotate_y[2], cur_rotate_z[2];
    double cur_cx[2], cur_cy[2], cur_cz[2]; //center of the current object, which should NOT affected by a rotation, but should be updated if a shift happen
    bool cur_flip_x[2], cur_flip_y[2], cur_flip_z[2];

    //neurons
    QList<NeuronTree> * ntList;
    QList<NeuronTree *> ntpList;
    QList<NeuronTree> nt_bkList;
    QList<QList<int> > type_bk;
    int ant; // 0 or 1, the id of the neuron tree that is under operation
    int stack_dir; // 0/1/2/3=x/y/z/free

private:
    void resetInternalStates();
    void resetInternalStates(int i);
    void create();
    void updateContent();

public slots:
    void change_ant(int idx);
    void change_stackdir(int dir);
    void update_boundingbox();
    void highlight_matchpoint(double s);//multiple neuron compatible
    void highlight_boundpoint(double s);//multiple neuron compatible
    void highlight_matchpoint_check(int c);//multiple neuron compatible
    void highlight_boundpoint_check(int c);//multiple neuron compatible
    void change_neurontype();
    void resetreject();
    void resetaccept();
    void reset();
    void quickmove();
    void shift_x(double s);
    void shift_y(double s);
    void shift_z(double s);
    void scale_x(double s);
    void scale_y(double s);
    void scale_z(double s);
    void gscale_x(double s);
    void gscale_y(double s);
    void gscale_z(double s);
    void scale_r(double s);
    void rotate_around_x(int v);
    void rotate_around_y(int v);
    void rotate_around_z(int v);
    void flip_x(int v);
    void flip_y(int v);
    void flip_z(int v);

public:
    void highlight_points(bool force);
};

void highlight_dial(QDial *d);
double NORMALIZE_ROTATION_AngleStep( double  angle );

#endif // NEURON_GEOMETRY_DIALOG_H
