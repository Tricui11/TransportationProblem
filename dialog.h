#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QRadioButton>
#include <QTableWidget>
#include <shipment_solver.cpp>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void onGenerateButtonClicked();
    void onSteppingStoneMethodButtonClicked();
    void onPotentialsMethodButtonClicked();

private:
    Ui::Dialog *ui;
    QSpinBox *supplierSpinBox;
    QSpinBox *demandSpinBox;
    QTableWidget *tableWidget;
    QLineEdit *infoLineEdit;
    QRadioButton *minPriceRadioButton;
    ShipmentSolver *solver = nullptr;

    void inputDataInSolver();
    void outputDataFromSolver();
};
#endif // DIALOG_H
