#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "CurvatureFlow_called.h"
#include "V3DITKFilterSingleImage.h"

// ITK Header Files
#include "itkCastImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"


// Q_EXPORT_PLUGIN2 ( PluginName, ClassName )
// The value of PluginName should correspond to the TARGET specified in the
// plugin's project file.
Q_EXPORT_PLUGIN2(CurvatureFlow, CurvatureFlowPlugin)


QStringList CurvatureFlowPlugin::menulist() const
{
    return QStringList() << QObject::tr("ITK CurvatureFlow")
           << QObject::tr("about this plugin");
}

QStringList CurvatureFlowPlugin::funclist() const
{
    return QStringList()<< QObject::tr("ITK CurvatureFlow")
           << QObject::tr("about this plugin");
}


template <typename TPixelType>
class PluginSpecialized : public V3DITKFilterSingleImage< TPixelType, TPixelType >
{
    typedef V3DITKFilterSingleImage< TPixelType, TPixelType >   Superclass;
    typedef typename Superclass::Input3DImageType               ImageType;

    typedef itk::Image< float, 3 >                              FloatImageType;

    typedef itk::CastImageFilter< ImageType, FloatImageType >   InputCastFilterType;
    typedef itk::CurvatureFlowImageFilter< FloatImageType, FloatImageType > FilterType;
    typedef itk::CastImageFilter< FloatImageType, ImageType >   OutputCastFilterType;

public:

    PluginSpecialized( V3DPluginCallback * callback ): Superclass(callback)
    {
        this->m_Filter = FilterType::New();
        this->m_InputCast = InputCastFilterType::New();
        this->m_OutputCast = OutputCastFilterType::New();

        this->RegisterInternalFilter( this->m_InputCast, 0.1 );
        this->RegisterInternalFilter( this->m_Filter, 0.8 );
        this->RegisterInternalFilter( this->m_OutputCast, 0.1 );
    }

    virtual ~PluginSpecialized() {};


    void Execute(const QString &menu_name, QWidget *parent)
    {
        V3DITKGenericDialog dialog("CurvatureFlow");

        dialog.AddDialogElement("Iterations",2.0, 1.0, 10.0);
        dialog.AddDialogElement("TimeStep",0.06, 0.001, 0.625);

        if( dialog.exec() == QDialog::Accepted )
        {
            this->m_Filter->SetNumberOfIterations( dialog.GetValue("Iterations") );
            this->m_Filter->SetTimeStep( dialog.GetValue("TimeStep") );

            this->Compute();
        }
    }

    virtual void ComputeOneRegion()
    {

        this->m_InputCast->SetInput( this->GetInput3DImage() );
        this->m_Filter->SetInput( this->m_InputCast->GetOutput() );
        this->m_OutputCast->SetInput( this->m_Filter->GetOutput() );

        if( !this->ShouldGenerateNewWindow() )
        {
            this->m_Filter->InPlaceOn();
        }

        this->m_OutputCast->Update();

        this->SetOutputImage( this->m_OutputCast->GetOutput() );
    }

    bool ComputeOneRegion(const V3DPluginArgList & input, V3DPluginArgList & output)
    {

        V3DITKGenericDialog dialog("CurvatureFlow");

        dialog.AddDialogElement("Iterations",2.0, 1.0, 10.0);
        dialog.AddDialogElement("TimeStep",0.05, 0.001, 0.625);


        if( dialog.exec() == QDialog::Accepted )
        {
            this->m_Filter->SetNumberOfIterations( dialog.GetValue("Iterations") );
            this->m_Filter->SetTimeStep( dialog.GetValue("TimeStep") );

            V3DITKProgressDialog progressDialog( this->GetPluginName().toStdString().c_str() );

            this->AddObserver( progressDialog.GetCommand() );
            progressDialog.ObserveFilter( this->m_ProcessObjectSurrogate );
            progressDialog.show();
            this->RegisterInternalFilter( this->m_Filter, 1.0 );

            void * p=NULL;
            p=(void*)input.at(0).p;
            if(!p)perror("errro");
            this->m_InputCast->SetInput((ImageType*) p);
            this->m_Filter->SetInput(m_InputCast->GetOutput());
            this->m_OutputCast->SetInput(this->m_Filter->GetOutput());

            this->m_OutputCast->Update();
            V3DPluginArgItem arg;
            typename ImageType::Pointer outputImage = this->m_OutputCast->GetOutput();
            outputImage->Register();
            arg.p = (void*) outputImage;
            arg.type="UINT8Image";
            output.replace(0,arg);
            return true;
        }
        else return false;
    }


private:

    typename FilterType::Pointer              m_Filter;
    typename InputCastFilterType::Pointer     m_InputCast;
    typename OutputCastFilterType::Pointer    m_OutputCast;

};


#define EXECUTE_PLUGIN_FOR_ONE_IMAGE_TYPE( v3d_pixel_type, c_pixel_type ) \
  case v3d_pixel_type: \
    { \
    PluginSpecialized< c_pixel_type > runner( &callback ); \
    runner.Execute( menu_name, parent ); \
    break; \
    }


bool CurvatureFlowPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output,
                                 V3DPluginCallback2 & v3d,  QWidget * parent)
{
    if (func_name == QObject::tr("about this plugin"))
    {
        QMessageBox::information(parent, "Version info", "New Pugin for Other(developed by Yu Ping");
        return false ;
    }
    PluginSpecialized<unsigned char> runner(&v3d);
    bool result = runner.ComputeOneRegion(input, output);
    return result;
}


void CurvatureFlowPlugin::domenu(const QString & menu_name, V3DPluginCallback2 & callback, QWidget * parent)
{
    if (menu_name == QObject::tr("about this plugin"))
    {
        QMessageBox::information(parent, "Version info", "ITK CurvatureFlow 1.0 (2010-Jun-21): this plugin is developed by Luis Ibanez.");
        return;
    }

    v3dhandle curwin = callback.currentImageWindow();
    if (!curwin)
    {
        v3d_msg(tr("You don't have any image open in the main window."));
        return;
    }

    Image4DSimple *p4DImage = callback.getImage(curwin);
    if (! p4DImage)
    {
        v3d_msg(tr("The input image is null."));
        return;
    }

    EXECUTE_PLUGIN_FOR_ALL_PIXEL_TYPES;
}

