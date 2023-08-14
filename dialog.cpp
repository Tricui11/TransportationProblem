#include <dialog.h>
#include <ui_dialog.h>
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <algorithm>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *supplierLabel = new QLabel("Number of Suppliers:", this);
    QFont labelFont = setStyleSheetForQLabel(supplierLabel, 12, true);
    layout->addWidget(supplierLabel, 0, 0);

    supplierSpinBox = new QSpinBox(this);
    QColor supplierSpinBoxColor = QColor(Qt::blue);
    supplierSpinBox->setStyleSheet("color: " + supplierSpinBoxColor.name());
    supplierSpinBox->setFont(labelFont);
    layout->addWidget(supplierSpinBox, 0, 1, 1, 3);

    QLabel *demandLabel = new QLabel("Number of Demands:", this);
    demandLabel->setAlignment(Qt::AlignRight);
    demandLabel->setFont(labelFont);
    layout->addWidget(demandLabel, 1, 0);

    demandSpinBox = new QSpinBox(this);
    demandSpinBox->setStyleSheet("color: " + supplierSpinBoxColor.name());
    demandSpinBox->setFont(labelFont);
    layout->addWidget(demandSpinBox, 1, 1, 1, 3);

    QPushButton *generateButton = new QPushButton("Generate Table", this);
    generateButton->setFixedWidth(250);
    setStyleSheetForQPushButton(generateButton);
    connect(generateButton, &QPushButton::clicked, this, &Dialog::onGenerateButtonClicked);
    layout->addWidget(generateButton, 2, 0, 1, 4, Qt::AlignCenter);

    tableWidget = new QTableWidget(this);
    layout->addWidget(tableWidget, 3, 0, 1, 4);

    QRadioButton *nwRadioButton = new QRadioButton("North-west corner rule", this);
    nwRadioButton->setChecked(true);
    layout->addWidget(nwRadioButton, 4, 0);

    minPriceRadioButton = new QRadioButton("Minimum price rule", this);
    layout->addWidget(minPriceRadioButton, 4, 1);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(nwRadioButton);
    buttonGroup->addButton(minPriceRadioButton);

    QPushButton *steppingStoneMethodButton = new QPushButton("Stepping stone method", this);
    connect(steppingStoneMethodButton, &QPushButton::clicked, this, &Dialog::onSteppingStoneMethodButtonClicked);
    steppingStoneMethodButton->setFixedWidth(250);
    setStyleSheetForQPushButton(steppingStoneMethodButton);
    layout->addWidget(steppingStoneMethodButton, 5, 1, Qt::AlignRight);

    QPushButton *potentialsMethodButton = new QPushButton("Potentials method", this);
    connect(potentialsMethodButton, &QPushButton::clicked, this, &Dialog::onPotentialsMethodButtonClicked);
    potentialsMethodButton->setFixedWidth(250);
    setStyleSheetForQPushButton(potentialsMethodButton);
    layout->addWidget(potentialsMethodButton, 5, 2, Qt::AlignLeft);

    QLabel *totalStagesLabel = new QLabel("Total stages:", this);
    totalStagesLabel->setAlignment(Qt::AlignRight);
    totalStagesLabel->setFont(labelFont);
    layout->addWidget(totalStagesLabel, 6, 0);

    totalStagesLineEdit = new QLineEdit(this);
    QColor totalStagesColor = QColor(Qt::blue);
    totalStagesLineEdit->setStyleSheet("color: " + totalStagesColor.name());
    totalStagesLineEdit->setFont(labelFont);
    totalStagesLineEdit->setReadOnly(true);
    layout->addWidget(totalStagesLineEdit, 6, 1);

    QLabel *stageLabel = new QLabel("Stage:", this);
    stageLabel->setAlignment(Qt::AlignRight);
    stageLabel->setFont(labelFont);
    layout->addWidget(stageLabel, 6, 2);

    stageLineEdit = new QLineEdit(this);
    QColor stageColor = QColor(Qt::blue);
    stageLineEdit->setStyleSheet("color: " + stageColor.name());
    stageLineEdit->setFont(labelFont);
    stageLineEdit->setReadOnly(true);
    layout->addWidget(stageLineEdit, 6, 3);

    QLabel *infoLabel = new QLabel("Total costs:", this);
    setStyleSheetForQLabel(infoLabel, 14, false);
    layout->addWidget(infoLabel, 7, 0);

    infoLineEdit = new QLineEdit(this);
    QColor textColor = QColor(Qt::red);
    infoLineEdit->setStyleSheet("color: " + textColor.name());
    infoLineEdit->setFont(labelFont);
    infoLineEdit->setReadOnly(true);
    layout->addWidget(infoLineEdit, 7, 1, 1, 3);

    QPushButton *pauseButton = new QPushButton("Pause", this);
    pauseButton->setFixedWidth(50);
    layout->addWidget(pauseButton, 8, 1, Qt::AlignRight);
    QObject::connect(pauseButton, &QPushButton::clicked, this, &Dialog::onPauseButtonClicked);

    QPushButton *playButton = new QPushButton("Play", this);
    playButton->setFixedWidth(50);
    layout->addWidget(playButton, 8, 2, Qt::AlignLeft);
    QObject::connect(playButton, &QPushButton::clicked, this, &Dialog::onPlayButtonClicked);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Dialog::animateMatrices);

    this->setLayout(layout);
}

Dialog::~Dialog()
{
    if (solver != nullptr)
    {
        delete solver;
    }
    delete ui;
}

void Dialog::onGenerateButtonClicked()
{
    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();
    if (numSuppliers < 1 || numDemands < 1)
    {
        QMessageBox::critical(this, "Error", "Invalid table size input!");
        return;
    }

    tableWidget->setRowCount(numSuppliers + 1);
    tableWidget->setColumnCount(numDemands + 1);

    for (int row = 0; row <= numSuppliers; ++row)
    {
        for (int col = 0; col <= numDemands; ++col)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (col == 0 && row == 0)
            {
                setStyleForQTableWidgetItem(item, Qt::black, Qt::black, true, 17);
            }
            else if (row == 0 && col > 0)
            {
                item->setText(QString("Demand %1").arg(col));
                setStyleForQTableWidgetItem(item, Qt::black, Qt::green, true, 17);
            } else if (col == 0 && row > 0)
            {
                item->setText(QString("Supplier %1").arg(row));
                setStyleForQTableWidgetItem(item, Qt::black, QColor::fromRgb(255, 51, 255), true, 17);
            } else if (row > 0 && col > 0)
            {
                item->setText(QString("Cost"));
                setStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
            tableWidget->setItem(row, col, item);
        }
    }
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    infoLineEdit->setText("");
    totalStagesLineEdit->setText("");
    stageLineEdit->setText("");

    animationTimer->stop();

    if (solver != nullptr)
    {
        delete solver;
    }
}

void Dialog::onSteppingStoneMethodButtonClicked()
{
    bool isSuccessInput = inputDataInSolver();
    if (!isSuccessInput)
    {
        return;
    }

    bool isMinPriceRule = minPriceRadioButton->isChecked();
    solver->processBySteppingStoneMethod(isMinPriceRule);

    outputDataFromSolver();
}

void Dialog::onPotentialsMethodButtonClicked()
{
    bool isSuccessInput = inputDataInSolver();
    if (!isSuccessInput)
    {
        return;
    }

    bool isMinPriceRule = minPriceRadioButton->isChecked();
    bool isSuccess = solver->processByPotentialsMethod(isMinPriceRule);

    if(!isSuccess)
    {
        QMessageBox::critical(this, "Error", "Can not find solution by potentials method. Please try another rule or method!");
        return;
    }

    outputDataFromSolver();
}

bool Dialog::inputDataInSolver()
{
    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();

    if (solver == nullptr)
    {
        solver = new ShipmentSolver();
    }
    else
    {
        currentMatrixIndex = 0;
        for (int row = 1; row <= numSuppliers; ++row)
        {
            QTableWidgetItem *item = tableWidget->item(row, 0);
            double value = item->text().toDouble();
            solver->supply[row-1] = value;
        }
        for (int col = 1; col <= numDemands; ++col)
        {
            QTableWidgetItem *item = tableWidget->item(0, col);
            double value = item->text().toDouble();
            solver->demand[col-1] = value;
        }
        solver->history.clear();
        solver->map.clear();
        for (int i = 1; i <= numSuppliers; i++)
        {
            size_t cap = max(static_cast<size_t>(numDemands), solver->demand.size());
            vector<Shipment> st(cap);
            solver->map.push_back(st);
        }
        for (size_t i = numSuppliers; i < solver->supply.size(); i++)
        {
            size_t cap = max(static_cast<size_t>(numDemands), solver->demand.size());
            vector<Shipment> st(cap);
            solver->map.push_back(st);
        }
        return true;
    }

    vector<double> source, target;
    int i;
    bool conversionOk = false;
    for (i = 1; i <= numSuppliers+1; i++)
    {
        QTableWidgetItem *item = tableWidget->item(i, 0);
        if (item)
        {
            QString text = item->text();
            double value = text.toDouble(&conversionOk);
            if (!conversionOk)
            {
                QMessageBox::critical(this, "Error", "Invalid input: " + text);
                return false;
            }
            source.push_back(value);
        }
    }
    for (i = 1; i <= numDemands+1; i++)
    {
        QTableWidgetItem *item = tableWidget->item(0, i);
        if (item)
        {
            QString text = item->text();
            double value = text.toDouble(&conversionOk);
            if (!conversionOk)
            {
                QMessageBox::critical(this, "Error", "Invalid input: " + text);
                return false;
            }
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
        tableWidget->setColumnCount(numDemands + 2);
        for (int row = 0; row <= numSuppliers; ++row)
        {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            if (row == 0)
            {
                item->setText(QString::number(diff));
                setStyleForQTableWidgetItem(item, Qt::black, Qt::green, true, 17);
            }
            else
            {
                setStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
            tableWidget->setItem(row, numDemands+1, item);
        }
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    else if (totalDestination > totalSource)
    {
        double diff = totalDestination - totalSource;
        source.push_back(diff);

        supplierSpinBox->setValue(numSuppliers+1);
        tableWidget->setRowCount(numSuppliers + 2);
        for (int column = 0; column <= numDemands; ++column)
        {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            if (column == 0)
            {
                item->setText(QString::number(diff));
                setStyleForQTableWidgetItem(item, Qt::black, QColor::fromRgb(255, 51, 255), true, 17);
            }
            else
            {
                setStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
            tableWidget->setItem(numSuppliers + 1, column, item);
        }
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    solver->supply = source;
    solver->demand = target;

    solver->costs.clear();
    solver->map.clear();

    for (int i = 1; i <= numSuppliers; i++)
    {
        size_t cap = max(static_cast<size_t>(numDemands), solver->demand.size());

        vector<double> dt(cap);
        vector<Shipment> st(cap);

        for (int j = 1; j <= numDemands; j++)
        {
            QTableWidgetItem *item = tableWidget->item(i, j);
            if (item)
            {
                QString text = item->text();
                double value = text.toDouble(&conversionOk);
                if (!conversionOk)
                {
                    QMessageBox::critical(this, "Error", "Invalid input: " + text);
                    return false;
                }
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

    return true;
}

void Dialog::outputDataFromSolver()
{
    animationTimer->start(1500);
    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();
    for (int row = 0; row <= numSuppliers; ++row)
    {
        for (int col = 0; col <= numDemands; ++col)
        {

            QTableWidgetItem *item = tableWidget->item(row, col);
            if (item)
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
}

void Dialog::animateMatrices()
{
    const vector<vector<Shipment>> &matrix = solver->history[currentMatrixIndex];
    int numSuppliers = supplierSpinBox->value();
    int numDemands = demandSpinBox->value();
    double totalCosts = 0.0;
    QString resCosts;
    for (int row = 1; row <= numSuppliers; ++row)
    {
        for (int col = 1; col <= numDemands; ++col)
        {
            QTableWidgetItem *item = tableWidget->item(row, col);
            auto cell = matrix[row-1][col-1];
            if (item)
            {
                item->setText(QString::number(solver->costs[row-1][col-1]) + " / " + QString::number(cell.quantity));
            }
            if (cell != Shipment::Empty && cell.row == row-1 && cell.column == col-1)
            {
                if (!std::isnan(cell.quantity))
                {
                    totalCosts += cell.quantity * cell.costPerUnit;
                    resCosts += QString::number(cell.quantity) + "*" + QString::number(cell.costPerUnit) + "+";
                    setStyleForQTableWidgetItem(item, Qt::red, Qt::yellow, true, 14);
                }
            }
            else
            {
                setStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
        }
    }
    resCosts.replace(resCosts.size() - 1, 1, "=");
    resCosts += QString::number(totalCosts);
    infoLineEdit->setText(resCosts);
    int historySize = solver->history.size();
    currentMatrixIndex = (currentMatrixIndex + 1) % historySize;
    totalStagesLineEdit->setText(QString::number(historySize));
    int stage = currentMatrixIndex != 0 ? currentMatrixIndex : historySize;
    stageLineEdit->setText(QString::number(stage));
}

void Dialog::setStyleForQTableWidgetItem(QTableWidgetItem *item, QColor foregroundColor, QColor backgroundColor, bool isBold, int fontSize)
{
    QColor textColor = QColor(foregroundColor);
    QBrush textBrush(textColor);
    item->setForeground(textBrush);
    QColor qBackgroundColor = QColor(backgroundColor);
    QBrush backgroundBrush(qBackgroundColor);
    item->setBackground(backgroundBrush);
    QFont font = item->font();
    font.setPointSize(fontSize);
    font.setBold(isBold);
    item->setFont(font);
    item->setTextAlignment(Qt::AlignCenter);
}

void Dialog::setStyleSheetForQPushButton(QPushButton *button)
{
    QString buttonStyle = "background-color: #FF0000; color: #FFFFFF; font-size: 16px; font-weight: bold;";
    button->setStyleSheet(buttonStyle);
}

QFont Dialog::setStyleSheetForQLabel(QLabel *label, int fontSize, bool isItalic)
{
    label->setAlignment(Qt::AlignRight);
    QFont font = label->font();
    font.setPointSize(fontSize);
    font.setBold(true);
    font.setItalic(isItalic);
    label->setFont(font);
    return font;
}

void Dialog::onPauseButtonClicked()
{
    animationTimer->stop();
}

void Dialog::onPlayButtonClicked()
{
    animationTimer->start(1500);
}
