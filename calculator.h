#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStack>
#include <QGridLayout>

class Calculator : public QWidget {
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);

private slots:
    void onDigitPressed();
    void onOperatorPressed();
    void onEqualsPressed();
    void onClearPressed();

private:
    QLineEdit *display;
    QStack<double> operandStack;
    QStack<QChar> operatorStack;

    void calculate();
    int precedence(QChar op);
    double applyOperator(double a, double b, QChar op);
};

#endif // CALCULATOR_H
