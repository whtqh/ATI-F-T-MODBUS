#include "modbusslave.h"
#include "ui_modbusslave.h"
#include <QModbusDevice>
#include <QSerialPort>
#include <QModbusRtuSerialSlave>
#include <QDebug>
modbusSlave::modbusSlave(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::modbusSlave)
{
    ui->setupUi(this);
    modbusDevice = new QModbusRtuSerialSlave(this);

    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 10 });//
    reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 10 });

    modbusDevice->setMap(reg);//把reg设置给了全局的m_modbusDataUnitMap
    connect(modbusDevice, &QModbusServer::stateChanged,
            this, &modbusSlave::onStateChanged);//连接状态发生改变时改变connect按钮上的显示文字（connect or discennect）


    connect(modbusDevice, &QModbusServer::dataWritten,
            this, &modbusSlave::updateWidgets);//modbus客户端向服务器写入数据时dataWritten信号被触发
    connect(modbusDevice, &QModbusServer::errorOccurred,
            this, &modbusSlave::handleDeviceError);



}

modbusSlave::~modbusSlave()
{
    delete ui;
    if (modbusDevice)
        modbusDevice->disconnectDevice();//清除
    delete modbusDevice;
}

void modbusSlave::on_connectButton_clicked()
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

        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    }else{//处在连接状态进行断开连接的操作
        modbusDevice->disconnectDevice();
    }
}

void modbusSlave::onStateChanged(int state)//判断联连状态是否改变，如改变则更改连接按钮上的文字
{

    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}

void modbusSlave::updateWidgets(QModbusDataUnit::RegisterType table, int address, int size)
{
    for (int i = 0; i < size; ++i) {
        quint16 value;

        switch (table) {
        case QModbusDataUnit::Coils:
            modbusDevice->data(QModbusDataUnit::Coils, address + i, &value);
            qDebug()<<value;
            break;
        case QModbusDataUnit::HoldingRegisters:
            modbusDevice->data(QModbusDataUnit::HoldingRegisters, address + i, &value);//得到数据后做出显示
            qDebug()<<value;
            break;
        default:
            break;
        }
        QString s = QString::number(value, 16);
        qDebug("%d",value);
        ui->textEdit->append(s);
    }

}

void modbusSlave::handleDeviceError(QModbusDevice::Error newError)//错误提示
{
    if (newError == QModbusDevice::NoError || !modbusDevice)
        return;

    statusBar()->showMessage(modbusDevice->errorString(), 5000);
}

void modbusSlave::on_clearButton_clicked()//清除texiedit上的文本
{
    ui->textEdit->clear();
}

void modbusSlave::on_writeButton_clicked()
{
    QString str = ui->textEdit_2->toPlainText();
    QString str2 = str.replace(" ","");

    for(int i = 0 ;i < 10;i++)
    {
       int j = 2*i;
       QString str1 = str2.mid(j,2);
       bool ok;
       quint16 hex = str1.toInt(&ok,16);
       qDebug("%d",hex);
       ok = modbusDevice->setData(QModbusDataUnit::HoldingRegisters, i, hex);
     //  modbusDevice->data(QModbusDataUnit::HoldingRegisters, i, &hex);//得到数据后做出显示
    }
}
