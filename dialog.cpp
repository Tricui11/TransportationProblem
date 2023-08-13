#include <dialog.h>
#include <ui_dialog.h>
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <algorithm>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *supplierLabel = new QLabel("Number of Suppliers:", this);
    supplierLabel->setAlignment(Qt::AlignRight);
    QFont supplierFont = supplierLabel->font();
    supplierFont.setPointSize(12);
    supplierFont.setBold(true);
    supplierFont.setItalic(true);
    supplierLabel->setFont(supplierFont);
    layout->addWidget(supplierLabel, 0, 0);

    supplierSpinBox = new QSpinBox(this);
    QColor supplierSpinBoxColor = QColor(Qt::blue);
    supplierSpinBox->setStyleSheet("color: " + supplierSpinBoxColor.name());
    supplierSpinBox->setFont(supplierFont);
    layout->addWidget(supplierSpinBox, 0, 1, 1, 3);

    QLabel *demandLabel = new QLabel("Number of Demands:", this);
    demandLabel->setAlignment(Qt::AlignRight);
    demandLabel->setFont(supplierFont);
    layout->addWidget(demandLabel, 1, 0);

    demandSpinBox = new QSpinBox(this);
    demandSpinBox->setStyleSheet("color: " + supplierSpinBoxColor.name());
    demandSpinBox->setFont(supplierFont);
    layout->addWidget(demandSpinBox, 1, 1, 1, 3);

    QPushButton *generateButton = new QPushButton("Generate Table", this);
    generateButton->setFixedWidth(250);
    SetStyleSheetForQPushButton(generateButton);
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
    SetStyleSheetForQPushButton(steppingStoneMethodButton);
    layout->addWidget(steppingStoneMethodButton, 5, 1, Qt::AlignRight);

    QPushButton *potentialsMethodButton = new QPushButton("Potentials method", this);
    connect(potentialsMethodButton, &QPushButton::clicked, this, &Dialog::onPotentialsMethodButtonClicked);
    potentialsMethodButton->setFixedWidth(250);
    SetStyleSheetForQPushButton(potentialsMethodButton);
    layout->addWidget(potentialsMethodButton, 5, 2, Qt::AlignLeft);

    QLabel *totalStagesLabel = new QLabel("Total stages:", this);
    totalStagesLabel->setAlignment(Qt::AlignRight);
    QFont totalStagesFont = totalStagesLabel->font();
    totalStagesFont.setPointSize(12);
    totalStagesFont.setBold(true);
    totalStagesFont.setItalic(true);
    totalStagesLabel->setFont(totalStagesFont);
    layout->addWidget(totalStagesLabel, 6, 0);

    totalStagesLineEdit = new QLineEdit(this);
    QColor totalStagesColor = QColor(Qt::blue);
    totalStagesLineEdit->setStyleSheet("color: " + totalStagesColor.name());
    totalStagesLineEdit->setFont(totalStagesFont);
    layout->addWidget(totalStagesLineEdit, 6, 1);

    QLabel *stageLabel = new QLabel("Stage:", this);
    stageLabel->setAlignment(Qt::AlignRight);
    QFont stageFont = stageLabel->font();
    stageFont.setPointSize(12);
    stageFont.setBold(true);
    stageFont.setItalic(true);
    stageLabel->setFont(stageFont);
    layout->addWidget(stageLabel, 6, 2);

    stageLineEdit = new QLineEdit(this);
    QColor stageColor = QColor(Qt::blue);
    stageLineEdit->setStyleSheet("color: " + stageColor.name());
    stageLineEdit->setFont(stageFont);
    layout->addWidget(stageLineEdit, 6, 3);

    QLabel *infoLabel = new QLabel("Total costs:", this);
    infoLabel->setAlignment(Qt::AlignRight);
    QFont font = infoLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    infoLabel->setFont(font);
    layout->addWidget(infoLabel, 7, 0);

    infoLineEdit = new QLineEdit(this);
    QColor textColor = QColor(Qt::red);
    infoLineEdit->setStyleSheet("color: " + textColor.name());
    infoLineEdit->setFont(font);
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

    tableWidget->setRowCount(numSuppliers + 1);
    tableWidget->setColumnCount(numDemands + 1);

    for (int row = 0; row <= numSuppliers; ++row)
    {
        for (int col = 0; col <= numDemands; ++col)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (col == 0 && row == 0)
            {
                SetStyleForQTableWidgetItem(item, Qt::black, Qt::black, true, 17);
            }
            else if (row == 0 && col > 0)
            {
                item->setText(QString("Demand %1").arg(col));
                SetStyleForQTableWidgetItem(item, Qt::black, Qt::green, true, 17);
            } else if (col == 0 && row > 0)
            {
                item->setText(QString("Supplier %1").arg(row));
                SetStyleForQTableWidgetItem(item, Qt::black, QColor::fromRgb(255, 51, 255), true, 17);
            } else if (row > 0 && col > 0)
            {
                item->setText(QString("Cost"));
                SetStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
            tableWidget->setItem(row, col, item);
        }
    }

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    infoLineEdit->setText("");
}

void Dialog::onSteppingStoneMethodButtonClicked()
{
    inputDataInSolver();

    bool isMinPriceRule = minPriceRadioButton->isChecked();
    solver->processBySteppingStoneMethod(isMinPriceRule);

    outputDataFromSolver();
}

void Dialog::onPotentialsMethodButtonClicked()
{
    inputDataInSolver();

    bool isMinPriceRule = minPriceRadioButton->isChecked();
    solver->processByPotentialsMethod(isMinPriceRule);

    outputDataFromSolver();
}

void Dialog::inputDataInSolver()
{
    if (solver == nullptr)
    {
        solver = new ShipmentSolver();
    }

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
                SetStyleForQTableWidgetItem(item, Qt::black, Qt::green, true, 17);
            }
            else
            {
                item->setBackground(Qt::yellow);
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
                SetStyleForQTableWidgetItem(item, Qt::black, QColor::fromRgb(255, 51, 255), true, 17);
            }
            else
            {
                item->setBackground(Qt::yellow);
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
}

void Dialog::outputDataFromSolver()
{
    animationTimer->start(1500);
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
                    SetStyleForQTableWidgetItem(item, Qt::red, Qt::yellow, true, 14);
                }
            }
            else
            {
                SetStyleForQTableWidgetItem(item, Qt::black, Qt::yellow, false, 12);
            }
        }
    }
    resCosts.replace(resCosts.size() - 1, 1, "=");
    resCosts += QString::number(totalCosts);
    infoLineEdit->setText(resCosts);
    int historyCapacity = solver->history.capacity();
    currentMatrixIndex = (currentMatrixIndex + 1) % historyCapacity;
    totalStagesLineEdit->setText(QString::number(historyCapacity));
    int stage = currentMatrixIndex != 0 ? currentMatrixIndex : historyCapacity;
    stageLineEdit->setText(QString::number(stage));
}

void Dialog::SetStyleForQTableWidgetItem(QTableWidgetItem *item, QColor foregroundColor, QColor backgroundColor, bool isBold, int fontSize)
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

void Dialog::SetStyleSheetForQPushButton(QPushButton *button)
{
    QString buttonStyle = "background-color: #FF0000; color: #FFFFFF; font-size: 16px; font-weight: bold;";
    button->setStyleSheet(buttonStyle);
}

void Dialog::onPauseButtonClicked()
{
    animationTimer->stop();
}

void Dialog::onPlayButtonClicked()
{
    animationTimer->start(1500);
}
