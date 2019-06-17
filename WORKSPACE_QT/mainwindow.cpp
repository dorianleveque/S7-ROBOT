#include "mainwindow.h"
#include "ui_mainwindow.h"

//===================================================
#define ID_PC
#define ID_CARTE

#define ID_ROUES
#define ID_ROUES_UP
#define ID_ROUES_DOWN
#define ID_ROUES_LEFT
#define ID_ROUES_RIGHT

#define ID_TOURELLE
#define ID_TOURELLE_UP
#define ID_TOURELLE_DOWN
#define ID_TOURELLE_LEFT
#define ID_TOURELLE_RIGHT

#define ID_CONSIGNE
//===================================================

static QString nameport = "/dev/ttyACM0";
static QString zigbeePort = "/dev/ttyUSB0";


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initProgram();
    openSerialPort();
    openZigbeePort();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initProgram()
{
    setUp(false);
    setDown(false);
    setRight(false);
    setLeft(false);
    ui->labelTest->setHidden(false);
    update();
/*
    QPicture pict; // A voir plus tard
    bool success;
    success = pict.load("C:/Users/Elouan/Documents/SEN S7/Projet Robot/ihm_projetsen/WORKSPACE_QT/Images/up.png");
    //printf("Reussite du chargement de l'image: %d\n",success);
    qDebug()<<"Reussite du chargement de l'image: "<<success;
    ui->labelTest->setPicture(pict);
*/
    ui->slider->setFocusPolicy(Qt::NoFocus); // Pour dire au slider de ne pas recuperer les evenements claviers pour lui tout seul
}

bool MainWindow::event(QEvent *event) // Fonction surchargée qui va gerer les evenemments clavier
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Up)
        {
            setUp(true);
            update();
            robotAvancer();
            return true;
        }
        else if (ke->key() == Qt::Key_Down)
        {
            setDown(true);
            update();
            robotReculer();
            return true;
        }
        else if (ke->key() == Qt::Key_Left)
        {
            setLeft(true);
            update();
            robotGauche();
            return true;
        }
        else if (ke->key() == Qt::Key_Right)
        {
            setRight(true);
            update();
            robotDroite();
            return true;
        }

        else if (ke->key() == Qt::Key_Z)
        {
            //setTourelleUp(true);
            update();
            //tourelleUp();
            return true;
        }
        else if (ke->key() == Qt::Key_S)
        {
            //setTourelleDown(true);
            update();
            //tourelleDown();
            return true;
        }
        else if (ke->key() == Qt::Key_Q)
        {
            //setTourelleLeft(true);
            update();
            //tourelleGauche();
            return true;
        }
        else if (ke->key() == Qt::Key_D)
        {
            //setTourelleRight(true);
            update();
            //tourelleDroite();
            return true;
        }

        else
        {
            // Other key
            QByteArray dataZ = QByteArray("Z");
            serialZigbee->write(dataZ);
            qInfo()<<"Envoi au zigbee:"<<dataZ;
            return true;
        }
    }

    if (event->type() == QEvent::KeyRelease) // Le robot s'arrete
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if (ke->key() == Qt::Key_Up)
        {
            setUp(false);
            update();
            robotStop();
            return true;
        }
        else if (ke->key() == Qt::Key_Down)
        {
            setDown(false);
            update();
            robotStop();
            return true;
        }
        else if (ke->key() == Qt::Key_Left)
        {
            setLeft(false);
            update();
            robotStop();
            return true;
        }
        else if (ke->key() == Qt::Key_Right)
        {
            setRight(false);
            update();
            robotStop();
            return true;
        }

        else if (ke->key() == Qt::Key_Z)
        {
            //setTourelleUp(true);
            update();
            //tourelleStop();
            return true;
        }
        else if (ke->key() == Qt::Key_S)
        {
            //setTourelleDown(false);
            update();
            //tourelleStop();
            return true;
        }
        else if (ke->key() == Qt::Key_Q)
        {
            //setTourelleLeft(true);
            update();
            //tourelleStop();
            return true;
        }
        else if (ke->key() == Qt::Key_D)
        {
            //setTourelleRight(true);
            update();
            //tourelleStop();
            return true;
        }

        else
        {
            // Other key

            return true;
        }
    }

    return QWidget::event(event);
}

void MainWindow::update()
{
    if(isUp)
    {
        ui->keyboardUp->setHidden(false);
        ui->keyboardDown->setHidden(true);
        ui->keyboardLeft->setHidden(true);
        ui->keyboardRight->setHidden(true);
    }
    else if(isDown)
    {
        ui->keyboardUp->setHidden(true);
        ui->keyboardDown->setHidden(false);
        ui->keyboardLeft->setHidden(true);
        ui->keyboardRight->setHidden(true);
    }
    else if(isLeft)
    {
        ui->keyboardUp->setHidden(true);
        ui->keyboardDown->setHidden(true);
        ui->keyboardLeft->setHidden(false);
        ui->keyboardRight->setHidden(true);
    }
    else if(isRight)
    {
        ui->keyboardUp->setHidden(true);
        ui->keyboardDown->setHidden(true);
        ui->keyboardLeft->setHidden(true);
        ui->keyboardRight->setHidden(false);
    }
    else // On est dans aucun des cas => toutes les touches relachées
    {
        ui->keyboardUp->setHidden(true);
        ui->keyboardDown->setHidden(true);
        ui->keyboardLeft->setHidden(true);
        ui->keyboardRight->setHidden(true);
    }
}


void MainWindow::setUp(bool set)
{
    if(set)
    {
        isUp=true;isDown=false;isLeft=false;isRight=false;
    }
    else
    {
        isUp=false;isDown=false;isLeft=false;isRight=false; // On met tout à false
    }
}
void MainWindow::setDown(bool set)
{
    if(set)
    {
        isUp=false;isDown=true;isLeft=false;isRight=false;
    }
    else
    {
        isUp=false;isDown=false;isLeft=false;isRight=false;
    }
}
void MainWindow::setLeft(bool set)
{
    if(set)
    {
        isUp=false;isDown=false;isLeft=true;isRight=false;
    }
    else
    {
        isUp=false;isDown=false;isLeft=false;isRight=false;
    }
}
void MainWindow::setRight(bool set)
{
    if(set)
    {
        isUp=false;isDown=false;isLeft=false;isRight=true;
    }
    else
    {
        isUp=false;isDown=false;isLeft=false;isRight=false;
    }
}

/*
bool QSlider::event(QEvent *event) override
{
    if ((event->type() == QEvent::KeyPress) || (event->type() == QEvent::KeyRelease))
    {
        return true;
    }
}
*/

void MainWindow::robotAvancer()
{
#if USE_LINUX
    sendData= QByteArray("A"); // Si on envoie plusieurs caracteres, il faut le changer dans le programme carte. Sinon la callback ne se declenche pas.
    writeData(sendData);
    //readData(); // ça renvoie bien
#else
    qInfo()<<"Fonction avancer";
#endif
}
void MainWindow::robotReculer()
{
#if USE_LINUX
    sendData= QByteArray("R");
    writeData(sendData);
    //readData(); // ça renvoie bien
#else
    qInfo()<<"Fonction reculer";
#endif
}
void MainWindow::robotGauche()
{
#if USE_LINUX
    sendData= QByteArray("G");
    writeData(sendData);
    //readData(); // ça renvoie bien
#else
    qInfo()<<"Fonction gauche";
#endif
}
void MainWindow::robotDroite()
{
#if USE_LINUX
    sendData= QByteArray("D");
    writeData(sendData);
    //readData(); // ça renvoie bien
#else
    qInfo()<<"Fonction droite";
#endif
}

void MainWindow::robotStop()
{
#if USE_LINUX
    sendData= QByteArray("S");
    writeData(sendData);
    //readData(); // ça renvoie bien
#else
    qInfo()<<"Fonction stop";
#endif
}

// Les fonctions pour la tourelle vont etre à faire

void MainWindow::openSerialPort() // Il faut brancher une carte pour que cela fonctionne
{
#if USE_LINUX
    serial = new QSerialPort(this);
    serial->setPortName(nameport);
    serial->open(QIODevice::ReadWrite);

     if( serial->isOpen()==false)
     {
          serial->clearError();
          QMessageBox::critical(this, "Port error", "Port: "+nameport);
          QMessageBox::information(this, "Port error", "Vérifier nom du port \n Fermer tout programme utilisant la lisaison RS232 "+nameport);
      }
   else
     {
         QMessageBox::information(this, "Port open", " "+nameport);
          serial->setBaudRate(QSerialPort::Baud115200);
          serial->setStopBits(QSerialPort::OneStop);
          serial->setParity(QSerialPort::NoParity);
          serial->setDataBits(QSerialPort::Data8);
          serial->setFlowControl(QSerialPort::NoFlowControl);
     }
#else
    qInfo()<<"Version windows de la fonction openSerialPort. Nom du port: "<<nameport;
#endif
}


void MainWindow::openZigbeePort() // Il faut brancher une carte pour que cela fonctionne
{
#if USE_LINUX
    serialZigbee = new QSerialPort(this);
    serialZigbee->setPortName(zigbeePort);
    serialZigbee->open(QIODevice::ReadWrite);

     if( serialZigbee->isOpen()==false)
     {
          serialZigbee->clearError();
          QMessageBox::critical(this, "Port error", "Port: "+zigbeePort);
          QMessageBox::information(this, "Port error", "Vérifier nom du port \n Fermer tout programme utilisant la lisaison RS232 "+zigbeePort);
      }
   else
     {
         QMessageBox::information(this, "Port open", " "+zigbeePort);
          serialZigbee->setBaudRate(QSerialPort::Baud9600);
          serialZigbee->setStopBits(QSerialPort::OneStop);
          serialZigbee->setParity(QSerialPort::NoParity);
          serialZigbee->setDataBits(QSerialPort::Data8);
          serialZigbee->setFlowControl(QSerialPort::NoFlowControl);
     }
#else
    qInfo()<<"Version windows de la fonction openZigbeePort. Nom du port: "<<zigbeePort;
#endif
}



void MainWindow::onButSendClicked()
{
    /*
    QString message=mes_to_send->text();
    writeData(message.toUtf8()); // QString --> QByteArray
    */
}


void MainWindow::writeData(const QByteArray &data)
{
#if USE_LINUX
    serial->write(data);
    qInfo()<<"Message ecrit:"<<data;

#else
    qInfo()<<"Fonction write data";
#endif
}

void MainWindow::readData()
{
#if USE_LINUX
    QByteArray data = serial->readAll();
    qInfo()<<"Data receive:"<<data;
#else

#endif
}
