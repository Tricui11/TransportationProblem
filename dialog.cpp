#include <dialog.h>
#include <ui_dialog.h>
#include <shipment_solver.cpp>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_pushButton_solve_clicked()
{
//    ShipmentSolver *solver = new ShipmentSolver();
//  //  solver->process("input1.txt");
// //   solver->process("input2.txt");
//    solver->process("input4.txt");

    ShipmentSolver *solver = new ShipmentSolver();
    solver->process("input3.txt");
 //   solver->process("input2.txt");
  //  solver->solve();
}
