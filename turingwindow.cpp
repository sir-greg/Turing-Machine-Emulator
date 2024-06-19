#include "turingwindow.h"
#include "ui_turingwindow.h"
#include <iostream>

TuringWindow::TuringWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TuringWindow)
{
    ui->setupUi(this);
    alphabetWindow = nullptr;
    MainAlphabet = ExtraAlphabet = CompleteAlphabet = QString();

    resetTuringLine();

    QTableWidgetItem* defaultItem = new QTableWidgetItem();
    defaultItem->setTextAlignment(Qt::AlignCenter);
    ui->CommandTable->setItemPrototype(defaultItem);

    ui->arrow->setGeometry(arrowPos(getPointer()));

    setCurrentRow(0);
    setCurrentCol(0);
/*
    QPixmap playButtonIcon = QPixmap("C:/Qt projects/TuringEmulator_/TuringEmulator/resources/playButton.png");
    ui->playButton->setIcon(playButtonIcon);
    ui->playButton->setIconSize(playButtonIcon.rect().size());
*/

    QObject::connect(ui->discardProgressButton, &QPushButton::clicked,
                     this, &TuringWindow::setDiscardProgressStateTrue);
}

TuringWindow::~TuringWindow()
{
    delete ui;
}

bool TuringWindow::getEnteredStringCorrectness(const QString & EnteredString) const
{
    for (int ind = 0; ind < EnteredString.size(); ++ind)
    {
        if (MainAlphabet.indexOf(EnteredString.at(ind)) == -1)
        {
            return false;
        }
    }
    return true;
}

int TuringWindow::getPointer() const
{
    return Pointer;
}

int TuringWindow::getPointerPreLast() const
{
    return PointerPreLast;
}

int TuringWindow::getPointerPrePreLast() const
{
    return PointerPrePreLast;
}

void TuringWindow::setPointer(int newPointer)
{
    PointerPrePreLast = PointerPreLast;
    PointerPreLast = Pointer;
    Pointer = newPointer;
}

void TuringWindow::renewVisibleTuringLine()
{
    // Clearing the layout

    QLayoutItem* child;
    while ((child = ui->SymbolLayout->takeAt(0)) != nullptr)
    {
        delete child->widget();
        delete child;
    }

    // adding new elements;

    for (int times = 0; times < VisibleCharactersCount; ++times)
    {
        QLabel* SymbolHolder = new QLabel(TuringLine[times + getStartOfVisiblePart()]);
        SymbolHolder->setStyleSheet("QLabel { background-color: white } ");
        SymbolHolder->setAlignment(Qt::AlignCenter);
        ui->SymbolLayout->addWidget(SymbolHolder);
    }
}

int TuringWindow::getStartOfVisiblePart() const
{
    return VisiblePartStart;
}

void TuringWindow::setStartOfVisiblePart(int start_)
{
    VisiblePartStart = start_;
}

void TuringWindow::runTuring()
{
    if (getDiscardProgressState())
    {
        ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: white }");
        ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: white }");
        if (getPreRow() != -1 && getPreCol() != -1)
        {
            ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
            ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
            ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
        }
        setButtonStateWhileRunning(true);
        resetTuringLine();
        resetStartingTableItem();
        setCurrentlyRunning(false);
        setEndedRunning(false);
        ui->arrow->setGeometry(arrowPos(getPointer()));
        return;
    }
    if (getTmpStop())
    {
        ui->speedSlider->setEnabled(true);
        ui->SpeedRegulatorLabel->setEnabled(true);
        ui->doOneStepButton->setEnabled(true);
        return;
    }

    setCurrentlyRunning(true);

    QPropertyAnimation* movingArrow = new QPropertyAnimation();

    if (getPointerPrePreLast() != -1)
        ui->SymbolLayout->itemAt(getPointerPrePreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: white }");

    if (ui->CommandTable->item(getCurrentRow(), getCurrentCol())->text().isEmpty())
        ui->CommandTable->setItem(getCurrentRow(), getCurrentCol(), new QTableWidgetItem);

    ui->CommandTable->item(getCurrentRow(), getCurrentCol())->setFont(boldFont);
    ui->CommandTable->horizontalHeaderItem(getCurrentCol())->setFont(boldFont);
    ui->CommandTable->verticalHeaderItem(getCurrentRow())->setFont(boldFont);

    doRule(1);

    if (getPointerPreLast() != -1)
        ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: yellow }");

    movingArrow = new QPropertyAnimation(ui->arrow, "geometry");
    movingArrow->setDuration(getArrowSpeed());
    movingArrow->setStartValue(arrowPos(getPointerPreLast() - getStartOfVisiblePart()));
    movingArrow->setEndValue(arrowPos(getPointer() - getStartOfVisiblePart()));
    movingArrow->start();

    ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                "QLabel { background-color: blue }");

    QObject::connect(ui->speedSlider, &QSlider::valueChanged,
                     movingArrow, &QPropertyAnimation::setDuration);

    QObject::connect(this, &TuringWindow::discardProgressSignal,
                     movingArrow, [movingArrow](){
        movingArrow->stop();
        emit movingArrow->finished();
    });

    QObject::connect(movingArrow, &QPropertyAnimation::finished,
                     movingArrow, &QPropertyAnimation::deleteLater);

    if (getCurrentlyRunning() && !getDiscardProgressState())
        QObject::connect(movingArrow, &QPropertyAnimation::finished,
                     this, &TuringWindow::runTuringFurther);
    else
        QObject::connect(movingArrow, &QPropertyAnimation::finished,
                         this, [this](){
            ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: white }");
            ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: white }");
            ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
            ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
            ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
            setButtonStateWhileRunning(true);
            setEndedRunning(true);
            if (getDiscardProgressState())
            {
                resetTuringLine();
                resetStartingTableItem();
                ui->arrow->setGeometry(arrowPos(getPointer()));
            }
        });
}

void TuringWindow::runTuringFurther()
{
    ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                "QLabel { background-color: yellow }");
    ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
    ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
    ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
    runTuring();
}

void TuringWindow::checkTuringCorrectness(TuringError &ErrorType)
{
    if (ErrorType != TuringError::None)
        return;

    int ind = 0;
    for (; ind < TuringLine.size() && TuringLine[ind] == Lambda; ++ind);
    for (; ind < TuringLine.size() && TuringLine[ind] != Lambda; ++ind)
    {
        if (MainAlphabet.indexOf(TuringLine[ind]) == -1)
        {
            ErrorType = TuringError::OutputWordHasExtraCharacters;
            break;
        }
    }
    for (; ind < TuringLine.size(); ++ind)
    {
        if (TuringLine[ind] != Lambda)
        {
            ErrorType = TuringError::OutputWordIsNotWhole;
            return;
        }
    }
}

bool TuringWindow::testRunTuring()
{
    int countOperations = 0;
    setCurrentlyRunning(true);
    TuringError ErrorType = TuringError::None;
    while (getCurrentlyRunning()) {
        doRule(0);
        ++countOperations;
        if (countOperations == MaximumOperationsCount)
        {
            ErrorType = TuringError::OperationsLimitExceeded;
            break;
        }
    }
    checkTuringCorrectness(ErrorType);
    resetTuringLine();
    if (ErrorType != TuringError::None)
    {
        QString ErrorString;
        switch (ErrorType)
        {
            case TuringError::OperationsLimitExceeded:
                ErrorString = "ОШИБКА!\n\nДопустимый предел операций (" +  QString::number(MaximumOperationsCount)
                                                             + ") был достигнут";
                break;

            case TuringError::OutputWordHasExtraCharacters:
                ErrorString = "ОШИБКА!\n\nВыходное слово имеет символы, принадлежащие дополнительному алфавиту";
                break;

            case TuringError::OutputWordIsNotWhole:
                ErrorString = "ОШИБКА!\n\nВыходное слово имеет пробелы в виде пустоты";
                break;

            case None:
                break;
        }
        ErrorWindow* errorWindow = new ErrorWindow(ErrorString);
        errorWindow->show();
        QObject::connect(errorWindow, &ErrorWindow::closed,
                         errorWindow, &ErrorWindow::deleteLater);
        return false;
    }
    return true;
}

QRect TuringWindow::arrowPos(int currentSymbolIcon)
{
    if (currentSymbolIcon < 0 || currentSymbolIcon >= VisibleCharactersCount)
    {
        return QRect(3000,
                     3000,
                     ui->arrow->geometry().width(),
                     ui->arrow->geometry().height());
    }
    return QRect(
                LayoutLeftMargin + currentSymbolIcon * SymbolIconWidth,
                LayoutTopMargin + SymbolIconHeight + ArrowLayoutMargin,
                ui->arrow->geometry().width(),
                ui->arrow->geometry().height());
}


void TuringWindow::doRule(int mode)
{
    QString rule = ui->CommandTable->item(getCurrentRow(), getCurrentCol())->text();
    if (!getCurrentlyRunning())
        return;
    if (rule.isEmpty())
    {
        setCurrentlyRunning(false);
        return;
    }
    int commandInd = 0;
    int curPointer = getPointer();

    if (CompleteAlphabet.contains(rule[commandInd]))
    {
        if (curPointer >= TuringLine.size())
            TuringLine.resize(curPointer + 1, Lambda);
        TuringLine[curPointer] = rule[commandInd];
        commandInd += 2;
        if (commandInd >= rule.size())
            goto doneProcessing;

        if (rule[commandInd] == 'L')
        {
            setPointer(--curPointer);
            if (curPointer < getStartOfVisiblePart())
            {
                setStartOfVisiblePart(getStartOfVisiblePart() - VisibleCharactersCount / 3);
                if (getStartOfVisiblePart() < 0)
                {
                    for (int times = 0; times < abs(getStartOfVisiblePart()); ++times)
                    {
                        TuringLine.prepend(Lambda);
                    }
                    setStartOfVisiblePart(0);
                    setPointer(VisibleCharactersCount / 3 - 1);
                }

            }
            commandInd += 2;
        }
        else if (rule[commandInd] == 'R')
        {
            setPointer(++curPointer);
            if (curPointer == getStartOfVisiblePart() + VisibleCharactersCount)
            {
                setStartOfVisiblePart(getStartOfVisiblePart() + VisibleCharactersCount / 3);
                if (getStartOfVisiblePart() + VisibleCharactersCount > TuringLine.size())
                    TuringLine.resize(getStartOfVisiblePart() + VisibleCharactersCount, Lambda);

            }
            commandInd += 2;
        }

        setCurrentCol(CompleteAlphabet.indexOf(TuringLine[getPointer()]));

        if (commandInd >= rule.size())
            goto doneProcessing;

        if (rule[commandInd] == '!')
        {
            setCurrentlyRunning(false);
            if (mode)
            {
                renewVisibleTuringLine();
            }
            return;
        }

        if (rule[commandInd] == 'q')
        {
            bool correctNumber;
            int stateNumber = rule.mid(commandInd + 1).toInt(&correctNumber);
            if (!correctNumber)
            {
                std::cerr << "Reading state failed!";
                exit(0);
            }
            setCurrentRow(stateNumber - 1);
        }
    }
    else
    {
        if (rule[commandInd] == 'L')
        {
            setPointer(--curPointer);
            if (curPointer < getStartOfVisiblePart())
            {
                setStartOfVisiblePart(getStartOfVisiblePart() - VisibleCharactersCount / 3);
                if (getStartOfVisiblePart() < 0)
                {
                    for (int times = 0; times < abs(getStartOfVisiblePart()); ++times)
                    {
                        TuringLine.prepend(Lambda);
                    }
                    setStartOfVisiblePart(0);
                    setPointer(VisibleCharactersCount / 3 - 1);
                }

            }
            commandInd += 2;
        }
        else if (rule[commandInd] == 'R')
        {
            setPointer(++curPointer);
            if (curPointer == getStartOfVisiblePart() + VisibleCharactersCount)
            {
                setStartOfVisiblePart(getStartOfVisiblePart() + VisibleCharactersCount / 3);
                if (getStartOfVisiblePart() + VisibleCharactersCount > TuringLine.size())
                    TuringLine.resize(getStartOfVisiblePart() + VisibleCharactersCount, Lambda);

            }
            commandInd += 2;
        }

        setCurrentCol(CompleteAlphabet.indexOf(TuringLine[getPointer()]));

        if (commandInd >= rule.size())
            goto doneProcessing;

        if (rule[commandInd] == '!')
        {
            setCurrentlyRunning(false);
            if (mode)
            {
                renewVisibleTuringLine();
            }
            return;
        }

        if (rule[commandInd] == 'q')
        {
            bool correctNumber;
            int stateNumber = rule.mid(commandInd + 1).toInt(&correctNumber);
            if (!correctNumber)
            {
                std::cerr << "Reading state failed!";
                exit(0);
            }
            setCurrentRow(stateNumber - 1);
        }

    }
    doneProcessing:;
    if (mode)
    {
        renewVisibleTuringLine();
    }
    setCurrentlyRunning(true);
}

void TuringWindow::on_InputAlphabetButton_clicked()
{
    if (alphabetWindow != nullptr)
        delete alphabetWindow;
    alphabetWindow = new AlphabetWindow(MainAlphabet, ExtraAlphabet);
    alphabetWindow->show();
    QObject::connect(alphabetWindow, &AlphabetWindow::doneInputtingAlphabets,
                     this, &TuringWindow::setAlphabets);

}

void TuringWindow::on_InputStringButton_clicked()
{
    if (getEnteredStringCorrectness(ui->InputStringLine->displayText()))
    {
        resetTuringLine();
        InputString = ui->InputStringLine->displayText();
        setCurrentRow(0);
        setCurrentCol(0);
        setPointer(3);
        ui->arrow->setGeometry(arrowPos(getPointer()));
        if (!InputString.isEmpty())
            setCurrentCol(CompleteAlphabet.indexOf(InputString.front()));
        ui->InputStringLine->setStyleSheet("QLineEdit { background-color: green }");
        int cur_cell = getPointer();
        for (; cur_cell - getPointer() < InputString.size(); ++cur_cell)
        {
            if (cur_cell >= TuringLine.size())
                TuringLine.resize(cur_cell + 1);
            TuringLine[cur_cell] = InputString[cur_cell - getPointer()];
        }
        renewVisibleTuringLine();
    }
}

void TuringWindow::on_InputStringLine_textChanged(const QString & EnteredString)
{
    if (!getEnteredStringCorrectness(EnteredString))
    {
        ui->InputStringLine->setStyleSheet("QLineEdit { background-color: red }");
    }
    else
    {
        ui->InputStringLine->setStyleSheet("QLineEdit { background-color: white }");
    }
}

void TuringWindow::on_MoreRows_clicked()
{
    int LastRow = ui->CommandTable->rowCount();
    ui->CommandTable->insertRow(LastRow);
    ui->CommandTable->setVerticalHeaderItem(LastRow,
                                            new QTableWidgetItem(("q" + std::to_string(LastRow + 1)).c_str()));
}

void TuringWindow::on_LessRows_clicked()
{
    ui->CommandTable->removeRow(ui->CommandTable->rowCount() - 1);
}

void TuringWindow::on_playButton_clicked()
{
    bool checkCorrectness = false;

    for (int row = 0; row < ui->CommandTable->rowCount(); ++row)
    {
        for (int col = 0; col < ui->CommandTable->columnCount(); ++col)
        {
            if (ui->CommandTable->item(row, col) == nullptr)
                continue;
            if (!checkTableItemCorrectness(ui->CommandTable->item(row, col)))
            {
                checkCorrectness = false;
                goto IncorrectItem;
            }
            if (!ui->CommandTable->item(row, col)->text().isEmpty() &&
                    ui->CommandTable->item(row, col)->text().size() && ui->CommandTable->item(row, col)->text().back() == '!')
            {
                checkCorrectness = true;
            }
        }
    }
    IncorrectItem:;
    if (!checkCorrectness)
    {
        ui->playButton->setStyleSheet("QPushButton {background-color: red}");
        return;
    }
    else
    {
        ui->playButton->setStyleSheet("QPushButton {background-color: white}");
    }
    setEndedRunning(false);
    if (getTmpStop())
    {
        if (getCurrentlyRunning())
        {
            setDiscardProgressStateFalse();
            setTmpStop(false);
            setButtonStateWhileRunning(false);
            runTuring();
            setEndedRunning(true);
            setDiscardProgressStateFalse();
        }
        else
        {
            setDiscardProgressStateFalse();
            setButtonStateWhileRunning(false);
            resetTuringLine();
            resetStartingTableItem();
            runTuring();
            setEndedRunning(true);
            setDiscardProgressStateFalse();
        }
    }
    else
    {
        resetTuringLine();
        resetStartingTableItem();
        setDiscardProgressStateFalse();
        if (testRunTuring())
        {
            setButtonStateWhileRunning(false);
            resetTuringLine();
            resetStartingTableItem();
            runTuring();
            setEndedRunning(true);
            setDiscardProgressStateFalse();
        }
    }
}

void TuringWindow::on_CommandTable_itemChanged(QTableWidgetItem *item)
{
    ui->playButton->setStyleSheet("QPushButton {background-color: white}");
    if (!checkTableItemCorrectness(item))
        item->setBackground(Qt::red);
    else
        item->setBackground(Qt::white);
}

bool TuringWindow::checkTableItemCorrectness(QTableWidgetItem *item)
{
    QString newRule = item->text();
    int curPos = 0;
    if (!newRule.size())
    {
        return true;
    }
    else
    {
        if (!CompleteAlphabet.contains(newRule[curPos]))
        {
            if (checkForState(newRule))
                return true;

            if (newRule[curPos] != 'L' && newRule[curPos] != 'R')
                return false;

            ++curPos;
            if (curPos == newRule.size())
                return true;

            if (newRule[curPos] != ',')
                return false;

            ++curPos;
            if (curPos == newRule.size())
                return false;

            return checkForState(newRule.mid(curPos));
        }
        else
        {
            ++curPos;
            if (curPos >= newRule.size() || newRule[curPos] != ',')
                return false;

            ++curPos;
            if (curPos >= newRule.size())
                return false;

            if (newRule[curPos] == 'L' || newRule[curPos] == 'R')
            {
                if (curPos + 1 == newRule.size())
                {
                    return true;
                }
                else
                {
                    ++curPos;

                    if (newRule[curPos] != ',')
                        return false;
                    ++curPos;

                    if (curPos >= newRule.size())
                        return false;
                }
            }

            return checkForState(newRule.mid(curPos));
        }
    }
}

bool TuringWindow::checkForState(const QString &state)
{
    int curPos = 0;
    if (curPos >= state.size())
        return true;

    if (state[curPos] == 'q')
    {
        bool converted;
        int NumberOfState = state.mid(++curPos).toInt(&converted, 10);
        if (!converted || NumberOfState > ui->CommandTable->rowCount())
            return false;
        return true;
    }
    if (state[curPos] == '!' && curPos + 1 == state.size())
    {
        return true;
    }
    return false;
}

void TuringWindow::setAlphabets()
{
    ui->CommandTable->setEnabled(true);
    ui->MoreRows->setEnabled(true);
    ui->LessRows->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    ui->SpeedRegulatorLabel->setEnabled(true);
    ui->InputStringLabel->setEnabled(true);
    ui->InputStringButton->setEnabled(true);
    ui->InputStringLine->setEnabled(true);
    if (getEnteredStringCorrectness(ui->InputStringLine->displayText()))
        ui->InputStringLine->setStyleSheet("QLineEdit { background-color: white }");
    else
        ui->InputStringLine->setStyleSheet("QLineEdit { background-color: red }");

    if (oldMainAlphabet != MainAlphabet || oldExtraAlphabet != ExtraAlphabet)
    {
        if (std::all_of(oldMainAlphabet.begin(), oldMainAlphabet.end(), [this](QChar a)
        {
            return MainAlphabet.contains(a);
        }) &&
            std::all_of(oldExtraAlphabet.begin(), oldExtraAlphabet.end(), [this](QChar a)
        {
            return ExtraAlphabet.contains(a);
        }))
        {
           QTableWidget* newCommandTable = new QTableWidget();
           QString oldCompleteAlphabet = oldMainAlphabet + Lambda + oldExtraAlphabet;
           CompleteAlphabet = MainAlphabet + Lambda + ExtraAlphabet;
           newCommandTable->setColumnCount(CompleteAlphabet.size());
           newCommandTable->setRowCount(ui->CommandTable->rowCount());
           for (int row = 0; row < newCommandTable->rowCount(); ++row)
           {
               newCommandTable->setVerticalHeaderItem(row, new QTableWidgetItem("q" + QString::number(row + 1)));
           }
           for (int col = 0; col < CompleteAlphabet.size(); ++col)
           {
               newCommandTable->setHorizontalHeaderItem(col, new QTableWidgetItem(CompleteAlphabet[col]));
           }
           for (int col = 0; col < CompleteAlphabet.size(); ++col)
           {
               int oldCol = oldCompleteAlphabet.indexOf(CompleteAlphabet[col]);
               if (oldCol != -1)
               {
                   for (int row = 0; row < newCommandTable->rowCount(); ++row)
                   {
                       if (ui->CommandTable->item(row, oldCol))
                            newCommandTable->setItem(row, col, new QTableWidgetItem(*ui->CommandTable->item(row, oldCol)));
                   }

               }
           }
           ui->CommandTable->clearContents();
           ui->CommandTable->setRowCount(newCommandTable->rowCount());
           ui->CommandTable->setColumnCount(newCommandTable->columnCount());
           for (int col = 0; col < CompleteAlphabet.size(); ++col)
           {
               ui->CommandTable->setHorizontalHeaderItem(col, new QTableWidgetItem(CompleteAlphabet[col]));
           }
           for (int row = 0; row < ui->CommandTable->rowCount(); ++row)
           {
               ui->CommandTable->setVerticalHeaderItem(row, new QTableWidgetItem("q" + QString::number(row + 1)));
               for (int col = 0; col < ui->CommandTable->columnCount(); ++col)
               {
                   if (newCommandTable->item(row, col))
                       ui->CommandTable->setItem(row, col, new QTableWidgetItem(*newCommandTable->item(row, col)));
               }
           }
           delete newCommandTable;
           oldMainAlphabet = MainAlphabet;
           oldExtraAlphabet = ExtraAlphabet;
        }
        else
        {
            ui->CommandTable->clear();
            int cur_col = 0;
            for (; cur_col < MainAlphabet.size(); ++cur_col)
            {
                ui->CommandTable->setHorizontalHeaderItem(cur_col,
                                                          new QTableWidgetItem(MainAlphabet[cur_col]));
            }
            ui->CommandTable->setHorizontalHeaderItem(cur_col,
                                                      new QTableWidgetItem(Lambda));
            ++cur_col;
            for (; cur_col - MainAlphabet.size() - 1 < ExtraAlphabet.size(); ++cur_col)
            {
                ui->CommandTable->setHorizontalHeaderItem(cur_col,
                                                          new QTableWidgetItem(ExtraAlphabet[cur_col - MainAlphabet.size() - 1]));
            }
            for (int row = 0; row < ui->CommandTable->rowCount(); ++row) {
                ui->CommandTable->setVerticalHeaderItem(row, new QTableWidgetItem("q" + QString::number(row + 1)));
            }
            oldMainAlphabet = MainAlphabet;
            oldExtraAlphabet = ExtraAlphabet;
            CompleteAlphabet = MainAlphabet + Lambda + ExtraAlphabet;
        }
    }
}

int TuringWindow::getCurrentCol() const
{
    return CurrentCol;
}

int TuringWindow::getPreRow() const
{
    return PreRow;
}

int TuringWindow::getPreCol() const
{
    return PreCol;
}

int TuringWindow::getArrowSpeed() const
{
    return (ui->speedSlider->maximum() - ui->speedSlider->value()) / 5;
}

void TuringWindow::setCurrentCol(int newCurrentCol)
{
    PreCol = CurrentCol;
    CurrentCol = newCurrentCol;
}

void TuringWindow::resetStartingTableItem()
{
    setPreRow(-1);
    setPreCol(-1);
    setCurrentRow(0);
    setCurrentCol(CompleteAlphabet.indexOf(TuringLine[getPointer()]));
}

int TuringWindow::getCurrentRow() const
{
    return CurrentRow;
}

void TuringWindow::setCurrentRow(int newCurrentRow)
{
    PreRow = CurrentRow;
    CurrentRow = newCurrentRow;
}

void TuringWindow::resetTuringLine()
{
    TuringLine.resize(VisibleCharactersCount, Lambda);
    setPrePrePointer(-1);
    setPrePointer(-1);
    setPointer(3);
    setStartOfVisiblePart(0);
    ui->arrow->setGeometry(arrowPos(getPointer() - getStartOfVisiblePart()));
    int cur_cell = getPointer();
    for (; cur_cell - getPointer() < InputString.size(); ++cur_cell)
    {
        if (cur_cell >= TuringLine.size())
            TuringLine.resize(cur_cell + 1);
        TuringLine[cur_cell] = InputString[cur_cell - getPointer()];
    }
    renewVisibleTuringLine();

}


void TuringWindow::on_turnLineLeftButton_clicked()
{
    if (getStartOfVisiblePart() == 0) {
        setPointer(getPointer() + 1);
        TuringLine.prepend(Lambda);
    }
    else
    {
        setStartOfVisiblePart(getStartOfVisiblePart() - 1);
    }
    ui->arrow->setGeometry(arrowPos(getPointer() - getStartOfVisiblePart()));
    renewVisibleTuringLine();
}


void TuringWindow::on_turnLineRightButton_clicked()
{
    if (getStartOfVisiblePart() + VisibleCharactersCount == TuringLine.size())
    {
        TuringLine.append(Lambda);
    }
    setStartOfVisiblePart(getStartOfVisiblePart() + 1);
    ui->arrow->setGeometry(arrowPos(getPointer() - getStartOfVisiblePart()));
    renewVisibleTuringLine();
}

void TuringWindow::on_discardProgressButton_clicked()
{
    if (!getCurrentlyRunning() || getTmpStop())
    {
        ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: white }");
        ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: white }");
        if (getPreRow() != -1 && getPreCol() != -1)
        {
            ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
            ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
            ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
        }
        setButtonStateWhileRunning(true);
        resetTuringLine();
        resetStartingTableItem();
        ui->arrow->setGeometry(arrowPos(getPointer()));
    }
    setDiscardProgressStateTrue();
    setTmpStop(false);
    setEndedRunning(false);
    setCurrentlyRunning(false);
    emit discardProgressSignal();
}

bool TuringWindow::getTmpStop() const
{
    return tmpStop;
}

void TuringWindow::setTmpStop(bool newTmpStop)
{
    tmpStop = newTmpStop;
}

void TuringWindow::setPrePrePointer(int newPrePrePointer)
{
    PointerPreLast = newPrePrePointer;
}

void TuringWindow::setPrePointer(int newPrePointer)
{
    PointerPrePreLast = newPrePointer;
}

void TuringWindow::setPreCol(int newPreCol)
{
    PreCol = newPreCol;
}

void TuringWindow::setPreRow(int newPreRow)
{
    PreRow = newPreRow;
}

bool TuringWindow::getDiscardProgressState() const
{
    return discardProgressState;
}

void TuringWindow::setDiscardProgressStateFalse()
{
    discardProgressState = false;
}

void TuringWindow::setDiscardProgressStateTrue()
{
    discardProgressState = true;
}

void TuringWindow::setButtonStateWhileRunning(bool state)
{
    ui->InputAlphabetButton->setEnabled(state);
    ui->InputStringLine->setEnabled(state);
    ui->InputStringButton->setEnabled(state);
    ui->InputStringLabel->setEnabled(state);
    ui->CommandTable->setEnabled(state);
    ui->MoreRows->setEnabled(state);
    ui->LessRows->setEnabled(state);
    ui->turnLineLeftButton->setEnabled(state);
    ui->turnLineRightButton->setEnabled(state);
    ui->SpeedRegulatorLabel->setEnabled(state);
    ui->speedSlider->setEnabled(state);
    ui->doOneStepButton->setEnabled(state);
}

void TuringWindow::on_stopButton_clicked()
{
    if (getCurrentlyRunning())
        setTmpStop(true);
}

bool TuringWindow::getCurrentlyRunning() const
{
    return currentlyRunning;
}

void TuringWindow::setCurrentlyRunning(bool newCurrentlyRunning)
{
    currentlyRunning = newCurrentlyRunning;
}

void TuringWindow::on_doOneStepButton_clicked()
{
    if (getCurrentlyRunning())
    {

        ui->doOneStepButton->setEnabled(false);
        ui->playButton->setEnabled(false);

        QPropertyAnimation* movingArrow = new QPropertyAnimation();

        if (getPointerPrePreLast() != -1)
            ui->SymbolLayout->itemAt(getPointerPrePreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: white }");

        if (ui->CommandTable->item(getCurrentRow(), getCurrentCol())->text().isEmpty())
            ui->CommandTable->setItem(getCurrentRow(), getCurrentCol(), new QTableWidgetItem);

        ui->CommandTable->item(getCurrentRow(), getCurrentCol())->setFont(boldFont);
        ui->CommandTable->horizontalHeaderItem(getCurrentCol())->setFont(boldFont);
        ui->CommandTable->verticalHeaderItem(getCurrentRow())->setFont(boldFont);

        doRule(1);

        if (getPointerPreLast() != -1)
            ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: yellow }");

        movingArrow = new QPropertyAnimation(ui->arrow, "geometry");
        movingArrow->setDuration(getArrowSpeed());
        movingArrow->setStartValue(arrowPos(getPointerPreLast() - getStartOfVisiblePart()));
        movingArrow->setEndValue(arrowPos(getPointer() - getStartOfVisiblePart()));
        movingArrow->start();

        ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                    "QLabel { background-color: blue }");

        QObject::connect(ui->speedSlider, &QSlider::valueChanged,
                         movingArrow, &QPropertyAnimation::setDuration);

        QObject::connect(this, &TuringWindow::discardProgressSignal,
                         movingArrow, [movingArrow](){
            movingArrow->stop();
            emit movingArrow->finished();
        });

        QObject::connect(movingArrow, &QPropertyAnimation::finished,
                         movingArrow, &QPropertyAnimation::deleteLater);

        QObject::connect(movingArrow, &QPropertyAnimation::finished,
                         this, [this]()
        {
            ui->doOneStepButton->setEnabled(true);
            ui->playButton->setEnabled(true);

            ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: white }");
            ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: white }");
            ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
            ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
            ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
        });
        if (!getCurrentlyRunning() || getDiscardProgressState())
        {
            QObject::connect(movingArrow, &QPropertyAnimation::finished,
                             this, [this]()
            {
                setButtonStateWhileRunning(true);
                setEndedRunning(true);
                setTmpStop(false);
                if (getDiscardProgressState())
                {
                    resetTuringLine();
                    resetStartingTableItem();
                    ui->arrow->setGeometry(arrowPos(getPointer()));
                }
            });
        }
    }
    else
    {
        resetTuringLine();
        resetStartingTableItem();
        setDiscardProgressStateFalse();
        if (testRunTuring())
        {
            setButtonStateWhileRunning(false);
            setCurrentlyRunning(true);
            setButtonStateWhileRunning(false);
            resetTuringLine();
            resetStartingTableItem();

            ui->doOneStepButton->setEnabled(false);
            ui->playButton->setEnabled(false);

            QPropertyAnimation* movingArrow = new QPropertyAnimation();

            if (getPointerPrePreLast() != -1)
                ui->SymbolLayout->itemAt(getPointerPrePreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                            "QLabel { background-color: white }");

            if (ui->CommandTable->item(getCurrentRow(), getCurrentCol())->text().isEmpty())
                ui->CommandTable->setItem(getCurrentRow(), getCurrentCol(), new QTableWidgetItem);

            ui->CommandTable->item(getCurrentRow(), getCurrentCol())->setFont(boldFont);
            ui->CommandTable->horizontalHeaderItem(getCurrentCol())->setFont(boldFont);
            ui->CommandTable->verticalHeaderItem(getCurrentRow())->setFont(boldFont);

            doRule(1);

            if (getPointerPreLast() != -1)
                ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                            "QLabel { background-color: yellow }");

            movingArrow = new QPropertyAnimation(ui->arrow, "geometry");
            movingArrow->setDuration(getArrowSpeed());
            movingArrow->setStartValue(arrowPos(getPointerPreLast() - getStartOfVisiblePart()));
            movingArrow->setEndValue(arrowPos(getPointer() - getStartOfVisiblePart()));
            movingArrow->start();

            ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                        "QLabel { background-color: blue }");

            QObject::connect(ui->speedSlider, &QSlider::valueChanged,
                             movingArrow, &QPropertyAnimation::setDuration);

            QObject::connect(this, &TuringWindow::discardProgressSignal,
                             movingArrow, [movingArrow](){
                movingArrow->stop();
                emit movingArrow->finished();
            });

            QObject::connect(movingArrow, &QPropertyAnimation::finished,
                             movingArrow, &QPropertyAnimation::deleteLater);

            QObject::connect(movingArrow, &QPropertyAnimation::finished,
                             this, [this]()
            {
                ui->doOneStepButton->setEnabled(true);
                ui->playButton->setEnabled(true);

                ui->SymbolLayout->itemAt(getPointerPreLast() - getStartOfVisiblePart())->widget()->setStyleSheet(
                            "QLabel { background-color: white }");
                ui->SymbolLayout->itemAt(getPointer() - getStartOfVisiblePart())->widget()->setStyleSheet(
                            "QLabel { background-color: white }");
                ui->CommandTable->item(getPreRow(), getPreCol())->setFont(defaultFont);
                ui->CommandTable->horizontalHeaderItem(getPreCol())->setFont(defaultFont);
                ui->CommandTable->verticalHeaderItem(getPreRow())->setFont(defaultFont);
            });
            if (!getCurrentlyRunning() || getDiscardProgressState())
            {
                QObject::connect(movingArrow, &QPropertyAnimation::finished,
                                 this, [this]()
                {
                    setButtonStateWhileRunning(true);
                    setEndedRunning(true);
                    if (getDiscardProgressState())
                    {
                        resetTuringLine();
                        resetStartingTableItem();
                        ui->arrow->setGeometry(arrowPos(getPointer()));
                    }
                });
            }
            setTmpStop(true);
        }
    }
}

bool TuringWindow::getEndedRunning() const
{
    return endedRunning;
}

void TuringWindow::setEndedRunning(bool newEndedRunning)
{
    endedRunning = newEndedRunning;
}

