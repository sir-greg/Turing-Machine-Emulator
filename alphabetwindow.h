#ifndef ALPHABETWINDOW_H
#define ALPHABETWINDOW_H

#include <QWidget>

namespace Ui {
class AlphabetWindow;
}

class AlphabetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AlphabetWindow(QString& MainAlphabet, QString& ExtraAlphabet, QWidget *parent = nullptr);
    ~AlphabetWindow();

    bool getInputCorrectness() const;

    QString getMainAlphabet() const;

    QString getExtraAlphabet() const;

    void setInputCorrectness(bool);

    void setMainAlphabet(const QString);

    void setExtraAlphabet(const QString);

private slots:

    void on_InputMainA_field_textChanged(const QString &);

    void on_InputExtraA_field_textChanged(const QString &);

    void on_MakeAlphabetButton_clicked();

signals:

    void doneInputtingAlphabets();

private:
    Ui::AlphabetWindow *ui;

    QString& MainAlphabet;
    QString& ExtraAlphabet;

    bool InputCorrectness;
};

#endif // ALPHABETWINDOW_H
