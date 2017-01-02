#include "modbusmaster.h"
#include "ui_modbusmaster.h"
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QModbusDataUnit>
#include <QDebug>
modbusMaster::modbusMaster(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::modbusMaster),m_holdingRegisters(20)
{
    ui->setupUi(this);
    ui->lineEdit->setText("/dev/pts/18");//设置默认串口号

    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }
    modbusDevice = new QModbusRtuSerialMaster(this);//QModbusRtuSerialMaster用于串口通信中的主机也就是client
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });
    connect(modbusDevice, &QModbusClient::stateChanged,
            this, &modbusMaster::onStateChanged);//连接状态发生改变时改变connect按钮上的显示文字（connect or discennect）


//    QByteArray text = QByteArray::fromHex("517420697320677265617421");
//    text.data();            // returns "Qt is great!"
//    qDebug()<<text.data();

}

modbusMaster::~modbusMaster()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
    delete ui; 
}

void modbusMaster::on_sendButton_clicked()
{
    if (!modbusDevice)//如果设备没有被创建就返回
        return;

    QString strDis;
    QString str1 = ui->textEdit->toPlainText();
    qDebug()<<str1;

    qDebug()<<"QserialPort:"<<int(QSerialPort::Baud115200);
    QByteArray str2 = QByteArray::fromHex (str1.toLatin1().data());//按十六进制编码接入文本
    //qDebug()<<str2;
    QString str3 = str2.toHex().data();//以十六进制显示

    statusBar()->clearMessage();//清除状态栏显示的信息
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters,0,10);//发送的数据信息（数据类型 ，起始地址，个数）
//    QModbusDataUnit::RegisterType table = writeUnit.registerType();//得到寄存器类型传给table
    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        int j = 2*i;
        QString st = str3.mid (j,2);
        bool ok;
        int hex =st.toInt(&ok,16);
        qDebug()<<hex;
        writeUnit.setValue(i,hex);//设置发送数据为123456789
    }
 //   auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1);//发送数据 1是server address
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {
        if (!reply->isFinished()) {   //reply 结束后返回fause就进相应的错误显示
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}

void modbusMaster::on_connectButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    QString comPortName = ui->lineEdit->text();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {//如果处在非连接状态，进行连接
        //进行串口连接
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            comPortName);//获取串口端口,下面设置各种参数
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
            QSerialPort::EvenParity);//偶较验
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
            QSerialPort::Baud19200);//波特率为19200
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
            QSerialPort::Data8);//数据位为8位
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
            QSerialPort::OneStop);//停止位为1位

        modbusDevice->setTimeout(1000);//连接超时1S
        modbusDevice->setNumberOfRetries(3);//连接失败重试三次连接
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    }else{//处在连接状态进行断开连接的操作
        modbusDevice->disconnectDevice();
    }

}

void modbusMaster::onStateChanged(int state)//更新connect按钮的显示状态
{

    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}

void modbusMaster::on_readButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,0,10);
    if (auto *reply = modbusDevice->sendReadRequest(readUnit, 1)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &modbusMaster::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}

void modbusMaster::readReady()//在这里读的数据
{
    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
    if (!reply)
        return;
//数据从QModbusReply这个类的resuil方法中获取,也就是本程序中的reply->result()
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
//            ui->readValue->addItem(entry);//把数据放在QListWidget中
            ui->textEdit_2->append(entry);
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }

    reply->deleteLater();
}
