//------------------------------------------------------------------------------------------------
// Copyright (c) 2012  Alessandro Bria and Giulio Iannello (University Campus Bio-Medico of Rome).  
// All rights reserved.
//------------------------------------------------------------------------------------------------

/*******************************************************************************************************************************************************************************************
*    LICENSE NOTICE
********************************************************************************************************************************************************************************************
*    By downloading/using/running/editing/changing any portion of codes in this package you agree to this license. If you do not agree to this license, do not download/use/run/edit/change
*    this code.
********************************************************************************************************************************************************************************************
*    1. This material is free for non-profit research, but needs a special license for any commercial purpose. Please contact Alessandro Bria at a.bria@unicas.it or Giulio Iannello at 
*       g.iannello@unicampus.it for further details.
*    2. You agree to appropriately cite this work in your related studies and publications.
*
*       Bria, A., et al., (2012) "Stitching Terabyte-sized 3D Images Acquired in Confocal Ultramicroscopy", Proceedings of the 9th IEEE International Symposium on Biomedical Imaging.
*       Bria, A., Iannello, G., "A Tool for Fast 3D Automatic Stitching of Teravoxel-sized Datasets", submitted on July 2012 to IEEE Transactions on Information Technology in Biomedicine.
*
*    3. This material is provided by  the copyright holders (Alessandro Bria  and  Giulio Iannello),  University Campus Bio-Medico and contributors "as is" and any express or implied war-
*       ranties, including, but  not limited to,  any implied warranties  of merchantability,  non-infringement, or fitness for a particular purpose are  disclaimed. In no event shall the
*       copyright owners, University Campus Bio-Medico, or contributors be liable for any direct, indirect, incidental, special, exemplary, or  consequential  damages  (including, but not 
*       limited to, procurement of substitute goods or services; loss of use, data, or profits;reasonable royalties; or business interruption) however caused  and on any theory of liabil-
*       ity, whether in contract, strict liability, or tort  (including negligence or otherwise) arising in any way out of the use of this software,  even if advised of the possibility of
*       such damage.
*    4. Neither the name of University  Campus Bio-Medico of Rome, nor Alessandro Bria and Giulio Iannello, may be used to endorse or  promote products  derived from this software without
*       specific prior written permission.
********************************************************************************************************************************************************************************************/

#include "PDialogImport.h"
#include "MyException.h"
#include "StackedVolume.h"
#include "PMain.h"
#include "control/CImport.h"

using namespace teramanager;

/*********************************************************************************
* Singleton design pattern: this class can have one instance only,  which must be
* instantiated by calling static method "istance(...)"
**********************************************************************************/
PDialogImport* PDialogImport::uniqueInstance = NULL;
void PDialogImport::uninstance()
{
    if(uniqueInstance)
    {
        delete uniqueInstance;
        uniqueInstance = NULL;
    }
}

PDialogImport::PDialogImport() : QDialog()
{
    #ifdef TSP_DEBUG
    printf("teramanager plugin [thread %d] >> PDialogImport created\n", this->thread()->currentThreadId());
    #endif

    //import form widgets
    import_form_desc_1 = new QLabel("Additional informations are required when the volume is imported for the first time or reimported.\n\nPlease fill all the fields and pay attention that the minus \"-\" sign before an axis is interpreted as a rotation by 180 degrees.");
    import_form_desc_1->setWordWrap(true);
    first_direction_label = new QLabel("First direction");
    second_direction_label = new QLabel("Second direction");
    third_direction_label = new QLabel("Third direction");
    axes_label = new QLabel("Axes (1 = Vertical, 2 = Horizontal, 3 = Depth)");
    voxels_dims_label = new QLabel("Voxel's dimensions (micrometers)");
    QRegExp axs_regexp("^-?[123]$");
    QRegExp vxl_regexp("^[0-9]+\\.?[0-9]*$");
    axs1_field = new QLineEdit();
    axs1_field->setAlignment(Qt::AlignCenter);
    axs1_field->setValidator(new QRegExpValidator(axs_regexp, axs1_field));
    axs2_field = new QLineEdit();
    axs2_field->setAlignment(Qt::AlignCenter);
    axs2_field->setValidator(new QRegExpValidator(axs_regexp, axs2_field));
    axs3_field = new QLineEdit();
    axs3_field->setAlignment(Qt::AlignCenter);
    axs3_field->setValidator(new QRegExpValidator(axs_regexp, axs3_field));
    vxl1_field = new QLineEdit();
    vxl1_field->setAlignment(Qt::AlignCenter);
    vxl1_field->setValidator(new QRegExpValidator(vxl_regexp, vxl1_field));
    vxl2_field = new QLineEdit();
    vxl2_field->setAlignment(Qt::AlignCenter);
    vxl2_field->setValidator(new QRegExpValidator(vxl_regexp, vxl2_field));
    vxl3_field = new QLineEdit();
    vxl3_field->setAlignment(Qt::AlignCenter);
    vxl3_field->setValidator(new QRegExpValidator(vxl_regexp, vxl3_field));
    import_button = new QPushButton("Import volume");


    /*** LAYOUT SECTION ***/
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(import_form_desc_1, 0, 0, 1, 5);
    layout->addWidget(first_direction_label, 1, 2, 1, 1, Qt::AlignHCenter);
    layout->addWidget(second_direction_label, 1, 3, 1, 1, Qt::AlignHCenter);
    layout->addWidget(third_direction_label, 1, 4, 1, 1, Qt::AlignHCenter);
    layout->addWidget(axes_label, 2, 0, 1, 2);
    layout->addWidget(axs1_field, 2, 2, 1, 1, Qt::AlignHCenter);
    layout->addWidget(axs2_field, 2, 3, 1, 1, Qt::AlignHCenter);
    layout->addWidget(axs3_field, 2, 4, 1, 1, Qt::AlignHCenter);
    layout->addWidget(voxels_dims_label, 3, 0, 1, 2);
    layout->addWidget(vxl1_field, 3, 2, 1, 1, Qt::AlignHCenter);
    layout->addWidget(vxl2_field, 3, 3, 1, 1, Qt::AlignHCenter);
    layout->addWidget(vxl3_field, 3, 4, 1, 1, Qt::AlignHCenter);
    layout->addWidget(import_button, 4, 0, 1, 5, Qt::AlignJustify);
    layout->setSizeConstraint( QLayout::SetFixedSize );
    setLayout(layout);

    //windows flags and title
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);
    this->setWindowTitle("Additional informations are required");

    // signals and slots
    connect(import_button, SIGNAL(clicked()), this, SLOT(import_button_clicked()));
}


PDialogImport::~PDialogImport()
{
    #ifdef TSP_DEBUG
    printf("teramanager plugin [thread %d] >> PDialogImport destroyed\n", this->thread()->currentThreadId());
    #endif
}

/**********************************************************************************
* Called when <import_button> emits <click()> signal.
***********************************************************************************/
void PDialogImport::import_button_clicked()
{
    #ifdef TMP_DEBUG
    printf("teramanager plugin [thread %d] >> ok_button_clicked() called\n", this->thread()->currentThreadId());
    #endif

    try
    {
        int pos=0;
        QString tbv = axs1_field->text();
        if(axs1_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            axs1_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        tbv = axs2_field->text();
        if(axs2_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            axs2_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        tbv = axs3_field->text();
        if(axs3_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            axs3_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        tbv = vxl1_field->text();
        if(vxl1_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            vxl1_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        tbv = vxl2_field->text();
        if(vxl2_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            vxl2_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        tbv = vxl3_field->text();
        if(vxl3_field->validator()->validate(tbv,pos) != QValidator::Acceptable)
        {
            vxl3_field->setFocus();
            throw MyException("One or more fields not properly filled");
        }
        CImport::instance()->setAxes(axs1_field->text().toStdString().c_str(),
                                     axs2_field->text().toStdString().c_str(),
                                     axs3_field->text().toStdString().c_str());
        CImport::instance()->setVoxels(vxl1_field->text().toStdString().c_str(),
                                       vxl2_field->text().toStdString().c_str(),
                                       vxl3_field->text().toStdString().c_str());
        this->close();
    }
    catch(MyException &ex)
    {
        QMessageBox::critical(this,QObject::tr("Error"), QObject::tr(ex.what()),QObject::tr("Ok"));
    }
}
