/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/uxa_dashboard/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace uxa_dashboard {

using namespace Qt;
using namespace std;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent)
	, qnode(argc,argv)
{
	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application

    ReadSettings();
	setWindowIcon(QIcon(":/images/icon.png"));
	ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

	/*********************
	** Logging
	**********************/
    // Ros info
	ui.view_logging->setModel(qnode.loggingModel());
    QObject::connect(&qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));
    // Communicate status
    ui.view_status->setModel(qnode.consoleModel());
    QObject::connect(&qnode, SIGNAL(consoleUpdated()), this, SLOT(updateConsoleView()));
    /*********************
    ** Auto Start
    **********************/
    if ( ui.checkbox_remember_settings->isChecked() ) {
        on_button_connect_clicked(true);
    }
}

MainWindow::~MainWindow() {}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
    close();
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */

//void MainWindow::on_button_connect_clicked(bool check ) {
//	if ( ui.checkbox_use_environment->isChecked() ) {
//        cout << "asd" << endl;
//		if ( !qnode.init() ) {
//			showNoMasterMessage();
//		} else {
//			ui.button_connect->setEnabled(false);
//		}
//	} else {
//		if ( ! qnode.init(ui.line_edit_master->text().toStdString(),
//				   ui.line_edit_host->text().toStdString()) ) {
//			showNoMasterMessage();
//		} else {
//			ui.button_connect->setEnabled(false);
//			ui.line_edit_master->setReadOnly(true);
//			ui.line_edit_host->setReadOnly(true);
//			ui.line_edit_topic->setReadOnly(true);
//		}
//    }
//}

void MainWindow::on_button_connect_clicked(bool check ) {    
    if ( !qnode.init() ) {// connect error
        showNoMasterMessage();
    } else {//connect success
        ui.button_connect->setEnabled(false);

    }
}


void MainWindow::on_checkbox_use_environment_stateChanged(int state) {
	bool enabled;
	if ( state == 0 ) {
		enabled = true;
	} else {
		enabled = false;
	}
	ui.line_edit_master->setEnabled(enabled);
	ui.line_edit_host->setEnabled(enabled);
    ui.line_edit_topic->setEnabled(enabled);
}

/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
void MainWindow::updateLoggingView() {
//        ui.view_logging->scrollToBottom();
        ui.view_status->scrollToBottom();
}

void MainWindow::updateConsoleView() {
//        ui.view_logging->scrollToBottom();
        ui.view_status->scrollToBottom();
}

/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("About ..."),tr("<h2>PACKAGE_NAME Test Program 0.10</h2><p>Copyright Yujin Robot</p><p>This package needs an about description.</p>"));
}

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/

void MainWindow::ReadSettings() {
    QSettings settings("Qt-Ros Package", "uxa_dashboard");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QString master_url = settings.value("master_url",QString("http://192.168.1.2:11311/")).toString();
    QString host_url = settings.value("host_url", QString("192.168.1.3")).toString();
    //QString topic_name = settings.value("topic_name", QString("/chatter")).toString();
    ui.line_edit_master->setText(master_url);
    ui.line_edit_host->setText(host_url);
    //ui.line_edit_topic->setText(topic_name);
    bool remember = settings.value("remember_settings", false).toBool();
    ui.checkbox_remember_settings->setChecked(remember);
    bool checked = settings.value("use_environment_variables", false).toBool();
    ui.checkbox_use_environment->setChecked(checked);
    if ( checked ) {
    	ui.line_edit_master->setEnabled(false);
    	ui.line_edit_host->setEnabled(false);
    	//ui.line_edit_topic->setEnabled(false);
    }
}

void MainWindow::WriteSettings() {
    QSettings settings("Qt-Ros Package", "uxa_dashboard");
    settings.setValue("master_url",ui.line_edit_master->text());
    settings.setValue("host_url",ui.line_edit_host->text());
    //settings.setValue("topic_name",ui.line_edit_topic->text());
    settings.setValue("use_environment_variables",QVariant(ui.checkbox_use_environment->isChecked()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("remember_settings",QVariant(ui.checkbox_remember_settings->isChecked()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	WriteSettings();
	QMainWindow::closeEvent(event);
}

void MainWindow::on_dial_2_sliderMoved(int position)
{
    cout << "position : " << position << endl;
    qnode.send_std_position((unsigned int)position);
}
void MainWindow::on_dial_2_valueChanged(int position)
{
    cout << "position : " << position << endl;
    qnode.send_std_position((unsigned int)position);
}

void MainWindow::on_pushButton_clicked()
{
    qnode.send_msg("pc_control");
}

void MainWindow::on_pushButton_2_clicked()
{
    qnode.send_msg("stop");

}

// Sitdown
void MainWindow::on_btnSitdown_clicked()
{
    qnode.send_msg(BTN_0);

}

void MainWindow::on_btnInitSerial_clicked()
{
    unsigned char Trans_chr[_SERIAL_BUFF_SIZE];
    unsigned char Recev_chr[_SERIAL_BUFF_SIZE];
    unsigned char cnt = 0;

    Trans_chr[cnt++] = 0xFF; //1
    Trans_chr[cnt++] = 0xFF; //2
    Trans_chr[cnt++] = 0xAA; //3
    Trans_chr[cnt++] = 0x55; //4
    Trans_chr[cnt++] = 0xAA; //5
    Trans_chr[cnt++] = 0x55; //6
    Trans_chr[cnt++] = 0x37; //7
    Trans_chr[cnt++] = 0xBA; //8

    Trans_chr[cnt++] = 0x10; //9
    Trans_chr[cnt++] = 0x00; //10
    Trans_chr[cnt++] = 0x00; //11
    Trans_chr[cnt++] = 0x00; //12
    Trans_chr[cnt++] = 0x00; //13
    Trans_chr[cnt++] = 0x01; //14
    Trans_chr[cnt++] = 0x01; //15
    Trans_chr[cnt++] = 0x01; //16

    qnode.Send_Serial_String(qnode.Serial, Trans_chr, cnt);
    sleep(1);

    cnt = 0;

    Trans_chr[cnt++] = 0xFF;
    Trans_chr[cnt++] = (unsigned char)(7 << 5);
    Trans_chr[cnt++] = 225;
    Trans_chr[cnt++] = 0;
    Trans_chr[cnt++] = 0X07;
    Trans_chr[cnt++] = (Trans_chr[1]^Trans_chr[2]^Trans_chr[3]^Trans_chr[4]) & 0x7F;

    qnode.Send_Serial_String(qnode.Serial, Trans_chr, cnt);
    sleep(1);

    memset(Trans_chr, '\0', sizeof(Trans_chr));
    memset(Recev_chr, '\0', sizeof(Recev_chr));

}

void MainWindow::on_btnIntroTask_clicked()
{


}

void MainWindow::on_btnInitMotion_clicked()
{
    qnode.send_msg(BTN_A);
}

void MainWindow::on_GetPosition_clicked()
{

    qnode.get_position(24);

}

}  // namespace uxa_dashboard




