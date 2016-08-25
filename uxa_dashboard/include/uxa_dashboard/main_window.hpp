/**
 * @file /include/uxa_dashboard/main_window.hpp
 *
 * @brief Qt based gui for uxa_dashboard.
 *
 * @date November 2010
 **/
#ifndef uxa_dashboard_MAIN_WINDOW_H
#define uxa_dashboard_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"


#define BTN_A       0x01
#define BTN_B       0x02
#define BTN_LR      0x03
#define BTN_U       0x04
#define BTN_RR      0x05
#define BTN_L       0x06
#define BTN_C       0x07
#define BTN_R       0x08
#define BTN_LA      0x09
#define BTN_D       0x0A
#define BTN_RA      0x0B

#define BTN_1       0x0C
#define BTN_2       0x0D
#define BTN_3       0x0E
#define BTN_4       0x0F
#define BTN_5       0x10
#define BTN_6       0x11
#define BTN_7       0x12
#define BTN_8       0x13
#define BTN_9       0x14
#define BTN_0       0x15

#define BTN_STAR_A  0x16
#define BTN_STAR_B  0x17
#define BTN_STAR_LR 0x18
#define BTN_STAR_U  0x19
#define BTN_STAR_RR 0x1A
#define BTN_STAR_L  0x1B
#define BTN_STAR_C  0x1C
#define BTN_STAR_R  0x1D
#define BTN_STAR_LA 0x1E
#define BTN_STAR_D  0x1F
#define BTN_STAR_RA 0x20

#define BTN_STAR_1  0x21
#define BTN_STAR_2  0x22
#define BTN_STAR_3  0x23
#define BTN_STAR_4  0x24
#define BTN_STAR_5  0x25
#define BTN_STAR_6  0x26
#define BTN_STAR_7  0x27
#define BTN_STAR_8  0x28
#define BTN_STAR_9  0x29
#define BTN_STAR_0  0x2A


/*****************************************************************************
** Namespace
*****************************************************************************/

namespace uxa_dashboard {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void ReadSettings(); // Load up qt program settings at startup
	void WriteSettings(); // Save qt program settings when closing

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/
	void on_actionAbout_triggered();
	void on_button_connect_clicked(bool check );
    void on_checkbox_use_environment_stateChanged(int state);

    /******************************************
    ** Manual connections
    *******************************************/
    void updateLoggingView(); // no idea why this can't connect automatically
    void updateConsoleView(); // no idea why this can't connect automatically

private Q_SLOTS:
        void on_dial_2_sliderMoved(int position);

        void on_dial_2_valueChanged(int position);

        void on_pushButton_clicked();

        void on_pushButton_2_clicked();

        void on_btnSitdown_clicked();

        void on_btnInitSerial_clicked();

        void on_btnIntroTask_clicked();

        void on_btnInitMotion_clicked();

        void on_GetPosition_clicked();

private:
	Ui::MainWindowDesign ui;
	QNode qnode;
};

}  // namespace uxa_dashboard

#endif // uxa_dashboard_MAIN_WINDOW_H
