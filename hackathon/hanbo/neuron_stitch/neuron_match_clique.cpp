/****************************************************************************
**
** neuron_match_clique.cpp
** by Hanbo Chen 2014.10.16
**
****************************************************************************/

#include "neuron_match_clique.h"
#include "../../../v3d_main/neuron_editing/neuron_xforms.h"
#include <iostream>
#include <vector>
#include "marker_match_dialog.h"

#define CANDMS_ENTRY(a,b) (candMS[(a)+(b)*MS_x])

NeuronMatchOnlyDialog::NeuronMatchOnlyDialog(NeuronTree *nt_in0, NeuronTree *nt_in1, LandmarkList *mList_in, double affineParam[7])
{
    nt1=nt_in1;
    nt0=nt_in0;
    mList=mList_in;
    affine=affineParam;

    creat();
}

void NeuronMatchOnlyDialog::creat()
{
    gridLayout = new QGridLayout();

    //matching zone
    cb_dir = new QComboBox(); cb_dir->addItem("x"); cb_dir->addItem("y"); cb_dir->addItem("z");
    cb_dir->setCurrentIndex(2);
    spin_zscale = new QDoubleSpinBox();
    spin_zscale->setRange(0,100000); spin_zscale->setValue(1);
    spin_ang = new QDoubleSpinBox();
    spin_ang->setRange(0,180); spin_ang->setValue(60);
    spin_matchdis = new QDoubleSpinBox();
    spin_matchdis->setRange(0,100000); spin_matchdis->setValue(100);
    spin_searchspan = new QDoubleSpinBox();
    spin_searchspan->setRange(0,100000); spin_searchspan->setValue(20);
    spin_cmatchdis = new QDoubleSpinBox();
    spin_cmatchdis->setRange(0,100000); spin_cmatchdis->setValue(100);
    spin_segthr = new QDoubleSpinBox();
    spin_segthr->setRange(0,100000); spin_segthr->setValue(0);
    spin_spineLen = new QSpinBox();
    spin_spineLen->setRange(0,100000); spin_spineLen->setValue(5); spin_spineLen->setEnabled(false);
    spin_spineAng = new QDoubleSpinBox();
    spin_spineAng->setRange(0,180); spin_spineAng->setValue(30); spin_spineAng->setEnabled(false);
    spin_spineRadius = new QDoubleSpinBox();
    spin_spineRadius->setRange(0,100000); spin_spineRadius->setValue(3); spin_spineRadius->setEnabled(false);
    check_spine = new QCheckBox("filter spines when matching:");
    check_spine -> setChecked(false);
    connect(check_spine, SIGNAL(stateChanged(int)), this, SLOT(spineCheck(int)));
    QLabel* label_0 = new QLabel("stacking direction: ");
    gridLayout->addWidget(label_0,9,0,1,2);
    gridLayout->addWidget(cb_dir,9,2,1,1);
    QLabel* label_1 = new QLabel("resacle stacking direction: ");
    gridLayout->addWidget(label_1,9,3,1,2);
    gridLayout->addWidget(spin_zscale,9,5,1,1);
    QLabel* label_2 = new QLabel("Max angular to match points (0~180): ");
    gridLayout->addWidget(label_2,10,0,1,2);
    gridLayout->addWidget(spin_ang,10,2,1,1);
    QLabel* label_3 = new QLabel("Max distance to match points: ");
    gridLayout->addWidget(label_3,10,3,1,2);
    gridLayout->addWidget(spin_matchdis,10,5,1,1);
    QLabel* label_4 = new QLabel("match candidates searching span: ");
    gridLayout->addWidget(label_4,11,0,1,2);
    gridLayout->addWidget(spin_searchspan,11,2,1,1);
    QLabel* label_5 = new QLabel("Max distance to match 3-clique: ");
    gridLayout->addWidget(label_5,11,3,1,2);
    gridLayout->addWidget(spin_cmatchdis,11,5,1,1);
    QLabel* label_6 = new QLabel("small segment filter (0=keep all): ");
    gridLayout->addWidget(label_6,12,0,1,2);
    gridLayout->addWidget(spin_segthr,12,2,1,1);
    gridLayout->addWidget(check_spine,13,0,1,2);
    QLabel* label_7 = new QLabel("point #:");
    gridLayout->addWidget(label_7,14,0,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineLen,14,1,1,1);
    QLabel* label_8 = new QLabel("turning angle:");
    gridLayout->addWidget(label_8,14,2,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineAng,14,3,1,1);
    QLabel* label_9 = new QLabel("radius:");
    gridLayout->addWidget(label_9,14,4,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineRadius,14,5,1,1);

    btn_match = new QPushButton("Match"); btn_match->setAutoDefault(false);
    connect(btn_match, SIGNAL(clicked()), this, SLOT(match()));
    gridLayout->addWidget(btn_match,15,5,1,1);

    setLayout(gridLayout);
}

void NeuronMatchOnlyDialog::match()
{
    gridLayout->setEnabled(false);
    NeuronTree nt0_run,nt1_run;
    backupNeuron(*nt0,nt0_run);
    backupNeuron(*nt1,nt1_run);
    //init matching function
    neuron_match_clique matchfunc(&nt0_run,&nt1_run);

    //parameters
    matchfunc.direction=cb_dir->currentIndex();
    matchfunc.zscale=spin_zscale->value();
    matchfunc.angThr_match=cos(spin_ang->value()/180*M_PI);
    matchfunc.pmatchThr = spin_matchdis->value();
    matchfunc.spanCand = spin_searchspan->value();
    matchfunc.cmatchThr = spin_cmatchdis->value();
    matchfunc.segmentThr = spin_segthr->value();
    if(check_spine->isChecked()){
        matchfunc.spineLengthThr = spin_spineLen->value();
        matchfunc.spineAngThr = cos(spin_spineAng->value()/180*M_PI);
        matchfunc.spineRadiusThr = spin_spineRadius->value();
    }else{
        matchfunc.spineLengthThr = 0;
        matchfunc.spineRadiusThr = 0;
    }

    //init clique and candidate
    matchfunc.init();

    //global match
    qDebug()<<"start global search";
    matchfunc.globalmatch();

    //get parameter
    affine[0]=matchfunc.shift_x;
    affine[1]=matchfunc.shift_y;
    affine[2]=matchfunc.shift_z;
    affine[3]=matchfunc.rotation_ang;
    affine[4]=matchfunc.rotation_cx;
    affine[5]=matchfunc.rotation_cy;
    affine[6]=matchfunc.rotation_cz;

    //populate markers
    matchfunc.update_matchedPoints_to_Markers(mList);

    //quit
    accept();
}

void NeuronMatchOnlyDialog::spineCheck(int c)
{
    if(check_spine->isChecked()){
        spin_spineLen->setEnabled(true);
        spin_spineAng->setEnabled(true);
        spin_spineRadius->setEnabled(true);
    }else{
        spin_spineLen->setEnabled(false);
        spin_spineAng->setEnabled(false);
        spin_spineRadius->setEnabled(false);
    }
}

NeuronLiveMatchDialog::NeuronLiveMatchDialog(V3DPluginCallback2 * cb, V3dR_MainWindow* inwin)
{
    if(!inwin)
        return;
    v3dwin=inwin;
    callback = cb;
    v3dcontrol = callback->getView3DControl_Any3DViewer(v3dwin);

    ntList=cb->getHandleNeuronTrees_Any3DViewer(v3dwin);
    if(ntList->size()!=2){ //this should not happen
        v3d_msg("The number of neurons in the window is not 2");
        return;
    }
    //reset the color of neuron list
    NeuronTree* ntp = 0;
    ntp = (NeuronTree*)&(ntList->at(0));
    ntp->color.r=ntp->color.g=ntp->color.b=ntp->color.a=0;
    for(int i=0; i<ntList->at(0).listNeuron.size(); i++){
        NeuronSWC *p = (NeuronSWC *)&(ntList->at(0).listNeuron.at(i));
        p->type=2;
    }
    ntp = (NeuronTree*)&(ntList->at(1));
    ntp->color.r=ntp->color.g=ntp->color.b=ntp->color.a=0;
    for(int i=0; i<ntList->at(1).listNeuron.size(); i++){
        NeuronSWC *p = (NeuronSWC *)&(ntList->at(1).listNeuron.at(i));
        p->type=7;
    }

    //back up
    for(int i=0; i<2; i++){
        NeuronTree nt_tmp;
        backupNeuron(ntList->at(i),nt_tmp);
        ntList_bk.append(nt_tmp);
    }

    mList = cb->getHandleLandmarkList_Any3DViewer(v3dwin);

    creat();

    matchfunc = new neuron_match_clique((NeuronTree*)(&ntList->at(0)),(NeuronTree*)(&ntList->at(1)));
    matchfunc->initNeuronComponents();
    matchfunc->update_matchedPoints_from_Markers(mList); //to-do,not implemented yet

    updateview();
}

void NeuronLiveMatchDialog::enterEvent(QEvent *e)
{
    checkwindow();

    QDialog::enterEvent(e);
}


void NeuronLiveMatchDialog::creat()
{
    gridLayout = new QGridLayout();

    //matching zone
    QLabel* label_a = new QLabel("Step 1: match and affine ");
    gridLayout->addWidget(label_a,8,0,1,5);
    btn_match = new QPushButton("Match"); btn_match->setAutoDefault(false);
    connect(btn_match, SIGNAL(clicked()), this, SLOT(match()));
    gridLayout->addWidget(btn_match,8,5,1,1);

    cb_dir = new QComboBox(); cb_dir->addItem("x"); cb_dir->addItem("y"); cb_dir->addItem("z");
    cb_dir->setCurrentIndex(2);
    spin_zscale = new QDoubleSpinBox();
    spin_zscale->setRange(0,100000); spin_zscale->setValue(1);
    spin_ang = new QDoubleSpinBox();
    spin_ang->setRange(0,180); spin_ang->setValue(60);
    spin_matchdis = new QDoubleSpinBox();
    spin_matchdis->setRange(0,100000); spin_matchdis->setValue(100);
    spin_searchspan = new QDoubleSpinBox();
    spin_searchspan->setRange(0,100000); spin_searchspan->setValue(20);
    spin_cmatchdis = new QDoubleSpinBox();
    spin_cmatchdis->setRange(0,100000); spin_cmatchdis->setValue(100);
    spin_segthr = new QDoubleSpinBox();
    spin_segthr->setRange(0,100000); spin_segthr->setValue(0);
    spin_spineLen = new QSpinBox();
    spin_spineLen->setRange(0,100000); spin_spineLen->setValue(5); spin_spineLen->setEnabled(false);
    spin_spineAng = new QDoubleSpinBox();
    spin_spineAng->setRange(0,180); spin_spineAng->setValue(30); spin_spineAng->setEnabled(false);
    spin_spineRadius = new QDoubleSpinBox();
    spin_spineRadius->setRange(0,100000); spin_spineRadius->setValue(3); spin_spineRadius->setEnabled(false);
    check_spine = new QCheckBox("filter spines when matching:");
    check_spine -> setChecked(false);
    connect(check_spine, SIGNAL(stateChanged(int)), this, SLOT(spineCheck(int)));

    QLabel* label_0 = new QLabel("stacking direction: ");
    gridLayout->addWidget(label_0,9,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(cb_dir,9,2,1,1);
    QLabel* label_1 = new QLabel("resacle stacking direction: ");
    gridLayout->addWidget(label_1,9,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_zscale,9,5,1,1);
    QLabel* label_2 = new QLabel("Max angular to match points (0~180): ");
    gridLayout->addWidget(label_2,10,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_ang,10,2,1,1);
    QLabel* label_3 = new QLabel("Max distance to match points: ");
    gridLayout->addWidget(label_3,10,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_matchdis,10,5,1,1);
    QLabel* label_4 = new QLabel("match candidates searching span: ");
    gridLayout->addWidget(label_4,11,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_searchspan,11,2,1,1);
    QLabel* label_5 = new QLabel("Max distance to match 3-clique: ");
    gridLayout->addWidget(label_5,11,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_cmatchdis,11,5,1,1);
    QLabel* label_6 = new QLabel("small segment filter (0=keep all): ");
    gridLayout->addWidget(label_6,12,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_segthr,12,2,1,1);
    gridLayout->addWidget(check_spine,13,0,1,2);
    QLabel* label_7 = new QLabel("point #:");
    gridLayout->addWidget(label_7,14,0,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineLen,14,1,1,1);
    QLabel* label_8 = new QLabel("turning angle:");
    gridLayout->addWidget(label_8,14,2,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineAng,14,3,1,1);
    QLabel* label_9 = new QLabel("radius:");
    gridLayout->addWidget(label_9,14,4,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineRadius,14,5,1,1);

    //stitching zone
    int stitchrow=15;
    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    line_1->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_1,stitchrow,0,1,6);
    QLabel* label_b = new QLabel("Step 2: stitch paired points");
    gridLayout->addWidget(label_b,stitchrow+1,0,1,5);
    cb_pair = new QComboBox();
    gridLayout->addWidget(cb_pair,stitchrow+2,0,1,2);
    connect(cb_pair, SIGNAL(currentIndexChanged(int)), this, SLOT(change_pair(int)));
    btn_manualmatch = new QPushButton("Manually Add"); btn_manualmatch->setAutoDefault(false);
    connect(btn_manualmatch, SIGNAL(clicked()), this, SLOT(manualadd()));
    gridLayout->addWidget(btn_manualmatch,stitchrow+2,2,1,1);
    btn_skip = new QPushButton("Skip"); btn_skip->setAutoDefault(false);
    connect(btn_skip, SIGNAL(clicked()), this, SLOT(skip()));
    gridLayout->addWidget(btn_skip,stitchrow+2,3,1,1);
    btn_stitch = new QPushButton("Stitch"); btn_stitch->setAutoDefault(false);
    connect(btn_stitch, SIGNAL(clicked()), this, SLOT(stitch()));
    gridLayout->addWidget(btn_stitch,stitchrow+2,4,1,1);
    btn_stitchall = new QPushButton("Stitch All"); btn_stitchall->setAutoDefault(false);
    connect(btn_stitchall, SIGNAL(clicked()), this, SLOT(stitchall()));
    gridLayout->addWidget(btn_stitchall,stitchrow+2,5,1,1);

    btn_stitch->setEnabled(false);
    btn_stitchall->setEnabled(false);
    btn_skip->setEnabled(false);

    //operation zone
    int optrow=18;
    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_2,optrow,0,1,6);
    btn_output = new QPushButton("Save"); btn_output->setAutoDefault(false);
    gridLayout->addWidget(btn_output,optrow+1,4,1,1);
    connect(btn_output,     SIGNAL(clicked()), this, SLOT(output()));
    btn_quit = new QPushButton("Quit"); btn_quit->setAutoDefault(false);
    connect(btn_quit,     SIGNAL(clicked()), this, SLOT(reject()));
    gridLayout->addWidget(btn_quit,optrow+1,5,1,1);

    setLayout(gridLayout);
}


void NeuronLiveMatchDialog::spineCheck(int c)
{
    if(check_spine->isChecked()){
        spin_spineLen->setEnabled(true);
        spin_spineAng->setEnabled(true);
        spin_spineRadius->setEnabled(true);
    }else{
        spin_spineLen->setEnabled(false);
        spin_spineAng->setEnabled(false);
        spin_spineRadius->setEnabled(false);
    }
}

void NeuronLiveMatchDialog::checkwindow()
{
    //check if current window is closed
    if (!callback){
        this->hide();
        return;
    }

    bool isclosed = true;
    //search to see if the window is still open
    QList <V3dR_MainWindow *> allWindowList = callback->getListAll3DViewers();
    for (V3DLONG i=0;i<allWindowList.size();i++)
    {
        if(allWindowList.at(i)==v3dwin){
            isclosed = false;
            break;
        }
    }

    //close the window
    if(isclosed){
        this->hide();
        return;
    }
}

void NeuronLiveMatchDialog::updateview()
{
    checkwindow();

    if(v3dwin){
        callback->update_3DViewer(v3dwin);
    }

    if(v3dcontrol){
        v3dcontrol->updateLandmark();
    }
}

void NeuronLiveMatchDialog::match()
{
    checkwindow();

    //clean up dialog storage:
    pmatch0.clear();
    pmatch1.clear();
    mmatch0.clear();
    mmatch1.clear();
    stitchmask.clear();
    mList->clear();
    cur_pair=-1;

    //retrive backup
    for(int i=0; i<2; i++){
        copyCoordinate(ntList_bk.at(i),ntList->at(i));
        copyProperty(ntList_bk.at(i),ntList->at(i));
    }

    updateview();

    matchfunc = new neuron_match_clique((NeuronTree*)(&ntList->at(0)),(NeuronTree*)(&ntList->at(1)));

    //parameters
    matchfunc->direction=cb_dir->currentIndex();
    matchfunc->zscale=spin_zscale->value();
    matchfunc->angThr_match=cos(spin_ang->value()/180*M_PI);
    matchfunc->pmatchThr = spin_matchdis->value();
    matchfunc->spanCand = spin_searchspan->value();
    matchfunc->cmatchThr = spin_cmatchdis->value();
    matchfunc->segmentThr = spin_segthr->value();
    if(check_spine->isChecked()){
        matchfunc->spineLengthThr = spin_spineLen->value();
        matchfunc->spineAngThr = cos(spin_spineAng->value()/180*M_PI);
        matchfunc->spineRadiusThr = spin_spineRadius->value();
    }else{
        matchfunc->spineLengthThr = 0;
        matchfunc->spineRadiusThr = 0;
    }

    //init clique and candidate
    matchfunc->init();

    //global match
    qDebug()<<"start global search";
    matchfunc->globalmatch();

    //update markers
    matchfunc->update_matchedPoints_to_Markers(mList);
    qDebug()<<"update makers: "<<mList->size();

    cur_pair=0;

    callback->update_NeuronBoundingBox(v3dwin);
    updateview();
    updatematchlist();
}

void NeuronLiveMatchDialog::updatematchlist()
{
    QList<int> mm0, mm1;
    pmatch0.clear();
    pmatch1.clear();
    mmatch0.clear();
    mmatch1.clear();
    stitchmask.clear();
    cb_pair->clear();
    int info[4];

    for(int i=0; i<mList->size(); i++){
        if(get_marker_info(mList->at(i),info)){
            if(info[2]>=0){ //matched marker
                if(info[2]>i && info[2]<mList->size()){
                    if(info[0]==0){
                        mm0.append(i);
                        mm1.append(info[2]);
                    }else{
                        mm0.append(info[2]);
                        mm1.append(i);
                    }
                }
            }
        }
    }
    for(int i=0; i<mm0.size(); i++){
        //get the matched point on neuron
        get_marker_info(mList->at(mm0[i]),info);
        if(info[1]>=ntList->at(0).listNeuron.size()) continue;
        int tmp=info[1];
        get_marker_info(mList->at(mm1[i]),info);
        if(info[1]>=ntList->at(1).listNeuron.size()) continue;
        pmatch0.append(tmp);
        pmatch1.append(info[1]);
        mmatch0.append(mm0[i]);
        mmatch1.append(mm1[i]);

        //check loop or sititched
        if(ntList->at(0).listNeuron.at(pmatch0.last()).x==ntList->at(1).listNeuron.at(pmatch1.last()).x &&
                ntList->at(0).listNeuron.at(pmatch0.last()).y==ntList->at(1).listNeuron.at(pmatch1.last()).y &&
                ntList->at(0).listNeuron.at(pmatch0.last()).z==ntList->at(1).listNeuron.at(pmatch1.last()).z)
            stitchmask.append(1);
        else if(matchfunc->checkloop(pmatch0.last(), pmatch1.last()))
            stitchmask.append(-1); //cannot stitch
        else
            stitchmask.append(0);

        if(stitchmask.last()<0)
            cb_pair->addItem("Marker: " + QString::number(mm0[i]+1) + " x Marker: " + QString::number(mm1[i]+1) +" = LOOP!");
        else if(stitchmask.last()==0)
            cb_pair->addItem("Marker: " + QString::number(mm0[i]+1) + " x Marker: " + QString::number(mm1[i]+1) );
        else
            cb_pair->addItem("Marker: " + QString::number(mm0[i]+1) + " x Marker: " + QString::number(mm1[i]+1) +" = stiched!");

    }

    if(pmatch0.size()>0){
        int id=cb_pair->currentIndex();
        btn_stitchall->setEnabled(true);
        btn_skip->setEnabled(true);
        btn_stitch->setEnabled(stitchmask.at(id)>=0);
        cur_pair=id;
    }
}

void NeuronLiveMatchDialog::highlight_pair()
{
    if(cb_pair->count()>0){
        int info[4];
        LocationSimple * SP = 0;

        //reset cur_pair
        if(cur_pair>=0 && cur_pair<mmatch0.size()){
            SP=(LocationSimple*)&(mList->at(mmatch0[cur_pair]));
            SP->color.r = 255; SP->color.g = 0; SP->color.b = 0;
            //update location
            get_marker_info(*SP, info);
            SP->x = ntList->at(0).listNeuron.at(info[1]).x;
            SP->y = ntList->at(0).listNeuron.at(info[1]).y;
            SP->z = ntList->at(0).listNeuron.at(info[1]).z;

            SP=(LocationSimple*)&(mList->at(mmatch1[cur_pair]));
            SP->color.r = 0; SP->color.g = 255; SP->color.b = 0;
            //update location
            get_marker_info(*SP, info);
            SP->x = ntList->at(1).listNeuron.at(info[1]).x;
            SP->y = ntList->at(1).listNeuron.at(info[1]).y;
            SP->z = ntList->at(1).listNeuron.at(info[1]).z;


            if(stitchmask.at(cur_pair)>0)
                matchfunc->highlight_nt1_seg(pmatch1[cur_pair],2);
            else
                matchfunc->highlight_nt1_seg(pmatch1[cur_pair],7);
            matchfunc->highlight_nt0_seg(pmatch0[cur_pair],2);
        }

        cur_pair=cb_pair->currentIndex();

        //highlight new pair
        SP=(LocationSimple*)&(mList->at(mmatch0[cur_pair]));
        SP->color.r = 255; SP->color.g = 255; SP->color.b = 128;
        //update location
        get_marker_info(*SP, info);
        SP->x = ntList->at(0).listNeuron.at(info[1]).x;
        SP->y = ntList->at(0).listNeuron.at(info[1]).y;
        SP->z = ntList->at(0).listNeuron.at(info[1]).z;

        SP=(LocationSimple*)&(mList->at(mmatch1[cur_pair]));
        SP->color.r = 255; SP->color.g = 255; SP->color.b = 128;
        //update location
        get_marker_info(*SP, info);
        SP->x = ntList->at(1).listNeuron.at(info[1]).x;
        SP->y = ntList->at(1).listNeuron.at(info[1]).y;
        SP->z = ntList->at(1).listNeuron.at(info[1]).z;

        if(stitchmask.at(cur_pair)<=0){
            //for test
            qDebug()<<"cojoc: "<<stitchmask.at(cur_pair)<<":"<<cur_pair;
            matchfunc->highlight_nt1_seg(pmatch1[cur_pair],5);
            matchfunc->highlight_nt0_seg(pmatch0[cur_pair],4);
        }

        updateview();
    }
}

void NeuronLiveMatchDialog::change_pair(int idx)
{
    checkwindow();

    highlight_pair();

    btn_stitch->setEnabled(stitchmask.at(idx)==0);
}

void NeuronLiveMatchDialog::manualadd()
{
    //recolor corrent one
    if(cb_pair->count()>0){
        int info[4];
        LocationSimple * SP = 0;

        //reset cur_pair
        if(cur_pair>=0 && cur_pair<mmatch0.size()){
            SP=(LocationSimple*)&(mList->at(mmatch0[cur_pair]));
            SP->color.r = 255; SP->color.g = 0; SP->color.b = 0;
            //update location
            get_marker_info(*SP, info);
            SP->x = ntList->at(0).listNeuron.at(info[1]).x;
            SP->y = ntList->at(0).listNeuron.at(info[1]).y;
            SP->z = ntList->at(0).listNeuron.at(info[1]).z;

            SP=(LocationSimple*)&(mList->at(mmatch1[cur_pair]));
            SP->color.r = 0; SP->color.g = 255; SP->color.b = 0;
            //update location
            get_marker_info(*SP, info);
            SP->x = ntList->at(1).listNeuron.at(info[1]).x;
            SP->y = ntList->at(1).listNeuron.at(info[1]).y;
            SP->z = ntList->at(1).listNeuron.at(info[1]).z;


            if(stitchmask.at(cur_pair)>0)
                matchfunc->highlight_nt1_seg(pmatch1[cur_pair],2);
            else
                matchfunc->highlight_nt1_seg(pmatch1[cur_pair],7);
            matchfunc->highlight_nt0_seg(pmatch0[cur_pair],2);
        }
    }

    link_new_marker_neuron();

    marker_match_dialog dialog(callback, mList);
    dialog.exec();

    updatematchlist();

    if(cb_pair->count()>0)
        cb_pair->setCurrentIndex(cb_pair->count()-1);

    checkwindow();
}

void NeuronLiveMatchDialog::link_new_marker_neuron()
{
    if(mList->size()<=0){
        return;
    }

    LocationSimple *p = 0;
    double dis;
    for(int i=0; i<mList->size(); i++){
        int info[4];
        double mdis=1e10;
        if(get_marker_info(mList->at(i),info))
            continue;
        int nid=-1;
        int pid=0;

        for(int j=0; j<2; j++){
            for(int k=0; k<ntList->at(j).listNeuron.size(); k++){
                dis=NTDIS(ntList->at(j).listNeuron[k],mList->at(i));
                if(dis<mdis){
                    mdis=dis;
                    nid=j;
                    pid=k;
                }
            }
        }
        p = (LocationSimple *)&(mList->at(i));
        p->name=QString::number(i).toStdString();
        if(nid>=0){
            QString tmp = QString::number(nid) + " " + QString::number(pid) + " -1";
            p->comments=tmp.toStdString();
            if(nid==0){
                p->color.r = 128;
                p->color.g = 0;
                p->color.b = 128;
            }else{
                p->color.r = 0;
                p->color.g = 128;
                p->color.b = 128;
            }
        }
    }

    updateview();
}

void NeuronLiveMatchDialog::skip()
{
    checkwindow();
    if(cb_pair->currentIndex() < cb_pair->count()-1)
        cb_pair->setCurrentIndex(cb_pair->currentIndex()+1);
    else{
        v3d_msg("Reach the end. Restart from the beginnig.");
        cb_pair->setCurrentIndex(0);
    }
}

void NeuronLiveMatchDialog::stitch()
{
    checkwindow();

    int idx=cb_pair->currentIndex();
    if(matchfunc->stitch(pmatch0.at(idx),pmatch1.at(idx))){
        stitchmask[idx]=1;
        cb_pair->setItemText(idx, cb_pair->currentText() + " = stitched!");
    }else{
        stitchmask[idx]=-1;
        cb_pair->setItemText(idx, cb_pair->currentText() + " = failed, LOOP!");
    }

    if(cb_pair->currentIndex() < cb_pair->count()-1)
        cb_pair->setCurrentIndex(cb_pair->currentIndex()+1);
    else{
        v3d_msg("Reach the end. Restart from the beginnig.");
        cb_pair->setCurrentIndex(0);
    }
}

void NeuronLiveMatchDialog::stitchall()
{
    checkwindow();

    for(int idx=0; idx<stitchmask.size(); idx++){
        if(stitchmask[idx] != 0)
            continue;
        if(matchfunc->stitch(pmatch0.at(idx),pmatch1.at(idx))){
            stitchmask[idx]=1;
            cb_pair->setItemText(idx, cb_pair->itemText(idx) + " = stitched!");

            //update the location of markers
            LocationSimple* SP=(LocationSimple*)&(mList->at(mmatch0.at(idx)));
            int info[4];
            //neuron 0
            get_marker_info(*SP, info);
            SP->x = ntList->at(0).listNeuron.at(info[1]).x;
            SP->y = ntList->at(0).listNeuron.at(info[1]).y;
            SP->z = ntList->at(0).listNeuron.at(info[1]).z;

            SP=(LocationSimple*)&(mList->at(mmatch1.at(idx)));
            //neuron 1
            get_marker_info(*SP, info);
            SP->x = ntList->at(1).listNeuron.at(info[1]).x;
            SP->y = ntList->at(1).listNeuron.at(info[1]).y;
            SP->z = ntList->at(1).listNeuron.at(info[1]).z;

            matchfunc->highlight_nt1_seg(pmatch1.at(idx),2);
        }else{
            stitchmask[idx]=-1;
            cb_pair->setItemText(idx, cb_pair->itemText(idx) + " = failed, LOOP!");
        }
    }

    if(cur_pair!=0){
        if(cb_pair->count()>0)
            cb_pair->setCurrentIndex(0);
    }else{
        btn_stitch->setEnabled(stitchmask[0] == 0);
        highlight_pair();
    }
    updateview();
}

void NeuronLiveMatchDialog::output()
{
    checkwindow();

    QString folder_output = QFileDialog::getExistingDirectory(this, "Select Output Folder","");
    if(folder_output.isEmpty())
        return;
    bool ok;
    QString fname_base = QInputDialog::getText(this, "Output Prefix","Assign a prefix for all outputs",QLineEdit::Normal,"",&ok);
    if(!ok)
        return;
    QString fname_output = QDir(folder_output).filePath(fname_base);

    matchfunc->output_candMatchScore(fname_output + "_matchscore.txt");
    matchfunc->output_affine(fname_output,ntList->at(0).name);
    matchfunc->output_matchedMarkers_orgspace(QDir(folder_output).filePath(fname_base + "_nt0_matched.marker"),QDir(folder_output).filePath(fname_base + "_nt1_matched.marker"));
    matchfunc->output_parameter(fname_output+"_param.txt");
    matchfunc->output_stitch(fname_output);
}

NeuronMatchDialog::NeuronMatchDialog()
{
    nt0 = nt1 = NULL;
    folder_output.clear();
    fname_output.clear();
    name_nt0.clear();
    name_nt1.clear();
    fname_nt0.clear();
    fname_nt1.clear();

    creat();
}

void NeuronMatchDialog::creat()
{
    gridLayout = new QGridLayout();

    //input zone
    label_load0 = new QLabel(QObject::tr("First Neuron for stitch (smaller in stacking direction):"));
    gridLayout->addWidget(label_load0,0,0,1,6);
    edit_load0 = new QLineEdit();
    edit_load0->setText(fname_nt0); edit_load0->setReadOnly(true);
    gridLayout->addWidget(edit_load0,1,0,1,5);
    btn_load0 = new QPushButton("...");
    gridLayout->addWidget(btn_load0,1,5,1,1);

    label_load1 = new QLabel(QObject::tr("Second Neuron for stitch (larger in stacking direction):"));
    gridLayout->addWidget(label_load1,2,0,1,6);
    edit_load1 = new QLineEdit();
    edit_load1->setText(fname_nt1); edit_load1->setReadOnly(true);
    gridLayout->addWidget(edit_load1,3,0,1,5);
    btn_load1 = new QPushButton("...");
    gridLayout->addWidget(btn_load1,3,5,1,1);

    label_output = new QLabel(QObject::tr("Output Direction"));
    gridLayout->addWidget(label_output,4,0,1,6);
    edit_output = new QLineEdit();
    edit_output->setText(fname_output);
    gridLayout->addWidget(edit_output,5,0,1,5);
    btn_output = new QPushButton("...");
    gridLayout->addWidget(btn_output,5,5,1,1);

    connect(btn_load0, SIGNAL(clicked()), this, SLOT(load0()));
    connect(btn_load1, SIGNAL(clicked()), this, SLOT(load1()));
    connect(btn_output, SIGNAL(clicked()), this, SLOT(output()));
    connect(edit_output, SIGNAL(textChanged(QString)), this, SLOT(outputchange(QString)));

    //parameter zone
    QFrame *line_1 = new QFrame();
    line_1->setFrameShape(QFrame::HLine);
    line_1->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_1,8,0,1,6);

    cb_dir = new QComboBox(); cb_dir->addItem("x"); cb_dir->addItem("y"); cb_dir->addItem("z");
    cb_dir->setCurrentIndex(2);
    spin_zscale = new QDoubleSpinBox();
    spin_zscale->setRange(0,100000); spin_zscale->setValue(1);
    spin_ang = new QDoubleSpinBox();
    spin_ang->setRange(0,180); spin_ang->setValue(60);
    spin_matchdis = new QDoubleSpinBox();
    spin_matchdis->setRange(0,100000); spin_matchdis->setValue(100);
    spin_searchspan = new QDoubleSpinBox();
    spin_searchspan->setRange(0,100000); spin_searchspan->setValue(20);
    spin_cmatchdis = new QDoubleSpinBox();
    spin_cmatchdis->setRange(0,100000); spin_cmatchdis->setValue(100);
    spin_segthr = new QDoubleSpinBox();
    spin_segthr->setRange(0,100000); spin_segthr->setValue(0);
    spin_spineLen = new QSpinBox();
    spin_spineLen->setRange(0,100000); spin_spineLen->setValue(5); spin_spineLen->setEnabled(false);
    spin_spineAng = new QDoubleSpinBox();
    spin_spineAng->setRange(0,180); spin_spineAng->setValue(30); spin_spineAng->setEnabled(false);
    spin_spineRadius = new QDoubleSpinBox();
    spin_spineRadius->setRange(0,100000); spin_spineRadius->setValue(3); spin_spineRadius->setEnabled(false);
    check_spine = new QCheckBox("filter spines when matching:");
    check_spine -> setChecked(false);
    connect(check_spine, SIGNAL(stateChanged(int)), this, SLOT(spineCheck(int)));
    QLabel* label_0 = new QLabel("stacking direction: ");
    gridLayout->addWidget(label_0,9,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(cb_dir,9,2,1,1);
    QLabel* label_1 = new QLabel("resacle stacking direction: ");
    gridLayout->addWidget(label_1,9,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_zscale,9,5,1,1);
    QLabel* label_2 = new QLabel("Max angular to match points (0~180): ");
    gridLayout->addWidget(label_2,10,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_ang,10,2,1,1);
    QLabel* label_3 = new QLabel("Max distance to match points: ");
    gridLayout->addWidget(label_3,10,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_matchdis,10,5,1,1);
    QLabel* label_4 = new QLabel("match candidates searching span: ");
    gridLayout->addWidget(label_4,11,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_searchspan,11,2,1,1);
    QLabel* label_5 = new QLabel("Max distance to match 3-clique: ");
    gridLayout->addWidget(label_5,11,3,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_cmatchdis,11,5,1,1);
    QLabel* label_6 = new QLabel("small segment filter (0=keep all): ");
    gridLayout->addWidget(label_6,12,0,1,2,Qt::AlignRight);
    gridLayout->addWidget(spin_segthr,12,2,1,1);
    gridLayout->addWidget(check_spine,13,0,1,2);
    QLabel* label_7 = new QLabel("point #:");
    gridLayout->addWidget(label_7,14,0,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineLen,14,1,1,1);
    QLabel* label_8 = new QLabel("turning angle:");
    gridLayout->addWidget(label_8,14,2,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineAng,14,3,1,1);
    QLabel* label_9 = new QLabel("radius:");
    gridLayout->addWidget(label_9,14,4,1,1,Qt::AlignRight);
    gridLayout->addWidget(spin_spineRadius,14,5,1,1);

    //operation zone
    int optrow=15;
    QFrame *line_2 = new QFrame();
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);
    gridLayout->addWidget(line_2,15,0,1,6);
    check_stitch = new QCheckBox("Stitch Matched Points");
    gridLayout->addWidget(check_stitch,16,0,1,2);
    btn_run = new QPushButton("Run");
    connect(btn_run,     SIGNAL(clicked()), this, SLOT(run()));
    gridLayout->addWidget(btn_run,16,4,1,1);
    btn_quit = new QPushButton("Quit");
    connect(btn_quit,     SIGNAL(clicked()), this, SLOT(reject()));
    gridLayout->addWidget(btn_quit,16,5,1,1);

    btn_run->setEnabled(false);

    setLayout(gridLayout);
}

bool NeuronMatchDialog::load0()
{
    QString fileOpenName;
    fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open File"),
            "",
            QObject::tr("Supported file (*.swc *.eswc)"
                ";;Neuron structure	(*.swc)"
                ";;Extended neuron structure (*.eswc)"
                ));
    if(fileOpenName.isEmpty())
        return false;
    NeuronTree* nt = new NeuronTree();
    if (fileOpenName.toUpper().endsWith(".SWC") || fileOpenName.toUpper().endsWith(".ESWC"))
    {
        *nt = readSWC_file(fileOpenName);
    }
    if(nt->listNeuron.size()<=0){
        v3d_msg("failed to read SWC file: "+fileOpenName);
        return false;
    }
    nt0 = nt;
    fname_nt0 = fileOpenName;
    name_nt0 = QFileInfo(fileOpenName).baseName();
    edit_load0->setText(fname_nt0);

    if(fname_nt0.length() * fname_nt1.length() != 0){
        fname_output = QDir(folder_output).filePath(name_nt1 + "_to_" + name_nt0);
    }

    QDir::setCurrent(QFileInfo(fileOpenName).absoluteFilePath());

    if(fname_nt0.length() * fname_nt1.length() * folder_output.length() != 0){
        btn_run->setEnabled(true);
        edit_output->setText(fname_output);
    }

    return true;
}

bool NeuronMatchDialog::load1()
{
    QString fileOpenName;
    fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open File"),
            "",
            QObject::tr("Supported file (*.swc *.eswc)"
                ";;Neuron structure	(*.swc)"
                ";;Extended neuron structure (*.eswc)"
                ));
    if(fileOpenName.isEmpty())
        return false;
    NeuronTree* nt = new NeuronTree();
    if (fileOpenName.toUpper().endsWith(".SWC") || fileOpenName.toUpper().endsWith(".ESWC"))
    {
        *nt = readSWC_file(fileOpenName);
    }
    if(nt->listNeuron.size()<=0){
        v3d_msg("failed to read SWC file: "+fileOpenName);
        return false;
    }
    nt1 = nt;
    fname_nt1 = fileOpenName;
    name_nt1 = QFileInfo(fileOpenName).baseName();
    edit_load1->setText(fname_nt1);

    if(fname_nt0.length() * fname_nt1.length() != 0){
        fname_output = QDir(folder_output).filePath(name_nt1 + "_to_" + name_nt0);
    }

    QDir::setCurrent(QFileInfo(fileOpenName).absoluteFilePath());

    if(fname_nt0.length() * fname_nt1.length() * folder_output.length() != 0){
        btn_run->setEnabled(true);
        edit_output->setText(fname_output);
    }

    return true;
}

bool NeuronMatchDialog::output()
{
    QString fileSaveDir;
    fileSaveDir = QFileDialog::getExistingDirectory(0, QObject::tr("Select Directory to Save Results"),
            "~",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(fileSaveDir.isEmpty())
        return false;

    folder_output = fileSaveDir;

    fname_output = QDir(folder_output).filePath(name_nt1 + "_to_" + name_nt0);
    edit_output->setText(fname_output);

    QDir::setCurrent(fileSaveDir);

    if(fname_nt0.length() * fname_nt1.length() * folder_output.length() != 0){
        btn_run->setEnabled(true);
    }

    return true;
}

void NeuronMatchDialog::outputchange(QString text)
{
    fname_output=text;
}

void NeuronMatchDialog::run()
{
    gridLayout->setEnabled(false);
    //init matching function
    neuron_match_clique matchfunc(nt0,nt1);

    //parameters
    matchfunc.direction=cb_dir->currentIndex();
    matchfunc.zscale=spin_zscale->value();
    matchfunc.angThr_match=cos(spin_ang->value()/180*M_PI);
    matchfunc.pmatchThr = spin_matchdis->value();
    matchfunc.spanCand = spin_searchspan->value();
    matchfunc.cmatchThr = spin_cmatchdis->value();
    matchfunc.segmentThr = spin_segthr->value();
    if(check_spine->isChecked()){
        matchfunc.spineLengthThr = spin_spineLen->value();
        matchfunc.spineAngThr = cos(spin_spineAng->value()/180*M_PI);
        matchfunc.spineRadiusThr = spin_spineRadius->value();
    }else{
        matchfunc.spineLengthThr = 0;
        matchfunc.spineRadiusThr = 0;
    }


    //init clique and candidate
    matchfunc.init();

    //global match
    qDebug()<<"start global search";
    matchfunc.globalmatch();

    //stitch if checked
    if(check_stitch->isChecked()){
        qDebug()<<"stitch matched points";
        matchfunc.stitch();
        matchfunc.output_stitch(fname_output);
    }

    //iterative local match
    matchfunc.output_candMatchScore(fname_output + "_matchscore.txt");
    matchfunc.output_affine(fname_output,fname_nt0);
    matchfunc.output_matchedMarkers_orgspace(QDir(folder_output).filePath(name_nt0 + "_matched.marker"),QDir(folder_output).filePath(name_nt1 + "_matched.marker"));
    matchfunc.output_parameter(fname_output+"_param.txt");

    gridLayout->setEnabled(true);
    v3d_msg("matching finished");
}

void NeuronMatchDialog::spineCheck(int c)
{
    if(check_spine->isChecked()){
        spin_spineLen->setEnabled(true);
        spin_spineAng->setEnabled(true);
        spin_spineRadius->setEnabled(true);
    }else{
        spin_spineLen->setEnabled(false);
        spin_spineAng->setEnabled(false);
        spin_spineRadius->setEnabled(false);
    }
}

neuron_match_clique::neuron_match_clique(NeuronTree* botNeuron, NeuronTree* topNeuron)
{
    nt0_stitch=botNeuron;
    nt1_stitch=topNeuron;

    nt0=new NeuronTree;
    nt0_org=new NeuronTree;
    nt1=new NeuronTree;
    nt1_org=new NeuronTree;
    nt1_a=new NeuronTree;

    backupNeuron(*nt0_stitch,*nt0);
    backupNeuron(*nt1_stitch,*nt1);
    backupNeuron(*nt0_stitch,*nt0_org);
    backupNeuron(*nt1_stitch,*nt1_org);
    backupNeuron(*nt1_stitch,*nt1_a);

    constructNeuronGraph(*nt0, ng0);
    constructNeuronGraph(*nt1, ng1);

    spanCand = 20;
    direction = 2;
    midplane = 0;
    angThr_match = cos(M_PI/3);
    angThr_stack = -1;
    dir_range = 100;
    cmatchThr = 100;
    pmatchThr = 100;
    zscale = 1;
    segmentThr = 0;
    spineLengthThr = 0;
    spineAngThr = -1;
    spineRadiusThr = 0;
}

void neuron_match_clique::globalmatch()
{
    if(rankedCliqueMatch.size()<=0)
        return;

    candmatch0.clear();
    candmatch1.clear();

    //search for best unconflict groups of matched points
    double bestEnergy = -1;
    double better_shift_x=0,better_shift_y=0,better_shift_z=0,better_angle=0,better_cent_x=0,better_cent_y=0,better_cent_z=0;

    multimap<int, QVector<int> , std::greater<int> >::iterator iter_conflict=rankedCliqueMatch.begin();
    //try 10 times, accept best one if no better one can be found with current trial
    for(int trial=0; trial<candID0.size()+candID1.size(); trial++){
        if(iter_conflict==rankedCliqueMatch.end()) break;
        //construct in-conflict matching pairs
        QList<int> tmpmatch0, tmpmatch1;
        int c0 = iter_conflict->second.at(0);//.value().at(0);
        int c1 = iter_conflict->second.at(1);
        tmpmatch0.append(cliqueList0.at(c0).idx[0]);
        tmpmatch0.append(cliqueList0.at(c0).idx[1]);
        tmpmatch0.append(cliqueList0.at(c0).idx[2]);
        tmpmatch1.append(cliqueList1.at(c1).idx[0]);
        tmpmatch1.append(cliqueList1.at(c1).idx[1]);
        tmpmatch1.append(cliqueList1.at(c1).idx[2]);
        iter_conflict++;
//This method will push inconflict matching point based on clique match as much as possible first.
//Marked this method because it will be unstable and cause error result
//        int reachConflict = 0; //not reached
//        for(multimap<int, QVector<int> , std::greater<int> >::iterator iter_cpair=rankedCliqueMatch.begin(); iter_cpair != rankedCliqueMatch.end(); iter_cpair++){
//            if(iter_cpair == iter_conflict){
//                reachConflict = 1; //reached, but no new confliction so far
//                continue;
//            }
//            c0=iter_cpair->second.at(0);
//            c1=iter_cpair->second.at(1);
//            //check confliction
//            bool conflict = false;
//            if(!conflict && tmpmatch0.contains(cliqueList0.at(c0).idx[0])){
//                if(tmpmatch1.at(tmpmatch0.indexOf(cliqueList0.at(c0).idx[0])) != cliqueList1.at(c1).idx[0])
//                    conflict = true;
//            }else if(!conflict && tmpmatch1.contains(cliqueList1.at(c1).idx[0])){
//                conflict = true;
//            }
//            if(!conflict && tmpmatch0.contains(cliqueList0.at(c0).idx[1])){
//                if(tmpmatch1.at(tmpmatch0.indexOf(cliqueList0.at(c0).idx[1])) != cliqueList1.at(c1).idx[1])
//                    conflict = true;
//            }else if(!conflict && tmpmatch1.contains(cliqueList1.at(c1).idx[1])){
//                conflict = true;
//            }
//            if(!conflict && tmpmatch0.contains(cliqueList0.at(c0).idx[2])){
//                if(tmpmatch1.at(tmpmatch0.indexOf(cliqueList0.at(c0).idx[2])) != cliqueList1.at(c1).idx[2])
//                    conflict = true;
//            }else if(!conflict && tmpmatch1.contains(cliqueList1.at(c1).idx[2])){
//                conflict = true;
//            }
//            if(!conflict){
//                if(!tmpmatch0.contains(cliqueList0.at(c0).idx[0])){
//                    tmpmatch0.append(cliqueList0.at(c0).idx[0]);
//                    tmpmatch1.append(cliqueList1.at(c1).idx[0]);
//                }
//                if(!tmpmatch0.contains(cliqueList0.at(c0).idx[1])){
//                    tmpmatch0.append(cliqueList0.at(c0).idx[1]);
//                    tmpmatch1.append(cliqueList1.at(c1).idx[1]);
//                }
//                if(!tmpmatch0.contains(cliqueList0.at(c0).idx[2])){
//                    tmpmatch0.append(cliqueList0.at(c0).idx[2]);
//                    tmpmatch1.append(cliqueList1.at(c1).idx[2]);
//                }
//            }
//            if(conflict && reachConflict == 1){
//                iter_conflict = iter_cpair;
//                reachConflict = 2; //reached and new one has been identified
//            }
//        }
        cout<<"matched points: "<<trial<<":";

        //perform affine transform
        while(1){
            cout<<":"<<tmpmatch0.size();

            double tmp_shift_x=0,tmp_shift_y=0,tmp_shift_z=0,tmp_angle=0,tmp_cent_x=0,tmp_cent_y=0,tmp_cent_z=0;
            if(direction==0) tmp_shift_x=-1;
            if(direction==1) tmp_shift_y=-1;
            if(direction==2) tmp_shift_z=-1;
            QList<XYZ> c0,c1,tmpcoord, tmpdir;
            for(int i=0; i<tmpmatch0.size(); i++){
                c0.append(XYZ(candcoord0.at(tmpmatch0.at(i))));
                c1.append(XYZ(candcoord1.at(tmpmatch1.at(i))));
            }
            if(!compute_affine_4dof(c0,c1,tmp_shift_x,tmp_shift_y,tmp_shift_z,tmp_angle,tmp_cent_x,tmp_cent_y,tmp_cent_z,direction)){
                break;
            }
            affine_XYZList(candcoord1, tmpcoord, tmp_shift_x, tmp_shift_y, tmp_shift_z, tmp_angle, tmp_cent_x, tmp_cent_y, tmp_cent_z, direction);
            affine_XYZList(canddircoord1, tmpdir, tmp_shift_x, tmp_shift_y, tmp_shift_z, tmp_angle, tmp_cent_x, tmp_cent_y, tmp_cent_z, direction);
            minus_XYZList(tmpdir,tmpcoord,tmpdir);
            QList<int> tmpMatchMarkers[2];
            tmpMatchMarkers[0]=QList<int>();
            tmpMatchMarkers[1]=QList<int>();
            getMatchPairs_XYZList(candcoord0, tmpcoord, canddir0, tmpdir, candcomponents0, candcomponents1, tmpMatchMarkers, pmatchThr, angThr_match);
            if(tmpMatchMarkers[0].size()>tmpmatch0.size()){
                tmpmatch0=tmpMatchMarkers[0];
                tmpmatch1=tmpMatchMarkers[1];
                better_shift_x=tmp_shift_x;
                better_shift_y=tmp_shift_y;
                better_shift_z=tmp_shift_z;
                better_angle=tmp_angle;
                better_cent_x=tmp_cent_x;
                better_cent_y=tmp_cent_y;
                better_cent_z=tmp_cent_z;
            }else{
                break;
            }
        }
        cout<<endl;

        if(tmpmatch0.size()>bestEnergy){
            bestEnergy = tmpmatch0.size();
            candmatch0 = tmpmatch0;
            candmatch1 = tmpmatch1;
            shift_x = better_shift_x;
            shift_y = better_shift_y;
            shift_z = better_shift_z;
            rotation_ang = better_angle;
            rotation_cx = better_cent_x;
            rotation_cy = better_cent_y;
            rotation_cz = better_cent_z;
            trial=0; //try another 10 times
        }
    }

    pmatch0.clear();
    pmatch1.clear();
    for(int i=0; i<candmatch0.size(); i++){
        pmatch0.append(candID0.at(candmatch0.at(i)));
        pmatch1.append(candID1.at(candmatch1.at(i)));
    }
    qDebug()<<"global match: "<<pmatch0.size()<<" matched points found";

    //affine based on matched points
    affine_nt1();
}

void neuron_match_clique::init()
{
    qDebug()<<"rescale in stacking direction";
    //rescale neurons for matching
    if(zscale!=1){
        if(direction == 0){
            proc_neuron_multiply_factor(nt0, zscale, 1, 1);
            proc_neuron_multiply_factor(nt1, zscale, 1, 1);
        }else if(direction == 1){
            proc_neuron_multiply_factor(nt0, 1, zscale, 1);
            proc_neuron_multiply_factor(nt1, 1, zscale, 1);
        }else{
            proc_neuron_multiply_factor(nt0, 1, 1, zscale);
            proc_neuron_multiply_factor(nt1, 1, 1, zscale);
        }
    }

    qDebug()<<"align in stacking direction";
    if(direction==0)
        shift_x=quickMoveNeuron(nt0, nt1, direction);
    else if(direction==1)
        shift_y=quickMoveNeuron(nt0, nt1, direction);
    else
        shift_z=quickMoveNeuron(nt0, nt1, direction);

    midplane=getNeuronTreeMidplane((*nt0), (*nt1), direction);    

    //find candidates
    qDebug()<<"search candidates";
    initNeuronAndCandidate(*nt0,ng0,neuronType0,candID0,candcoord0,canddir0,canddircoord0,components0,candcomponents0,parent0,1);
    initNeuronAndCandidate(*nt1,ng1,neuronType1,candID1,candcoord1,canddir1,canddircoord1,components1,candcomponents1,parent1,-1);
    //adjust component id of nt1
    int ccmax=0;
    for(int i=0; i<components0.size(); i++){
        ccmax=ccmax>components0.at(i)?ccmax:components0.at(i);
    }
    ccmax+=10;
    for(int i=0; i<components1.size(); i++){
        components1[i]+=ccmax;
    }
    for(int i=0; i<candcomponents1.size(); i++){
        candcomponents1[i]+=ccmax;
    }
    qDebug()<<"init neuron 0: cand:"<<candID0.size();
    qDebug()<<"init neuron 1: cand:"<<candID1.size();

    //find cliques
    qDebug()<<"start find cliques";
    getCliques(*nt0,candID0, candcoord0, canddir0, cliqueList0, cmatchThr, direction);
    getCliques(*nt1,candID1, candcoord1, canddir1, cliqueList1, cmatchThr, direction);
    qDebug()<<"init neuron 0: 3clique:"<<cliqueList0.size();
    qDebug()<<"init neuron 1: 3clique:"<<cliqueList1.size();

    qDebug()<<"start match cliques";

    //find the matched cliques
    matchCliquesAndCands();
}

void neuron_match_clique::output_parameter(QString fname)
{

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"stack_direction(0:x,1:y,2:z)= "<<direction<<endl;  //stack direction, 0x, 1y, 2z
    myfile<<"candidate_search_plane= "<<midplane<<endl;    //coordinate of the plane in between
    myfile<<"candidate_search_threshold_distance_to_plane= "<<spanCand<<endl;  //searching span from the stack plane for the candidate
    myfile<<"candidate_search_threshold_angle_to_plane(cos(a))= "<<angThr_stack<<endl; //angular threshold for candidate in stack direction (-1 ~ 1), -1 means nothing will be thresholded
    myfile<<"candidate_search_fragments_threshold= "<<segmentThr<<endl;  //threshold to filter out small segments when selecting candidates
    myfile<<"point_match_threshold_distance= "<<pmatchThr<<endl;   //match threshold for points
    myfile<<"point_match_threshold_angular(cos(a))= "<<angThr_match<<endl; //angular threshold for clique match (-1 ~ 1)=cos(theta), when angle is larger than theta (<cos(theta)), the point will not be matched
    myfile<<"clique_match_threshold_distance= "<<cmatchThr<<endl;   //match threshold for length of cliques
    myfile<<"stack_direction_rescale= "<<zscale<<endl;  //resacle stack direction
    myfile<<"segment_length_for_calculate_direction= "<<dir_range<<endl;   //length of section used to calculate direction of dead end
    myfile<<"spine_length_point_number= "<<spineLengthThr<<endl; //number of maximum points for spine
    myfile<<"spine_angle_threshold= "<<spineAngThr<<endl; //angular threshold for spine turning


    file.close();
}

//update matched point from markers, not necessary need. to do later
void neuron_match_clique::update_matchedPoints_from_Markers(LandmarkList * mList)
{

}

void neuron_match_clique::update_matchedPoints_to_Markers(LandmarkList * mList)
{
    mList->clear();
    int idx=0;
    //matched candidates first
    for(int i=0; i<pmatch0.size(); i++){
        int a=pmatch0.at(i);
        int aa=candmatch0.at(i);
        LocationSimple S0 = LocationSimple(nt0_stitch->listNeuron[a].x,
                                          nt0_stitch->listNeuron[a].y,
                                          nt0_stitch->listNeuron[a].z);
        S0.color.r = 255; S0.color.g = 0; S0.color.b = 0;
        S0.name = QString::number(idx++).toStdString();
        QString c0 = "0 "+QString::number(candID0.at(aa))+" "+QString::number(idx);
        S0.comments = c0.toStdString();
        mList->append(S0);

        int b=pmatch1.at(i);
        int bb=candmatch1.at(i);
        LocationSimple S1 = LocationSimple(nt1_stitch->listNeuron[b].x,
                                          nt1_stitch->listNeuron[b].y,
                                          nt1_stitch->listNeuron[b].z);
        S1.color.r = 0; S1.color.g = 255; S1.color.b = 0;
        S1.name = QString::number(idx++).toStdString();
        QString c1 = "1 "+QString::number(candID1.at(bb))+" "+QString::number(idx-2);
        S1.comments = c1.toStdString();
        mList->append(S1);
    }
    //then unmatched
    for(int i=0; i<candID0.size(); i++){
        if(candmatch0.contains(i)) continue;
        LocationSimple S = LocationSimple(nt0_stitch->listNeuron[candID0.at(i)].x,
                nt0_stitch->listNeuron[candID0.at(i)].y,
                nt0_stitch->listNeuron[candID0.at(i)].z);
        S.name = QString::number(idx++).toStdString();
        QString ctmp = "0 "+QString::number(candID0.at(i))+" -1 ";
        S.comments = ctmp.toStdString();
        S.color.r = 128; S.color.g = 0; S.color.b = 128;
        mList->append(S);
    }
    for(int i=0; i<candID1.size(); i++){
        if(candmatch1.contains(i)) continue;
        LocationSimple S = LocationSimple(nt1_stitch->listNeuron[candID1.at(i)].x,
                nt1_stitch->listNeuron[candID1.at(i)].y,
                nt1_stitch->listNeuron[candID1.at(i)].z);
        S.name = QString::number(idx++).toStdString();
        QString ctmp = "1 "+QString::number(candID1.at(i))+" -1";
        S.comments = ctmp.toStdString();
        S.color.r = 0; S.color.g = 128; S.color.b = 128;
        mList->append(S);
    }
}

void neuron_match_clique::output_candMatchScore(QString fname)
{
    QList<int> xs, ys;
    for(int i=0; i<candmatch0.size(); i++){
        xs.append(candmatch0.at(i));
        ys.append(candmatch1.at(i));
    }
    for(int i=0; i<candID0.size(); i++){
        if(!xs.contains(i))
            xs.append(i);
    }
    for(int i=0; i<candID1.size(); i++){
        if(!ys.contains(i))
            ys.append(i);
    }

    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"0\t";
    for(int j=0; j<candmatch0.size(); j++){
        myfile<<j*2+2<<"\t";
    }
    for(int j=candmatch0.size()+MS_x; j<MS_x+MS_y; j++){
        myfile<<j+1<<"\t";
    }
    myfile<<endl;
    for(int i=0; i<xs.size(); i++){
        if(i<candmatch0.size())
            myfile<<i*2+1<<"\t";
        else
            myfile<<i+candmatch0.size()+1<<"\t";
        for(int j=0; j<ys.size(); j++){
            myfile<<CANDMS_ENTRY(xs.at(i),ys.at(j))<<"\t";
        }
        myfile<<endl;
    }
    file.close();
}

void neuron_match_clique::output_matchedMarkers(QString fname, const NeuronTree& nt, QList<int> points)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    int idx=0;
    //matched candidates first
    for(int i=0; i<points.size(); i++){
        int a=points.at(i);
        myfile<<nt.listNeuron[a].x<<","<<nt.listNeuron[a].y<<","<<nt.listNeuron[a].z<<",0,1, "<<idx++<<", "
             <<", 255, 0, 0"<<endl;
    }
    file.close();
}

void neuron_match_clique::output_matchedMarkers_orgspace(QString fname_0, QString fname_1)
{
    QFile file(fname_0);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname_0 +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    int idx=0;
    //matched candidates first
    for(int i=0; i<pmatch0.size(); i++){
        int a=pmatch0.at(i);
        int aa=candmatch0.at(i);
        myfile<<nt0_org->listNeuron[a].x<<","<<nt0_org->listNeuron[a].y<<","<<nt0_org->listNeuron[a].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(aa)<<" -1 "
             <<canddir0[aa].x<<" "<<canddir0[aa].y<<" "<<canddir0[aa].z<<", 255, 0, 0"<<endl;
    }
    file.close();

    QFile file1(fname_1);
    if (!file1.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname_1 +" for write");
        return;
    }
    QTextStream myfile1(&file1);
    myfile1<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    idx=0;
    for(int i=0; i<pmatch0.size(); i++){
        int b=pmatch1.at(i);
        int bb=candmatch1.at(i);
        myfile1<<nt1_org->listNeuron[b].x<<","<<nt1_org->listNeuron[b].y<<","<<nt1_org->listNeuron[b].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(bb)<<" -1 "
             <<canddir1[bb].x<<" "<<canddir1[bb].y<<" "<<canddir1[bb].z<<", 0, 255, 0"<<endl;
    }
    file1.close();
}

void neuron_match_clique::output_markers_candspace(QString fname)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    int idx=0;
    //matched candidates first
    for(int i=0; i<candmatch0.size(); i++){
        int a=candmatch0.at(i);
        myfile<<candcoord0[a].x<<","<<candcoord0[a].y<<","<<candcoord0[a].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(a)<<" "<<idx<<" "
             <<canddir0[a].x<<" "<<canddir0[a].y<<" "<<canddir0[a].z<<", 255, 0, 0"<<endl;

        int b=candmatch1.at(i);
        myfile<<candcoord1[b].x<<","<<candcoord1[b].y<<","<<candcoord1[b].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(b)<<" "<<idx-1<<" "
             <<canddir1[b].x<<" "<<canddir1[b].y<<" "<<canddir1[b].z<<", 0, 255, 0"<<endl;
    }
    //then unmatched
    for(int i=0; i<candID0.size(); i++){
        if(candmatch0.contains(i)) continue;
        myfile<<candcoord0[i].x<<","<<candcoord0[i].y<<","<<candcoord0[i].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(i)<<" -1 "
             <<canddir0[i].x<<" "<<canddir0[i].y<<" "<<canddir0[i].z<<", 128, 0, 128"<<endl;
    }
    for(int i=0; i<candID1.size(); i++){
        if(candmatch1.contains(i)) continue;
        myfile<<candcoord1[i].x<<","<<candcoord1[i].y<<","<<candcoord1[i].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(i)<<" -1 "
             <<canddir1[i].x<<" "<<canddir1[i].y<<" "<<canddir1[i].z<<", 0, 128, 128"<<endl;
    }
    file.close();
}

void neuron_match_clique::output_markers_orgspace(QString fname)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    int idx=0;
    //matched candidates first
    for(int i=0; i<pmatch0.size(); i++){
        int a=pmatch0.at(i);
        int aa=candmatch0.at(i);
        myfile<<nt0_org->listNeuron[a].x<<","<<nt0_org->listNeuron[a].y<<","<<nt0_org->listNeuron[a].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(aa)<<" "<<idx<<" "
             <<canddir0[aa].x<<" "<<canddir0[aa].y<<" "<<canddir0[aa].z<<", 255, 0, 0"<<endl;

        int b=pmatch1.at(i);
        int bb=candmatch1.at(i);
        myfile<<nt1_org->listNeuron[b].x<<","<<nt1_org->listNeuron[b].y<<","<<nt1_org->listNeuron[b].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(bb)<<" "<<idx-1<<" "
             <<canddir1[bb].x<<" "<<canddir1[bb].y<<" "<<canddir1[bb].z<<", 0, 255, 0"<<endl;
    }
    //then unmatched
    for(int i=0; i<candID0.size(); i++){
        if(candmatch0.contains(i)) continue;
        myfile<<nt0_org->listNeuron[candID0.at(i)].x<<","<<nt0_org->listNeuron[candID0.at(i)].y<<","<<nt0_org->listNeuron[candID0.at(i)].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(i)<<" -1 "
             <<canddir0[i].x<<" "<<canddir0[i].y<<" "<<canddir0[i].z<<", 128, 0, 128"<<endl;
    }
    for(int i=0; i<candID1.size(); i++){
        if(candmatch1.contains(i)) continue;
        myfile<<nt1_org->listNeuron[candID1.at(i)].x<<","<<nt1_org->listNeuron[candID1.at(i)].y<<","<<nt1_org->listNeuron[candID1.at(i)].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(i)<<" -1 "
             <<canddir1[i].x<<" "<<canddir1[i].y<<" "<<canddir1[i].z<<", 0, 128, 128"<<endl;
    }
    file.close();
}

void neuron_match_clique::output_markers_affinespace(QString fname)
{
    QFile file(fname);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        v3d_msg("cannot open "+ fname +" for write");
        return;
    }
    QTextStream myfile(&file);
    myfile<<"##x,y,z,radius,shape,name,comment, color_r,color_g,color_b"<<endl;
    int idx=0;
    //matched candidates first
    for(int i=0; i<pmatch0.size(); i++){
        int a=pmatch0.at(i);
        int aa=candmatch0.at(i);
        myfile<<nt0_org->listNeuron[a].x<<","<<nt0_org->listNeuron[a].y<<","<<nt0_org->listNeuron[a].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(aa)<<" "<<idx<<" "
             <<canddir0[aa].x<<" "<<canddir0[aa].y<<" "<<canddir0[aa].z<<", 255, 0, 0"<<endl;

        int b=pmatch1.at(i);
        int bb=candmatch1.at(i);
        myfile<<nt1_a->listNeuron[b].x<<","<<nt1_a->listNeuron[b].y<<","<<nt1_a->listNeuron[b].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(bb)<<" "<<idx-1<<" "
             <<canddir1[bb].x<<" "<<canddir1[bb].y<<" "<<canddir1[bb].z<<", 0, 255, 0"<<endl;
    }
    //then unmatched
    for(int i=0; i<candID0.size(); i++){
        if(candmatch0.contains(i)) continue;
        myfile<<nt0_org->listNeuron[candID0.at(i)].x<<","<<nt0_org->listNeuron[candID0.at(i)].y<<","<<nt0_org->listNeuron[candID0.at(i)].z<<",0,1, "<<idx++<<","
             <<"0 "<<candID0.at(i)<<" -1 "
             <<canddir0[i].x<<" "<<canddir0[i].y<<" "<<canddir0[i].z<<", 128, 0, 128"<<endl;
    }
    for(int i=0; i<candID1.size(); i++){
        if(candmatch1.contains(i)) continue;
        myfile<<nt1_a->listNeuron[candID1.at(i)].x<<","<<nt1_a->listNeuron[candID1.at(i)].y<<","<<nt1_a->listNeuron[candID1.at(i)].z<<",0,1, "<<idx++<<","
             <<"1 "<<candID1.at(i)<<" -1 "
             <<canddir1[i].x<<" "<<canddir1[i].y<<" "<<canddir1[i].z<<", 0, 128, 128"<<endl;
    }
    file.close();
}

void neuron_match_clique::output_stitch(QString fname)
{
    if(candmatch0.size()<=0)
        return;
    if(nt0_stitch->listNeuron.size()<=0)
        return;
    if(nt1_stitch->listNeuron.size()<=0)
        return;

    QString fname_neuron0 = fname+"_stitched_nt0.swc";
    //output neuron
    if (!export_list2file(nt0_stitch->listNeuron,fname_neuron0,nt0_org->name))
    {
        v3d_msg("fail to write the output swc file:\n" + fname_neuron0);
        return;
    }

    QString fname_neuron1 = fname+"_stitched_nt1.swc";
    //output neuron
    if (!export_list2file(nt1_stitch->listNeuron,fname_neuron1,nt1_org->name))
    {
        v3d_msg("fail to write the output swc file:\n" + fname_neuron1);
        return;
    }

    //combine neurons and output
    QList<NeuronSWC> combined;
    int idmax=0;
    for(int i=0; i<nt0_stitch->listNeuron.size(); i++){
        NeuronSWC S;
        S.x = nt0_stitch->listNeuron.at(i).x;
        S.y = nt0_stitch->listNeuron.at(i).y;
        S.z = nt0_stitch->listNeuron.at(i).z;
        S.n = nt0_stitch->listNeuron.at(i).n;
        S.pn = nt0_stitch->listNeuron.at(i).pn;
        S.r = nt0_stitch->listNeuron.at(i).r;
        S.type = 2;
        combined.append(S);
        idmax=idmax>S.n?idmax:S.n;
        idmax=idmax>S.pn?idmax:S.pn;
    }
    for(int i=0; i<nt1_stitch->listNeuron.size(); i++){
        NeuronSWC S;
        S.x = nt1_stitch->listNeuron.at(i).x;
        S.y = nt1_stitch->listNeuron.at(i).y;
        S.z = nt1_stitch->listNeuron.at(i).z;
        S.n = nt1_stitch->listNeuron.at(i).n+idmax+1;
        S.r = nt1_stitch->listNeuron.at(i).r;
        S.type = 2;
        if(nt1_stitch->listNeuron.at(i).pn>=0)
            S.pn = nt1_stitch->listNeuron.at(i).pn+idmax+1;
        else
            S.pn=-1;
        combined.append(S);
    }
    QString fname_neuron = fname+"_stitched.swc";
    if (!export_list2file(combined,fname_neuron,nt0_org->name))
    {
        v3d_msg("fail to write the output swc file:\n" + fname_neuron);
        return;
    }

    //output markers
    QString fname_marker = fname+"_stitched.marker";
    output_matchedMarkers(fname_marker, *nt0_stitch, pmatch0);

    //output ano
    QString fname_ano = fname+"_stitched.ano";
    QFile qf_anofile(fname_ano);
    if(!qf_anofile.open(QIODevice::WriteOnly))
    {
        v3d_msg("Cannot open file for writing!");
        return;
    }
    QTextStream out(&qf_anofile);
    out<<"SWCFILE="<<fname_neuron0<<endl;
    out<<"SWCFILE="<<fname_neuron1<<endl;
    qf_anofile.close();
}

void neuron_match_clique::output_affine(QString fname, QString fname_nt0)
{
    if(candmatch0.size()<=0)
        return;

    QString fname_neuron = fname+"_affine.swc";
    //output neuron
    if (!export_list2file(nt1_a->listNeuron,fname_neuron,nt1_org->name))
    {
        v3d_msg("fail to write the output swc file:\n" + fname_neuron);
        return;
    }

    //output markers
    QString fname_marker = fname+"_affine.marker";
    output_markers_affinespace(fname_marker);

    //output amat
    double amat[16];
    getAffineAmat(amat, shift_x, shift_y, shift_z, rotation_ang, rotation_cx, rotation_cy, rotation_cz, direction);
    QString fname_amat = fname+"_amat.txt";
    writeAmat(fname_amat.toStdString().c_str(), amat);

    //output ano
    QString fname_ano = fname+"_affine.ano";
    QFile qf_anofile(fname_ano);
    if(!qf_anofile.open(QIODevice::WriteOnly))
    {
        v3d_msg("Cannot open file for writing!");
        return;
    }
    QTextStream out(&qf_anofile);
    out<<"SWCFILE="<<fname_nt0<<endl;
    out<<"SWCFILE="<<fname_neuron<<endl;
    qf_anofile.close();
}

void neuron_match_clique::affine_nt1()
{
    if(pmatch0.size()<0){
        qDebug()<<"not match point identified, quit align";
        return;
    }

    copyCoordinate(*nt1_org, *nt1_a);
    double shift_stack = quickMoveNeuron(nt0_org, nt1_a, direction);

    QList<XYZ> c0, c1;
    for(int i=0; i<pmatch0.size(); i++){
        c0.append(XYZ(nt0_org->listNeuron.at(pmatch0.at(i))));
        c1.append(XYZ(nt1_a->listNeuron.at(pmatch1.at(i))));
    }

    double tmp_shift_x=0,tmp_shift_y=0,tmp_shift_z=0,tmp_angle=0,tmp_cent_x=0,tmp_cent_y=0,tmp_cent_z=0;
    if(direction==0) tmp_shift_x=-1;
    if(direction==1) tmp_shift_y=-1;
    if(direction==2) tmp_shift_z=-1;
    if(!compute_affine_4dof(c0,c1,tmp_shift_x,tmp_shift_y,tmp_shift_z,tmp_angle,tmp_cent_x,tmp_cent_y,tmp_cent_z,direction)){
        return;
    }

    affineNeuron(*nt1_a,*nt1_a,tmp_shift_x,tmp_shift_y,tmp_shift_z,tmp_angle,tmp_cent_x,tmp_cent_y,tmp_cent_z,direction);
    copyCoordinate(*nt1_a,*nt1_stitch);
    if(direction==0) tmp_shift_x=shift_stack;
    if(direction==1) tmp_shift_y=shift_stack;
    if(direction==2) tmp_shift_z=shift_stack;
    shift_x = tmp_shift_x;
    shift_y = tmp_shift_y;
    shift_z = tmp_shift_z;
    rotation_ang = tmp_angle;
    rotation_cx = tmp_cent_x;
    rotation_cy = tmp_cent_y;
    rotation_cz = tmp_cent_z;
}

void neuron_match_clique::matchCliquesAndCands()
{
    rankedCliqueMatch.clear();
    candMS.clear();
    MS_x=candID0.size();
    MS_y=candID1.size();
    candMS.resize(MS_x*MS_y,0);
//    candMS=QVector(MS_x*MS_y,0);

    QVector<QVector<int> > cliquePairs;
    for(int i=0; i<cliqueList0.size(); i++){
        for(int j=0; j<cliqueList1.size(); j++){
            //find matching clique
            if(fabs(cliqueList0[i].e[0]-cliqueList1[j].e[0])>cmatchThr) continue;
            if(fabs(cliqueList0[i].e[1]-cliqueList1[j].e[1])>cmatchThr) continue;
            if(fabs(cliqueList0[i].e[2]-cliqueList1[j].e[2])>cmatchThr) continue;
            if(NTDOT(cliqueList0[i].dir[0],cliqueList1[j].dir[0])<angThr_match) continue;
            if(NTDOT(cliqueList0[i].dir[1],cliqueList1[j].dir[1])<angThr_match) continue;
            if(NTDOT(cliqueList0[i].dir[2],cliqueList1[j].dir[2])<angThr_match) continue;

            QVector<int> pair;
            pair<<i<<j;
            cliquePairs<<pair;
            for(int k=0; k<3; k++)
                CANDMS_ENTRY(cliqueList0[i].idx[k],cliqueList1[j].idx[k])++;
        }
    }

    qDebug()<<"found "<<cliquePairs.size()<<" pair of cliques";

    for(int i=0; i<cliquePairs.size(); i++){
        int matchscore=0;
        for(int k=0; k<3; k++){
            matchscore+=CANDMS_ENTRY(cliqueList0[cliquePairs[i].at(0)].idx[k],cliqueList1[cliquePairs[i].at(1)].idx[k]);
        }
        rankedCliqueMatch.insert(pair<int, QVector<int> >(matchscore,cliquePairs[i]) );
    }
}

void neuron_match_clique::stitch()
{
    for(int pid=0; pid<pmatch0.size(); pid++){
        if(components0[pmatch0.at(pid)]==components1[pmatch1.at(pid)]) //loop
            continue;

        stitchMatchedPoint(nt0_stitch, nt1_stitch, parent0, parent1, pmatch0.at(pid), pmatch1.at(pid));

        //update cand components
        int cid=components1[pmatch1.at(pid)];
        int nid=components0[pmatch0.at(pid)];
        int idx=components0.indexOf(cid);
        while(idx>=0){
            components0[idx]=nid;
            idx=components0.indexOf(cid,idx+1);
        }
        idx=candcomponents0.indexOf(cid);
        while(idx>=0){
            candcomponents0[idx]=nid;
            idx=candcomponents0.indexOf(cid, idx+1);
        }
        idx=components1.indexOf(cid);
        while(idx>=0){
            components1[idx]=nid;
            idx=components1.indexOf(cid,idx+1);
        }
        idx=candcomponents1.indexOf(cid);
        while(idx>=0){
            candcomponents1[idx]=nid;
            idx=candcomponents1.indexOf(cid, idx+1);
        }
    }
}

bool neuron_match_clique::stitch(int point0, int point1)
{
    if(components0.size()>0 && components1.size()>0)
        if(checkloop(point0, point1))
            return false;

    //stitch
    stitchMatchedPoint(nt0_stitch, nt1_stitch, parent0, parent1, point0, point1);

    //update cand components
    int cid=components1[point1];
    int nid=components0[point0];
    int idx=components0.indexOf(cid);
    while(idx>=0){
        components0[idx]=nid;
        idx=components0.indexOf(cid,idx+1);
    }
    idx=candcomponents0.indexOf(cid);
    while(idx>=0){
        candcomponents0[idx]=nid;
        idx=candcomponents0.indexOf(cid, idx+1);
    }
    idx=components1.indexOf(cid);
    while(idx>=0){
        components1[idx]=nid;
        idx=components1.indexOf(cid,idx+1);
    }
    idx=candcomponents1.indexOf(cid);
    while(idx>=0){
        candcomponents1[idx]=nid;
        idx=candcomponents1.indexOf(cid, idx+1);
    }

    return true;
}

bool neuron_match_clique::checkloop(int point0, int point1)
{
    if(components0.at(point0) == components1.at(point1)){
        return true;
    }
    return false;
}

void neuron_match_clique::highlight_nt1_seg(int point1, int type)
{
    int cid=components1.at(point1);
    int idx=components1.indexOf(cid);
    while(idx>=0){
        NeuronSWC* p = (NeuronSWC*)&(nt1_stitch->listNeuron.at(idx));
        p->type=type;
        idx=components1.indexOf(cid, idx+1);
    }
}

void neuron_match_clique::highlight_nt0_seg(int point0, int type)
{
    int cid=components0.at(point0);
    int idx=components0.indexOf(cid);
    while(idx>=0){
        NeuronSWC* p = (NeuronSWC*)&(nt0_stitch->listNeuron.at(idx));
        p->type=type;
        idx=components0.indexOf(cid, idx+1);
    }
}

////orientation should be 1/-1 for smaller/larger stack in direction
//void neuron_match_clique::initNeuronAndCandidate(NeuronTree& nt, QList<int>& cand, QList<XYZ>& candcoord,
//                                                 QList<XYZ>& canddir, QList<XYZ>& canddircoord, QList<int>& components,
//                                                 QList<int>& candcomponents, QList<int>& pList, int orientation)
//{
//    float min=midplane-spanCand;
//    float max=midplane+spanCand;

//    cand.clear();
//    candcoord.clear();
//    canddir.clear();
//    canddircoord.clear();
//    candcomponents.clear();

//    QVector<int> childNum(nt.listNeuron.size(), 0);
//    QVector<int> connNum(nt.listNeuron.size(), 0);
//    QVector<double> sectionLength(nt.listNeuron.size(), 0);
//    //QList<int> components;
//    components.clear();
//    pList.clear();
//    QVector<V3DLONG> componentSize;
//    QVector<V3DLONG> componentLength;
//    V3DLONG curid=0;
//    for(V3DLONG i=0; i<nt.listNeuron.size(); i++){
//        if(nt.listNeuron.at(i).pn<0){
//            connNum[i]--; //root that only have 1 clide will also be a dead end
//            components.append(curid); curid++;
//            pList.append(-1);
//        }
//        else{
//            int pid = nt.hashNeuron.value(nt.listNeuron.at(i).pn);
//            childNum[pid]++;
//            connNum[pid]++;
//            sectionLength[i]=sqrt(NTDIS(nt.listNeuron.at(i),nt.listNeuron.at(pid)));
//            components.append(-1);
//            pList.append(pid);
//        }
//    }
//    //connected component
//    for(V3DLONG cid=0; cid<curid; cid++){
//        QStack<int> pstack;
//        int chid, size = 0;
//        if(!components.contains(cid)) //should not happen, just in case
//            continue;
//        if(components.indexOf(cid)!=components.lastIndexOf(cid)) //should not happen
//            qDebug("unexpected multiple tree root, please check the code: neuron_stitch_func.cpp");
//        //recursively search for child and mark them as the same component
//        pstack.push(components.indexOf(cid));
//        size++;
//        while(!pstack.isEmpty()){
//            int pid=pstack.pop();
//            chid = -1;
//            chid = pList.indexOf(pid,chid+1);
//            while(chid>=0){
//                pstack.push(chid);
//                components[chid]=cid;
//                chid=pList.indexOf(pid,chid+1);
//                size++;
//            }
//        }
//        componentSize.append(size);
//    }
//    //component size
//    for(V3DLONG cid=0; cid<curid; cid++){
//        double length = 0;
//        int idx = -1;
//        for(V3DLONG i=0; i<componentSize[cid]; i++){
//            idx = components.indexOf(cid,idx+1);
//            length+=sectionLength[idx];
//        }
//        componentLength.append(length);
//    }


//    QList<int> tmpcand; tmpcand.clear();
//    for(V3DLONG i=0; i<childNum.size(); i++){
//        if(connNum[i]<=0 && componentLength[components[i]]>=segmentThr){
//            if(direction==0){//x
//                if(nt.listNeuron.at(i).x>min && nt.listNeuron.at(i).x<max){
//                    tmpcand.append(i);
//                }
//            }else if(direction==1){//y
//                if(nt.listNeuron.at(i).y>min && nt.listNeuron.at(i).y<max){
//                    tmpcand.append(i);
//                }
//            }else if(direction==2){//z
//                if(nt.listNeuron.at(i).z>min && nt.listNeuron.at(i).z<max){
//                    tmpcand.append(i);
//                }
//            }else{//all tips
//                tmpcand.append(i);
//            }
//        }
//    }

//    for(V3DLONG i=0; i<tmpcand.size(); i++){
//        XYZ tmpdir(0,0,0);
//        int id = tmpcand[i];
//        if(childNum[id]==1){ //single child root
//            float lentmp = 0;
//            while(lentmp<dir_range && id>=0){
//                int sid = pList.indexOf(id);
//                if(sid<0) break;
//                lentmp+=sectionLength[sid];
//                tmpdir.x += nt.listNeuron.at(id).x-nt.listNeuron.at(sid).x;
//                tmpdir.y += nt.listNeuron.at(id).y-nt.listNeuron.at(sid).y;
//                tmpdir.z += nt.listNeuron.at(id).z-nt.listNeuron.at(sid).z;
//                id=sid;
//            }
//        }else{ //tips
//            float lentmp = 0;
//            while(lentmp<dir_range && id>=0){
//                int sid = pList[id];
//                if(sid<0) break;
//                lentmp+=sectionLength[id];
//                tmpdir.x += nt.listNeuron.at(id).x-nt.listNeuron.at(sid).x;
//                tmpdir.y += nt.listNeuron.at(id).y-nt.listNeuron.at(sid).y;
//                tmpdir.z += nt.listNeuron.at(id).z-nt.listNeuron.at(sid).z;
//                id=sid;
//            }
//        }
//        //normalize direction
//        double tmpNorm = sqrt(tmpdir.x*tmpdir.x+tmpdir.y*tmpdir.y+tmpdir.z*tmpdir.z);
//        tmpdir.x/=tmpNorm; tmpdir.x*=orientation;
//        tmpdir.y/=tmpNorm; tmpdir.y*=orientation;
//        tmpdir.z/=tmpNorm; tmpdir.z*=orientation;

//        //judgement to the direction to avoid the connection that moves from plan to
//        double sa = 1;
//        if(direction==0){
//            sa = tmpdir.x;
//        }else if(direction == 1){
//            sa = tmpdir.y;
//        }else if(direction == 2){
//            sa = tmpdir.z;
//        }
//        //for test:
//        //qDebug()<<i<<":"<<tmpcand[i]<<"; coord"<<nt.listNeuron.at(tmpcand[i]).x<<":"<<nt.listNeuron.at(tmpcand[i]).y<<":"<<nt.listNeuron.at(tmpcand[i]).z<<"; dir"<<tmpdir.x<<":"<<tmpdir.y<<":"<<tmpdir.z<<":"<<tmpNorm;

//        if(sa<angThr_stack) continue;

//        cand.append(tmpcand[i]);
//        candcoord.append(XYZ(nt.listNeuron.at(tmpcand[i])));
//        canddir.append(tmpdir);
//        canddircoord.append(XYZ(candcoord.last().x+canddir.last().x,candcoord.last().y+canddir.last().y,candcoord.last().z+canddir.last().z));
//        candcomponents.append(components.at(tmpcand[i]));
//    }
//}



//orientation should be 1/-1 for smaller/larger stack in directio
//new type: 1:root; 2:path; 5:fork point; 6:end point; 7:single child root;
void neuron_match_clique::initNeuronAndCandidate(NeuronTree& nt, const HBNeuronGraph& ng, QList<int>& neuronType, QList<int>& cand, QList<XYZ>& candcoord,
                                                 QList<XYZ>& canddir, QList<XYZ>& canddircoord, QList<int>& components,
                                                 QList<int>& candcomponents, QList<int>& pList, int orientation)
{
    float min=midplane-spanCand;
    float max=midplane+spanCand;

    cand.clear();
    candcoord.clear();
    canddir.clear();
    canddircoord.clear();
    candcomponents.clear();

    QVector<int> childNum(nt.listNeuron.size(), 0);
    QVector<int> connNum(nt.listNeuron.size(), 0);
    QVector<double> sectionLength(nt.listNeuron.size(), 0);
    //QList<int> components;
    components.clear();
    pList.clear();
    QVector<V3DLONG> componentSize;
    QVector<V3DLONG> componentLength;
    V3DLONG curid=0;
    for(V3DLONG i=0; i<nt.listNeuron.size(); i++){
        if(nt.listNeuron.at(i).pn<0){
            connNum[i]--; //root that only have 1 clide will also be a dead end
            components.append(curid); curid++;
            pList.append(-1);
        }
        else{
            int pid = nt.hashNeuron.value(nt.listNeuron.at(i).pn);
            childNum[pid]++;
            connNum[pid]++;
            sectionLength[i]=sqrt(NTDIS(nt.listNeuron.at(i),nt.listNeuron.at(pid)));
            components.append(-1);
            pList.append(pid);
        }
    }

    //for test
    qDebug()<<"init Nueron type";

    //neuron type
    initNeuronType(nt,ng,neuronType);

//    //for test
//    qDebug()<<"connected component";

    //connected component
    for(V3DLONG cid=0; cid<curid; cid++){
        QStack<int> pstack;
        int chid, size = 0;
        if(!components.contains(cid)) //should not happen, just in case
            continue;
        if(components.indexOf(cid)!=components.lastIndexOf(cid)) //should not happen
            qDebug("unexpected multiple tree root, please check the code: neuron_stitch_func.cpp");
        //recursively search for child and mark them as the same component
        pstack.push(components.indexOf(cid));
        size++;
        while(!pstack.isEmpty()){
            int pid=pstack.pop();
            chid = -1;
            chid = pList.indexOf(pid,chid+1);
            while(chid>=0){
                pstack.push(chid);
                components[chid]=cid;
                chid=pList.indexOf(pid,chid+1);
                size++;
            }
        }
        componentSize.append(size);
    }
    //component size
    for(V3DLONG cid=0; cid<curid; cid++){
        double length = 0;
        int idx = -1;
        for(V3DLONG i=0; i<componentSize[cid]; i++){
            idx = components.indexOf(cid,idx+1);
            length+=sectionLength[idx];
        }
        componentLength.append(length);
    }


//    //for test
//    qDebug()<<"find candidate";

    //find candidate
    QList<int> tmpcand; tmpcand.clear();
    for(V3DLONG i=0; i<neuronType.size(); i++){
        if((neuronType.at(i)%10)==6 && componentLength[components[i]]>=segmentThr){
            if(direction==0){//x
                if(nt.listNeuron.at(i).x>min && nt.listNeuron.at(i).x<max){
                    tmpcand.append(i);
                }
            }else if(direction==1){//y
                if(nt.listNeuron.at(i).y>min && nt.listNeuron.at(i).y<max){
                    tmpcand.append(i);
                }
            }else if(direction==2){//z
                if(nt.listNeuron.at(i).z>min && nt.listNeuron.at(i).z<max){
                    tmpcand.append(i);
                }
            }else{//all tips
                tmpcand.append(i);
            }
        }
    }

//    //for test
//    qDebug()<<"finalize candidate";

    for(V3DLONG i=0; i<tmpcand.size(); i++){
        XYZ tmpdir(0,0,0);
        V3DLONG curid = tmpcand[i];
        V3DLONG nexid, preid=curid;
        if(neuronType.at(tmpcand.at(i))==6){ //normal tips or spine tips
            float lentmp = 0;
            while(lentmp<dir_range && curid>=0){
                lentmp+=sectionLength[curid];
                nexid=nextPointNeuronGraph(ng,curid,preid);
                preid=curid;
                curid=nexid;
            }
            tmpdir.x = nt.listNeuron.at(tmpcand[i]).x - nt.listNeuron.at(preid).x;
            tmpdir.y = nt.listNeuron.at(tmpcand[i]).y - nt.listNeuron.at(preid).y;
            tmpdir.z = nt.listNeuron.at(tmpcand[i]).z - nt.listNeuron.at(preid).z;
        }else if(neuronType.at(tmpcand.at(i))==56){ //spine fork
            float lentmp = 0;
            while(lentmp<dir_range && curid>=0){
                lentmp+=sectionLength[curid];
                nexid=-1;
                for(int j=0; j<ng.at(curid).size(); j++){
                    if(ng.at(curid).at(j)==preid) continue;
                    if(neuronType.at(ng.at(curid).at(j))==2 || neuronType.at(ng.at(curid).at(j))== 52){
                        if(nexid==-1){
                            nexid=ng.at(curid).at(j);
                        }else{
                            nexid=-2;
                            break;
                        }
                    }
                }
                preid=curid;
                curid=nexid;
            }
            tmpdir.x = nt.listNeuron.at(tmpcand[i]).x - nt.listNeuron.at(preid).x;
            tmpdir.y = nt.listNeuron.at(tmpcand[i]).y - nt.listNeuron.at(preid).y;
            tmpdir.z = nt.listNeuron.at(tmpcand[i]).z - nt.listNeuron.at(preid).z;
        }
        //normalize direction
        double tmpNorm = sqrt(tmpdir.x*tmpdir.x+tmpdir.y*tmpdir.y+tmpdir.z*tmpdir.z);
        tmpdir.x/=tmpNorm; tmpdir.x*=orientation;
        tmpdir.y/=tmpNorm; tmpdir.y*=orientation;
        tmpdir.z/=tmpNorm; tmpdir.z*=orientation;

        //judgement to the direction to avoid the connection that moves from plan to
        double sa = 1;
        if(direction==0){
            sa = tmpdir.x;
        }else if(direction == 1){
            sa = tmpdir.y;
        }else if(direction == 2){
            sa = tmpdir.z;
        }
        //for test:
        //qDebug()<<i<<":"<<tmpcand[i]<<"; coord"<<nt.listNeuron.at(tmpcand[i]).x<<":"<<nt.listNeuron.at(tmpcand[i]).y<<":"<<nt.listNeuron.at(tmpcand[i]).z<<"; dir"<<tmpdir.x<<":"<<tmpdir.y<<":"<<tmpdir.z<<":"<<tmpNorm;

        if(sa<angThr_stack) continue;

        cand.append(tmpcand[i]);
        candcoord.append(XYZ(nt.listNeuron.at(tmpcand[i])));
        canddir.append(tmpdir);
        canddircoord.append(XYZ(candcoord.last().x+canddir.last().x,candcoord.last().y+canddir.last().y,candcoord.last().z+canddir.last().z));
        candcomponents.append(components.at(tmpcand[i]));
    }
}

//initial type should have already been assigned
//initNeuronAndCandidate required befor running this
//normal type: 1:single root; 2:path; 5:fork point; 6:end point;
//spine related: 21:path; 61:end point;
//spine related fork: 51:have 0 none spine path (single root); 56:have only one none spine path (end); 52:have two none spine path (path); 55:have more than two none spine path (fork);
//all types ended with '6' can be used for matching across sections
int neuron_match_clique::initNeuronType(const NeuronTree& nt, const HBNeuronGraph& ng, QList<int>& neuronType)
{
    neuronType.clear();
    int spinecount=0;
    int length=spineLengthThr;
    //basic types
    for(V3DLONG idx=0; idx<nt.listNeuron.size(); idx++){
        if(ng.at(idx).size()<1){
            neuronType.append(1); //single root
        }else if(ng.at(idx).size()==1){
            neuronType.append(6); //end point
        }else if(ng.at(idx).size()==2){
            neuronType.append(2); //path
        }else{
            neuronType.append(5); //fork point
        }
    }

    //spine
    if(length<=1e-10) return 0;

    QMap<V3DLONG, QVector<V3DLONG> > fork_spines_map;
    for(V3DLONG idx=0; idx<nt.listNeuron.size(); idx++){
        if(ng.at(idx).size()==1){ //end point
            int count = 0;
            V3DLONG preid=idx, nexid=idx, curid=idx;
            do{
                count++;
                nexid=nextPointNeuronGraph(ng,curid,preid);
                preid=curid;
                curid=nexid;
            }while(nexid>=0 && count<=length);

            if(count<length){ //is spine
                neuronType[idx]=61; //spine tip
                curid=nextPointNeuronGraph(ng,idx);
                preid=idx;
                while(curid>=0 && ng.at(curid).size()==2){
                    neuronType[curid]=21; //spine path
                    nexid=nextPointNeuronGraph(ng,curid,preid);
                    preid=curid;
                    curid=nexid;
                }
                if(curid>=0){
                    if(fork_spines_map.contains(curid)){
                        fork_spines_map[curid].append(idx);
                    }else{
                        QVector<V3DLONG> tmp;
                        tmp.append(idx);
                        fork_spines_map[curid]=tmp;
                    }
                }
                spinecount++;
            }
        }
    }

    //find the spine that is matchable
    for(QMap<V3DLONG, QVector<V3DLONG> >::Iterator iter = fork_spines_map.begin();
        iter != fork_spines_map.end(); iter++){
        //check the branch type (a.have only one none spine connection (end); b. have two (path); c. have more than two (fork))
        int count=0;
        V3DLONG idx = iter.key();

        //for test
        qDebug()<<"init neuron type: matchable "<<idx;

        for(int i=0; i<ng.at(idx).size(); i++){
            if(neuronType.at(ng.at(idx).at(i))!=21 && neuronType.at(ng.at(idx).at(i))!=61){
                count++;
            }
        }
        switch(count){
        case 0:
            neuronType[idx]=51; //single dendritic root
            break;
        case 1:
            neuronType[idx]=56; //dendritic end
            break;
        case 2:
            neuronType[idx]=52; //dendritic path
            break;
        default:
            neuronType[idx]=55; //dendritic fork
        }


        if(neuronType[idx]==56){
            XYZ rootDir;
            bool spineMatchable=false;
            for(int i=0; i<ng.at(idx).size(); i++){
                if(neuronType.at(ng.at(idx).at(i))!=21 && neuronType.at(ng.at(idx).at(i))!=61){
                    rootDir.x = nt.listNeuron.at(idx).x-nt.listNeuron.at(ng.at(idx).at(i)).x;
                    rootDir.y = nt.listNeuron.at(idx).y-nt.listNeuron.at(ng.at(idx).at(i)).y;
                    rootDir.z = nt.listNeuron.at(idx).z-nt.listNeuron.at(ng.at(idx).at(i)).z;
                }
            }

            for(int i=0; i<iter.value().size(); i++){
                V3DLONG curid = iter.value().at(i);
                //check the angle condition
                XYZ spineDir;
                spineDir.x = nt.listNeuron.at(curid).x-nt.listNeuron.at(idx).x;
                spineDir.y = nt.listNeuron.at(curid).y-nt.listNeuron.at(idx).y;
                spineDir.z = nt.listNeuron.at(curid).z-nt.listNeuron.at(idx).z;

                if(NTDOT(spineDir,rootDir)/(NTNORM(spineDir)*NTNORM(rootDir))<spineAngThr){
                    continue;
                }
                //check radius condition if meet angle condition
                V3DLONG preid=curid, nexid=curid;
                while(nt.listNeuron.at(curid).radius<spineRadiusThr && curid!=idx){
                    nexid=nextPointNeuronGraph(ng,curid,preid);
                    preid=curid;
                    curid=nexid;
                }
                if(curid!=idx){ //this could be a normal branch, not spine
                    curid = iter.value().at(i);
                    neuronType[curid]=6;
                    nexid=nextPointNeuronGraph(ng,curid);
                    preid=curid;
                    curid=nexid;
                    while(curid!=idx){
                        neuronType[curid]=2;
                        nexid=nextPointNeuronGraph(ng,curid,preid);
                        preid=curid;
                        curid=nexid;
                    }
                    spineMatchable=true;
                }
            }

            idx = iter.key();
            if(neuronType[idx]==56){
                if(spineMatchable || nt.listNeuron.at(idx).radius<spineRadiusThr){
                    neuronType[idx]=52; //radius too small or child can match, downgrade it to path of spine
                }
            }
        }else if(neuronType[idx]==55 || neuronType[idx]==51){
            //check radius condition
            for(int i=0; i<iter.value().size(); i++){
                V3DLONG curid = iter.value().at(i);
                V3DLONG preid=curid, nexid=curid;
                while(nt.listNeuron.at(curid).radius<spineRadiusThr && curid!=idx){
                    nexid=nextPointNeuronGraph(ng,curid,preid);
                    preid=curid;
                    curid=nexid;
                }
                if(curid!=idx){ //this could be a normal branch, not spine
                    curid = iter.value().at(i);
                    neuronType[curid]=6;
                    nexid=nextPointNeuronGraph(ng,curid);
                    preid=curid;
                    curid=nexid;
                    while(curid!=idx){
                        neuronType[curid]=2;
                        nexid=nextPointNeuronGraph(ng,curid,preid);
                        preid=curid;
                        curid=nexid;
                    }
                }
            }
        }
    }
    return spinecount;
}

void neuron_match_clique::initNeuronComponents()
{
    initNeuronComponents(*nt0, components0, parent0);
    initNeuronComponents(*nt1, components1, parent1);
    //adjust component id of nt1
    int ccmax=0;
    for(int i=0; i<components0.size(); i++){
        ccmax=ccmax>components0.at(i)?ccmax:components0.at(i);
    }
    ccmax+=10;
    for(int i=0; i<components1.size(); i++){
        components1[i]+=ccmax;
    }
}

void neuron_match_clique::initNeuronComponents(NeuronTree& nt, QList<int>& components, QList<int>& pList)
{
    QVector<int> childNum(nt.listNeuron.size(), 0);
    QVector<int> connNum(nt.listNeuron.size(), 0);
    //QList<int> components;
    components.clear();
    pList.clear();
    V3DLONG curid=0;
    for(V3DLONG i=0; i<nt.listNeuron.size(); i++){
        if(nt.listNeuron.at(i).pn<0){
            connNum[i]--; //root that only have 1 clide will also be a dead end
            components.append(curid); curid++;
            pList.append(-1);
        }
        else{
            int pid = nt.hashNeuron.value(nt.listNeuron.at(i).pn);
            childNum[pid]++;
            connNum[pid]++;
            components.append(-1);
            pList.append(pid);
        }
    }
    //connected component
    for(V3DLONG cid=0; cid<curid; cid++){
        QStack<int> pstack;
        int chid, size = 0;
        if(!components.contains(cid)) //should not happen, just in case
            continue;
        if(components.indexOf(cid)!=components.lastIndexOf(cid)) //should not happen
            qDebug("unexpected multiple tree root, please check the code: neuron_stitch_func.cpp");
        //recursively search for child and mark them as the same component
        pstack.push(components.indexOf(cid));
        size++;
        while(!pstack.isEmpty()){
            int pid=pstack.pop();
            chid = -1;
            chid = pList.indexOf(pid,chid+1);
            while(chid>=0){
                pstack.push(chid);
                components[chid]=cid;
                chid=pList.indexOf(pid,chid+1);
                size++;
            }
        }
    }
}
