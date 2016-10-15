#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog( QWidget *parent ) : QDialog( parent ), ui( new Ui::Dialog ) {
	ui->setupUi(this);
	InitLayouts();
		// Инициализация полей класса
	// Таймер
	timerSendMsg = new QTimer( this );
	timerSendMsg->setInterval( 1000 );
	// UDP Сокет
	socketClient = new QUdpSocket();
	// IP и UserName
	ipAddress = "192.168.1.1";
	userName = "Default";


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
	connect( buttonApply, SIGNAL( clicked(bool) ), this, SLOT( SlotApply() ) );

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
		lineIp = new QLineEdit;
		lineIp->setValidator( new QRegExpValidator( ipExp, this ) );
		labelIp = new QLabel( "Enter IP" );

		QRegExp nameExp( "[0-9A-Za-z]{1,30}" );
		lineName = new QLineEdit;
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

void Dialog::SlotPositionUpdated( const QGeoPositionInfo &info ) {
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
	lineIp->setText( ipAddress );
	lineName->setText( userName );
}

void Dialog::SlotMainMenu() {
	stackedWidget->setCurrentIndex( 0 );
}

void Dialog::SlotApply() {
	if ( CheckIP( lineIp->text().toStdString().c_str() ) ) {
		QMessageBox *errorMsg = new QMessageBox();
		errorMsg->setText( "Wrong ip address!" );
		errorMsg->exec();
		return;
	}
	ipAddress = lineIp->text();
	userName = lineName->text();
}

bool Dialog::CheckIP( const char* ip ) {

	if ( strcmp( ip, "localhost" ) == 0 ) {
		return false;
	}

	if ( strlen( ip ) > 15 || strlen( ip ) < 7 ) {  // Проверка на длину IP адреса
		return true;
	}

	char dotCount = 0;
	for ( int i = 0; i < strlen( ip ); i++ ) {
		if ( ip[i] == '.' ) { 	// Проверка но количество точек
			dotCount++;
			continue;
		}
		if ( ip[i] < 48 || ip[i] > 57 ) {  // Проверка на то, является ли символ цифрой
			return true;
		}
	}
	if ( dotCount != 3 ) {
		return true;
	}

		// Проверка значений ip адреса, начинаем с конца строки
	for ( int i = strlen( ip ) - 1, val = -1, mn = 1; i >= -1; i-- ) {

		if ( i == -1 || ip[i] == '.' ) {  // Проверка на превышения интервала или если первый символ '.'
			if ( val > 255 || val == -1 ) {
				return true;
			}
			val = 0;
			mn = 1;
		} else {
			val += mn * ( ip[i] - 48 );
			mn *= 10;
		}

		if ( !(( i == 0 ) || ( i == strlen( ip ) - 1 )) ) {  // Не учитываем в проверке первый и последний символ
			if ( ip[i] == '0' && ip[i - 1] == '.' && ip[i + 1] != '.' ) { // Если 0 в значении стоит первым, и после него нету точки, то это неверно
				return true;
			}
		} else if ( i == 0 && ip[i] == '0' && ip[i + 1] != '.' ) {	// Если первый символ в строке 0, и после нее нет точки, ошибка
			return true;
		}

	}

	return false;
}
