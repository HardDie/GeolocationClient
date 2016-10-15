#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
	ui->setupUi(this);
	InitLayouts();
		// Инициализация полей класса
	// Таймер
	timerSendMsg = new QTimer( this );
	timerSendMsg->setInterval( 1000 );
	// UDP Сокет
	socketClient = new QUdpSocket();


		// Создание обьекта для отслеживания геопозиции и привязка его обновления к слоту
	QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(this);
	if (source) {
		connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(SlotPositionUpdated(QGeoPositionInfo)));
		source->startUpdates();
	}

		// Подключаем кнопки к слотам
	connect( buttonStartStopSend, SIGNAL( clicked(bool) ), this, SLOT( SlotStartStopSend() ) );
	connect( buttonSettings, SIGNAL( clicked(bool) ), this, SLOT( SlotSettings() ) );
	connect( buttonMainMenu, SIGNAL( clicked(bool) ), this, SLOT( SlotMainMenu() ) );

		// Подключаем таймер к слоту
	connect( timerSendMsg, SIGNAL( timeout() ), this, SLOT( SlotSendMsg() ) );

	//SlotMainMenu();
}

Dialog::~Dialog()
{
	delete ui;
}

void Dialog::InitLayouts() {
			// Создание виджета для главного меню
		buttonStartStopSend = new QPushButton( "Start send" );
		buttonSettings		= new QPushButton( "Settings" );

		QWidget* mainMenu = new QWidget;
		QVBoxLayout* layoutMainMenu = new QVBoxLayout;
		layoutMainMenu->setAlignment( Qt::AlignHCenter );
		layoutMainMenu->addWidget( buttonStartStopSend );
		layoutMainMenu->addWidget( buttonSettings );
		mainMenu->setLayout( layoutMainMenu );

			// Создание виджета для настроек
		buttonApply = new QPushButton( "Apply" );
		buttonMainMenu = new QPushButton( "Main menu" );

		QRegExp ipExp( "[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}" );
		lineIp = new QLineEdit( "192.168.1.1" );
		lineIp->setValidator( new QRegExpValidator( ipExp, this ) );
		labelIp = new QLabel( "Enter IP" );

		QRegExp nameExp( "[0-9A-Za-z]{1,30}" );
		lineName = new QLineEdit( "Default" );
		lineName->setValidator( new QRegExpValidator( nameExp, this ) );
		labelName = new QLabel( "Enter Name" );

		QWidget* settings = new QWidget;
		QHBoxLayout* ipString = new QHBoxLayout;
		ipString->setAlignment( Qt::AlignHCenter );
		ipString->addWidget( labelIp );
		ipString->addWidget( lineIp );
		QHBoxLayout* nameString = new QHBoxLayout;
		nameString->setAlignment( Qt::AlignHCenter );
		nameString->addWidget( labelName );
		nameString->addWidget( lineName );
		QVBoxLayout* layoutSettings = new QVBoxLayout;
		layoutSettings->setAlignment( Qt::AlignHCenter );
		layoutSettings->addLayout( ipString );
		layoutSettings->addLayout( nameString );
		layoutSettings->addWidget( buttonApply );
		layoutSettings->addWidget( buttonMainMenu );
		settings->setLayout( layoutSettings );

			// Добавление виджетов в стэк
		stackedWidget = new QStackedWidget;
		stackedWidget->addWidget( mainMenu );
		stackedWidget->addWidget( settings );

			// Инициализация виджета главного окна
		QVBoxLayout *layoutMain = new QVBoxLayout;
		layoutMain->addWidget( stackedWidget );
		setLayout( layoutMain );
}

void Dialog::SlotSendMsg() {
	char sendStr[1000];
	strcpy( sendStr, currentLocation );
	strcat( sendStr, "\0" );
	socketClient->writeDatagram( sendStr, strlen( sendStr ) + 1, QHostAddress( lineIp->text() ), 3000 );
}

void Dialog::SlotPositionUpdated(const QGeoPositionInfo &info) {
	sprintf( currentLocation, "%s\n%s", lineName->text().toStdString().c_str(), info.coordinate().toString().toStdString().c_str() );
}

void Dialog::SlotStartStopSend() {
	if ( buttonStartStopSend->text() == "Start send" ) {
		buttonStartStopSend->setText( "Stop send" );
		timerSendMsg->start();
	} else {
		buttonStartStopSend->setText( "Start send" );
		timerSendMsg->stop();
	}
}

void Dialog::SlotSettings() {
	stackedWidget->setCurrentIndex( 1 );
}

void Dialog::SlotMainMenu() {
	stackedWidget->setCurrentIndex( 0 );
}
