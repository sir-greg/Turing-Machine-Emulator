#ifndef TURINGWINDOW_H
#define TURINGWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPropertyAnimation>
#include <QVector>
#include "alphabetwindow.h"
#include "errorwindow.h"

enum TuringError
{
    None,
    OperationsLimitExceeded,
    OutputWordHasExtraCharacters,
    OutputWordIsNotWhole,
    PointerStoppedAtVoid
};

namespace Ui {
class TuringWindow;
}

class TuringWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TuringWindow(QWidget *parent = nullptr);
    ~TuringWindow();

    bool getEnteredStringCorrectness(const QString& EnteredString) const;

    int getPointer() const;

    int getPointerPreLast() const;

    int getPointerPrePreLast() const;

    void setPointer(int);

    void renewVisibleTuringLine();

    int getStartOfVisiblePart() const;

    void setStartOfVisiblePart(int start_);

    void runTuring();

    void doRule(int mode);

    bool checkTableItemCorrectness(QTableWidgetItem* item);

    bool checkForState(const QString& state);

    bool testRunTuring();

    QRect arrowPos(int currentSymbolIcon);

    void setCurrentRow(int newCurrentRow);

    int getCurrentRow() const;

    int getCurrentCol() const;

    int getPreRow() const;

    int getPreCol() const;

    int getArrowSpeed() const;

    void setCurrentCol(int newCurrentCol);

    void resetStartingTableItem();

    void runTuringFurther();

    void checkTuringCorrectness(TuringError& ErrorType);

    bool getDiscardProgressState() const;

    void setDiscardProgressStateFalse();

    void setDiscardProgressStateTrue();

    void setButtonStateWhileRunning(bool val);

    void setPreRow(int newPreRow);

    void setPreCol(int newPreCol);

    void setPrePrePointer(int newPointerPrePreLast);

    void setPrePointer(int newPointerPreLast);

    bool getTmpStop() const;
    void setTmpStop(bool newTmpStop);

    bool getCurrentlyRunning() const;
    void setCurrentlyRunning(bool newCurrentlyRunning);

    bool getEndedRunning() const;
    void setEndedRunning(bool newEndedRunning);

private slots:

    void on_InputAlphabetButton_clicked();

    void on_InputStringButton_clicked();

    void on_InputStringLine_textChanged(const QString &EnteredString);

    void on_MoreRows_clicked();

    void on_LessRows_clicked();

    void on_playButton_clicked();

    void on_CommandTable_itemChanged(QTableWidgetItem *item);

    void resetTuringLine();

    void setAlphabets();

    void on_turnLineLeftButton_clicked();

    void on_turnLineRightButton_clicked();

    void on_discardProgressButton_clicked();

    void on_stopButton_clicked();

    void on_doOneStepButton_clicked();

signals:

    void discardProgressSignal();

private:

    const QChar Lambda = QChar(0x03BB);
    const QFont boldFont = QFont("Segoe UI", 11, QFont::Bold);
    const QFont defaultFont = QFont("Segoe UI", 11);
    const int VisibleCharactersCount = 15;
    const int LayoutLeftMargin = 50;
    const int LayoutTopMargin = 100;
    const int SymbolIconWidth = 47;
    const int SymbolIconHeight = 59;
    const int ArrowLayoutMargin = 30;
    const int MaximumOperationsCount = 10000;

    Ui::TuringWindow *ui;
    AlphabetWindow *alphabetWindow;

    QString MainAlphabet, ExtraAlphabet, CompleteAlphabet;
    QString oldMainAlphabet, oldExtraAlphabet;
    QString InputString;

    QList<QChar> TuringLine;
    int VisiblePartStart;

    int Pointer;
    int PointerPreLast = -1;
    int PointerPrePreLast = -1;
    int arrowSpeed;

    int CurrentRow, CurrentCol;
    int PreRow = -1, PreCol = -1;

    bool discardProgressState = false;
    bool tmpStop = false;
    bool currentlyRunning = false;
    bool endedRunning = false;
};

#endif // TURINGWINDOW_H
