#ifndef PLOG_H
#define PLOG_H

#include <QDialog>
#include "CPlugin.h"
#include "COperation.h"

class teramanager::PLog : public QDialog
{
        Q_OBJECT

    private:

        //GUI widgets
        QGroupBox* timeComponentsPanel;
        QTextEdit *timeComponents;
        float timeIO, timeGPU, timeCPU, timeSum, timeActual;
        /* ----------- */
        QGroupBox* timeOperationsPanel;
        QTextEdit *timeOperations;
        QCheckBox *enableIoCoreOperationsCheckBox;
        bool enableIoCoreOperations;
        /* ----------- */
        QGroupBox* logPanel;
        QTextEdit *log;
        /* ----------- */
        QPushButton* closeButton;
        QPushButton* resetButton;

        // other object members
        std::map< std::string, std::vector<itm::Operation*> > loggedOperations; // grouped by name

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "instance(...)"
        **********************************************************************************/
        static PLog* uniqueInstance;

        //extracts from the given time field in the corresponding time measure
        float toFloat(QString timeField);

        //update
        void update();

    public:

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "instance(...)"
        **********************************************************************************/
        static PLog* instance(QWidget* parent=0)
        {
            /**/itm::debug(itm::LEV_MAX, 0, __itm__current__function__);

            if (uniqueInstance == 0)
                uniqueInstance = new PLog(parent);
            return uniqueInstance;
        }
        static void uninstance()
        {
            if(uniqueInstance)
            {
                delete uniqueInstance;
                uniqueInstance = 0;
            }
        }
        PLog(QWidget *parent);

        /**********************************************************************************
        * <sendAppend> event handler
        ***********************************************************************************/
        void appendOperation(itm::Operation* op, bool update_time_comps = true);
        void append(std::string text);

        /**********************************************************************************
        * Called by algorithms running from different threads.
        * Emits <sendAppend> signal
        ***********************************************************************************/
        void emitSendAppend(void* op)
        {
            if(enableIoCoreOperations)
                sendAppend(op);
        }

    signals:

        /*********************************************************************************
        * Carries op informations
        **********************************************************************************/
        void sendAppend(void* op);

    public slots:

        /**********************************************************************************
        * <sendAppend> event handler
        ***********************************************************************************/
        void appendOperationVoid(void* op){appendOperation((itm::Operation*)(op), false);}

        void enableIoCoreOperationsCheckBoxChanged(int s){enableIoCoreOperations = enableIoCoreOperationsCheckBox->isChecked();}

        void reset();

};

#endif // PLOG_H
