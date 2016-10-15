#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtNetwork>
#include <QMessageBox>
#include <QtPositioning/QGeoPositionInfo>
#include <QtPositioning/QGeoPositionInfoSource>
#include <QStackedWidget>

namespace Ui {
	class Dialog;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog( QWidget *parent = 0 );
	~Dialog();

private:
	Ui::Dialog *ui;
		// Layout main menu
	QPushButton		*	buttonStartStopSend;
	QPushButton		*	buttonSettings;

		// Layout settings
	QPushButton		*	buttonApply;
	QPushButton		*	buttonMainMenu;
	QLineEdit		*	lineIp;
	QLabel			*	labelIp;
	QLineEdit		*	lineName;
	QLabel			*	labelName;

		// Стэк виджетов
	QStackedWidget	*	stackedWidget;

	QUdpSocket		*	socketClient;
	QTimer			*	timerSendMsg;

	QGeoCoordinate	*	geoCoordinate;

	char				currentLocation[1000];
	QString				ipAddress;
	QString				userName;

	void InitLayouts();
	bool CheckIP( const char* ip );

private slots:
	void SlotSendMsg();
	void SlotStartStopSend();
	void SlotPositionUpdated( const QGeoPositionInfo &info );
	void SlotSettings();
	void SlotMainMenu();
	void SlotApply();
};

#endif // DIALOG_H
