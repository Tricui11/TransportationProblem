#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QRadioButton>
#include <QTableWidget>
#include <QTimer>
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
    void onPauseButtonClicked();
    void onPlayButtonClicked();

private:
    Ui::Dialog *ui;
    QSpinBox *supplierSpinBox;
    QSpinBox *demandSpinBox;
    QTableWidget *tableWidget;
    QLineEdit *infoLineEdit;
    QLineEdit *totalStagesLineEdit;
    QLineEdit *stageLineEdit;
    QRadioButton *minPriceRadioButton;
    ShipmentSolver *solver = nullptr;
    int currentMatrixIndex = 0;
    QTimer *animationTimer;

    void inputDataInSolver();
    void outputDataFromSolver();
    void animateMatrices();
    void SetStyleForQTableWidgetItem(QTableWidgetItem *item, QColor foregroundColor, QColor backgroundColor, bool isBold, int fontSize);
    void SetStyleSheetForQPushButton(QPushButton *button);
};
#endif // DIALOG_H
