#include <dialog.h>
#include <ui_dialog.h>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <shipment_solver.cpp>
#include <algorithm>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *supplierLabel = new QLabel("Number of Suppliers:", this);
    layout->addWidget(supplierLabel, 0, 0);

    supplierSpinBox = new QSpinBox(this);
    layout->addWidget(supplierSpinBox, 0, 1);

    QLabel *demandLabel = new QLabel("Number of Demands:", this);
    layout->addWidget(demandLabel, 1, 0);

    demandSpinBox = new QSpinBox(this);
    layout->addWidget(demandSpinBox, 1, 1);

    QPushButton *generateButton = new QPushButton("Generate Table", this);
    layout->addWidget(generateButton, 2, 0, 1, 2);

    tableWidget = new QTableWidget(this);
    layout->addWidget(tableWidget, 3, 0, 1, 2);

    QObject::connect(generateButton, &QPushButton::clicked, this, &Dialog::onGenerateButtonClicked);

    QRadioButton *radioButton1 = new QRadioButton("North-west corner rule", this);
    radioButton1->setChecked(true);
    layout->addWidget(radioButton1, 4, 0);

    QRadioButton *radioButton2 = new QRadioButton("Minimum price rule", this);
    layout->addWidget(radioButton2, 4, 1);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioButton1);
    buttonGroup->addButton(radioButton2);

    QPushButton *steppingStoneMethodButton = new QPushButton("Stepping stone method", this);
    QObject::connect(steppingStoneMethodButton, &QPushButton::clicked, this, &Dialog::onSteppingStoneMethodButtonClicked);
    steppingStoneMethodButton->setFixedWidth(200);
    layout->addWidget(steppingStoneMethodButton, 5, 0, Qt::AlignLeft);

    QPushButton *button4 = new QPushButton("Potentials method", this);
    button4->setFixedWidth(200);
    layout->addWidget(button4, 5, 1, Qt::AlignRight);

    QLabel *infoLabel = new QLabel("Total costs:", this);
    infoLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(infoLabel, 6, 0);

    infoLineEdit = new QLineEdit(this);
    layout->addWidget(infoLineEdit, 6, 1);

    this->setLayout(layout);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onGenerateButtonClicked()
{
    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();

    tableWidget->setRowCount(numSuppliers + 1);
    tableWidget->setColumnCount(numDemands + 1);

    for (int row = 0; row <= numSuppliers; ++row) {
        for (int col = 0; col <= numDemands; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (row == 0 && col > 0) {
                item->setText(QString("Demand %1").arg(col));
            } else if (col == 0 && row > 0) {
                item->setText(QString("Supplier %1").arg(row));
            } else if (row > 0 && col > 0) {
                item->setText(QString("Cost"));
            }
            tableWidget->setItem(row, col, item);
        }
    }

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Dialog::onSteppingStoneMethodButtonClicked()
{
    ShipmentSolver *solver = new ShipmentSolver();

    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();

    vector<double> source, target;
    size_t i;
    for (i = 1; i <= numSuppliers+1; i++)
    {
        QTableWidgetItem *item = tableWidget->item(i, 0);
        if (item)
        {
            QString text = item->text();
            double value = text.toDouble();
            source.push_back(value);
        }
    }
    for (i = 1; i <= numDemands+1; i++)
    {
        QTableWidgetItem *item = tableWidget->item(0, i);
        if (item)
        {
            QString text = item->text();
            double value = text.toDouble();
            target.push_back(value);
        }
    }

    // fix imbalance
    double totalSource = accumulate(source.cbegin(), source.cend(), 0);
    double totalDestination = accumulate(target.cbegin(), target.cend(), 0);
    if (totalSource > totalDestination)
    {
        double diff = totalSource - totalDestination;
        target.push_back(diff);

        demandSpinBox->setValue(numDemands+1);
        tableWidget->setColumnCount(numSuppliers + 2);
        for (int row = 0; row <= numSuppliers; ++row)
        {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            if (row == 0)
            {
                item->setText(QString::number(diff));
            }
            tableWidget->setItem(row, numSuppliers+1, item);
        }
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    else if (totalDestination > totalSource)
    {
        double diff = totalDestination - totalSource;
        source.push_back(diff);

        supplierSpinBox->setValue(numSuppliers+1);
        tableWidget->setRowCount(numDemands + 2);
        for (int column = 0; column <= numDemands; ++column)
        {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            if (column == 0)
            {
                item->setText(QString::number(diff));
            }
            tableWidget->setItem(numDemands+1, column, item);
        }
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    solver->supply = source;
    solver->demand = target;

    solver->costs.clear();
    solver->map.clear();

    for (size_t i = 1; i <= numSuppliers; i++)
    {
        size_t cap = max(static_cast<size_t>(numDemands), solver->demand.size());

        vector<double> dt(cap);
        vector<Shipment> st(cap);

        for (size_t j = 1; j <= numDemands; j++)
        {
            QTableWidgetItem *item = tableWidget->item(i, j);
            if (item)
            {
                QString text = item->text();
                double value = text.toDouble();
                dt[j-1] = value;
            }
        }
        solver->costs.push_back(dt);
        solver->map.push_back(st);
    }
    for (size_t i = numSuppliers; i < solver->supply.size(); i++)
    {
        size_t cap = max(static_cast<size_t>(numDemands), solver->demand.size());

        vector<Shipment> st(cap);
        solver->map.push_back(st);

        vector<double> dt(cap);
        solver->costs.push_back(dt);
    }

    solver->process();

    double totalCosts = 0.0;
    QString resCosts;
    for (int row = 1; row <= numSuppliers; ++row)
    {
        for (int col = 1; col <= numDemands; ++col)
        {
            QTableWidgetItem *item = tableWidget->item(row, col);
            auto cell = solver->map[row-1][col-1];
            if (item)
            {
                item->setText(item->text() + " / " + QString::number(cell.quantity));
            }
            if (cell != Shipment::Empty && cell.row == row-1 && cell.column == col-1)
            {
                totalCosts += cell.quantity * cell.costPerUnit;
                resCosts += QString::number(cell.quantity) + "*" + QString::number(cell.costPerUnit) + "+";
            }
        }
    }

    resCosts.replace(resCosts.size() - 1, 1, "=");
    resCosts += QString::number(totalCosts);
    infoLineEdit->setText(resCosts);

}
