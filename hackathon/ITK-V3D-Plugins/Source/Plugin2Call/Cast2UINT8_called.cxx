#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "Cast2UINT8_called.h"
#include "V3DITKFilterSingleImage.h"

// ITK Header Files
#include "itkCastImageFilter.h"


// Q_EXPORT_PLUGIN2 ( PluginName, ClassName )
// The value of PluginName should correspond to the TARGET specified in the
// plugin's project file.
Q_EXPORT_PLUGIN2(Cast2Float, CastPlugin)


QStringList CastPlugin::menulist() const
{
    return QStringList() << QObject::tr("ITK Cast")
           << QObject::tr("about this plugin");
}

QStringList CastPlugin::funclist() const
{
    return QStringList()<< QObject::tr("ITK Cast")
           << QObject::tr("about this plugin");
}


template <typename TInputPixelType,typename TOutputPixelType>
class PluginSpecialized : public V3DITKFilterSingleImage< TInputPixelType, TOutputPixelType >
{
    typedef V3DITKFilterSingleImage< TInputPixelType, TOutputPixelType >   Superclass;
    typedef typename Superclass::Input3DImageType               InputImageType;
    typedef typename Superclass::Output3DImageType		OutputImageType;

    typedef itk::CastImageFilter< InputImageType, OutputImageType > FilterType;

public:

    PluginSpecialized( V3DPluginCallback * callback ): Superclass(callback)
    {
        this->m_Filter = FilterType::New();
        this->RegisterInternalFilter( this->m_Filter, 1.0 );
    }

    virtual ~PluginSpecialized() {};


    void Execute(V3DPluginCallback * callback , QWidget *parent)
    {
        this->Compute();
    }

    virtual void ComputeOneRegion()
    {

        this->m_Filter->SetInput( this->GetInput3DImage() );

        if( !this->ShouldGenerateNewWindow() )
        {
            this->m_Filter->InPlaceOn();
        }

        this->m_Filter->Update();

        this->SetOutputImage( this->m_Filter->GetOutput() );
    }
    bool ComputeOneRegion(const V3DPluginArgList & input, V3DPluginArgList & output)
    {
        V3DITKProgressDialog progressDialog( this->GetPluginName().toStdString().c_str() );

        this->AddObserver( progressDialog.GetCommand() );
        progressDialog.ObserveFilter( this->m_ProcessObjectSurrogate );
        progressDialog.show();
        this->RegisterInternalFilter( this->m_Filter, 1.0 );

        void * p=NULL;
        p=(void*)input.at(0).p;
        if(!p)perror("errro");
        std::cout<<"erro?\n"<<std::endl;
        this->m_Filter->SetInput((InputImageType*) p );
        std::cout<<"No erro\n"<<std::endl;
        try
        {
            this->m_Filter->Update();
        }
        catch(itk::ExceptionObject &excp)
        {
            std::cerr<<"erro"<<std::endl;
            std::cerr<<excp<<std::endl;
            return false;
        }
        V3DPluginArgItem arg;
        typename OutputImageType::Pointer outputImage = m_Filter->GetOutput();
        outputImage->Register();
        arg.p = (void*)outputImage;
        arg.type="UINT8Image";
        output.replace(0,arg);

        return true;
    }


private:

    typename FilterType::Pointer   m_Filter;

};


#define EXECUTE( v3d_pixel_type, input_pixel_type, output_pixel_type ) \
	case v3d_pixel_type: \
	{ \
		PluginSpecialized< input_pixel_type, output_pixel_type > runner(&callback); \
		runner.Execute(&callback, parent ); \
		break; \
	}

#define EXECUTE_ALL_PIXEL_TYPES \
	if (! p4DImage) return; \
	ImagePixelType pixelType = p4DImage->getDatatype(); \
	switch( pixelType )  \
	{  \
		EXECUTE( V3D_UINT8, unsigned char, float );  \
		EXECUTE( V3D_UINT16, unsigned short int, float );  \
		EXECUTE( V3D_FLOAT32, float, float );  \
		case V3D_UNKNOWN:  \
		{  \
		}  \
	}

bool CastPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output,
                        V3DPluginCallback2 & v3d,  QWidget * parent)
{
    if (func_name == QObject::tr("about this plugin"))
    {
        QMessageBox::information(parent, "Version info", "New Pugin for Other(developed by Yu Ping");
        return false ;
    }
    if (input.at(0).type == "FLOATImage") {
      PluginSpecialized<float,unsigned char> runner(&v3d);
    
      return runner.ComputeOneRegion(input, output);
    }
    else if (input.at(0).type == "UINT16Image") {
      PluginSpecialized<unsigned short int, unsigned char> runner(&v3d);
    
      return runner.ComputeOneRegion(input, output);
    }
    else if (input.at(0).type == "UINT8Image") {
    
      qDebug() << "Nothing to do ";
      output[0] = input.at(0);
      return true;
    }
    else 
    {
      qDebug() << "error to run the plugin";
      return false;
    }
}


void CastPlugin::domenu(const QString & menu_name, V3DPluginCallback2 & callback, QWidget * parent)
{
    if (menu_name == QObject::tr("about this plugin"))
    {
        QMessageBox::information(parent, "Version info", "ITK Cast 1.0 (2010-Jun-21): this plugin is developed by Sophie Chen.");
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

    EXECUTE_ALL_PIXEL_TYPES;
}

