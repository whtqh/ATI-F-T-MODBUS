#ifndef MODBUSSLAVE_H
#define MODBUSSLAVE_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QModbusServer>
namespace Ui {
class modbusSlave;
}

class modbusSlave : public QMainWindow
{
    Q_OBJECT

public:
    explicit modbusSlave(QWidget *parent = 0);
    ~modbusSlave();

private slots:
    void on_connectButton_clicked();

    void onStateChanged(int state);
    void updateWidgets(QModbusDataUnit::RegisterType table, int address, int size);

    void on_clearButton_clicked();
    void handleDeviceError(QModbusDevice::Error newError);
    void on_writeButton_clicked();

private:
    Ui::modbusSlave *ui;
    QModbusServer *modbusDevice;//QModbusClient被QModbusRtuSerialMaster和QModbusTcpClient继成
};

#endif // MODBUSSLAVE_H
