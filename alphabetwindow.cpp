#include "alphabetwindow.h"
#include "ui_alphabetwindow.h"

AlphabetWindow::AlphabetWindow(QString& MainAlphabet, QString& ExtraAlphabet, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlphabetWindow),
    MainAlphabet(MainAlphabet),
    ExtraAlphabet(ExtraAlphabet)
{
    ui->setupUi(this);
    setInputCorrectness(true);
    if (ExtraAlphabet.isEmpty())
        ui->InputExtraA_field->setStyleSheet("QLineEdit { background-color: #d7d6d5 }");
    ui->InputMainA_field->setText(MainAlphabet);
    ui->InputExtraA_field->setText(ExtraAlphabet);
}

AlphabetWindow::~AlphabetWindow()
{
    delete ui;
}

bool AlphabetWindow::getInputCorrectness() const
{
    return InputCorrectness;
}

QString AlphabetWindow::getMainAlphabet() const
{
    return MainAlphabet;
}

QString AlphabetWindow::getExtraAlphabet() const
{
    return ExtraAlphabet;
}

void AlphabetWindow::setInputCorrectness(bool InputCorrectness_)
{
    InputCorrectness = InputCorrectness_;
}

void AlphabetWindow::setMainAlphabet(const QString newMainAlphabet)
{
    MainAlphabet = newMainAlphabet;
}

void AlphabetWindow::setExtraAlphabet(const QString newExtraAlphabet)
{
    ExtraAlphabet = newExtraAlphabet;
}



void AlphabetWindow::on_InputMainA_field_textChanged(const QString &newMainAlphabet)
{
    if (newMainAlphabet.isEmpty())
    {
        setInputCorrectness(false);
        ui->InputMainA_field->setStyleSheet("QLineEdit { background-color: red }");
    }
    for (int ind = 0; ind < newMainAlphabet.size(); ++ind)
    {
        if (ui->InputExtraA_field->displayText().indexOf(newMainAlphabet.at(ind)) != -1 ||
                newMainAlphabet.indexOf(newMainAlphabet.at(ind)) != ind)
        {
            setInputCorrectness(false);
            ui->InputMainA_field->setStyleSheet("QLineEdit { background-color: red }");
            return;
        }
    }
    ui->InputMainA_field->setStyleSheet("QLineEdit { background-color: white }");
    setInputCorrectness(true);
}


void AlphabetWindow::on_InputExtraA_field_textChanged(const QString &newExtraAlphabet)
{
    for (int ind = 0; ind < newExtraAlphabet.size(); ++ind)
    {
        if (ui->InputMainA_field->displayText().indexOf(newExtraAlphabet.at(ind)) != -1 ||
                newExtraAlphabet.indexOf(newExtraAlphabet.at(ind)) != ind)
        {
            ui->InputExtraA_field->setStyleSheet("QLineEdit { background-color: red }");
            setInputCorrectness(false);
            return;
        }
    }
    ui->InputExtraA_field->setStyleSheet("QLineEdit { background-color: white }");
    setInputCorrectness(true);

    if (newExtraAlphabet.isEmpty()) {
        ui->InputExtraA_field->setStyleSheet("QLineEdit { background-color: #d7d6d5 }");
    }
}

void AlphabetWindow::on_MakeAlphabetButton_clicked()
{
    if (getInputCorrectness())
    {
        setMainAlphabet(ui->InputMainA_field->displayText());
        setExtraAlphabet(ui->InputExtraA_field->displayText());
        emit doneInputtingAlphabets();
        this->close();
    }
}

