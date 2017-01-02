#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QModbusClient>

namespace Ui {
class modbusMaster;
}

class modbusMaster : public QMainWindow
{
    Q_OBJECT

public:
    explicit modbusMaster(QWidget *parent = 0);
//    QStringList list;
    ~modbusMaster();

private slots:
    void on_sendButton_clicked();

    void on_connectButton_clicked();
    void onStateChanged(int state);

    void on_readButton_clicked();
    void readReady();
private:
    Ui::modbusMaster *ui;
    QModbusClient *modbusDevice;//QModbusClient被QModbusRtuSerialMaster和QModbusTcpClient继成
    QVector<quint16> m_holdingRegisters;

};

#endif // MODBUSMASTER_H
