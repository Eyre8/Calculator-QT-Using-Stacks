#include "calculator.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QChar>
#include <QString>
#include <QStringList>
#include <QtMath>
#include <QRegularExpression>
#include <limits>

Calculator::Calculator(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Calculator-Stacks");
    resize(200, 300);
    setMinimumSize(200, 300);
    setStyleSheet("background: #0b2942;");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create a QLineEdit for displaying the input and results
    display = new QLineEdit(this);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setFixedHeight(50);
    display->setStyleSheet("font-size:24px; padding: 10px; border: 3px solid #003673; border-radius: 5px; background: #93d4ff; font-weight: bold;");
    mainLayout->addWidget(display);

    // Grid layout for buttons
    QGridLayout *buttonLayout = new QGridLayout();
    QString buttonLabels[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"}
    };

    // Create buttons for digits and operators
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            QPushButton *button = new QPushButton(buttonLabels[row][col], this);
            button->setFixedSize(40, 40);
            button->setStyleSheet("QPushButton {"
                                  "background: #0c498e;"
                                  "border: 2px solid #002348;"
                                  "font-size: 15px;"
                                  "color: white;"
                                  "font-weight: bold;"
                                  "border-radius: 5px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #4f6394;"
                                  "}"
                                  "QPushButton:pressed {"
                                  "background-color: #6f8acd;"
                                  "}");
            buttonLayout->addWidget(button, row, col);

            if (buttonLabels[row][col].contains(QRegularExpression("[0-9\\.]"))) {
                connect(button, &QPushButton::clicked, this, &Calculator::onDigitPressed);
            } else if (buttonLabels[row][col] == "=") {
                connect(button, &QPushButton::clicked, this, &Calculator::onEqualsPressed);
            } else {
                connect(button, &QPushButton::clicked, this, &Calculator::onOperatorPressed);
            }
        }
    }

    // Clear button
    QPushButton *clearButton = new QPushButton("C", this);
    clearButton->setFixedSize(40, 40);
    clearButton->setStyleSheet("QPushButton {"
                          "background: #0c498e;"
                          "border: 2px solid #002348;"
                          "font-size: 15px;"
                          "color: white;"
                          "font-weight: bold;"
                          "border-radius: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #4f6394;"
                          "}"
                          "QPushButton:pressed {"
                          "background-color: #6f8acd;"
                          "}");
    buttonLayout->addWidget(clearButton, 4, 0, 1, 4); // Span across 4 columns
    connect(clearButton, &QPushButton::clicked, this, &Calculator::onClearPressed);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void Calculator::onDigitPressed() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        display->setText(display->text() + button->text());
    }
}

void Calculator::onOperatorPressed() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString currentText = display->text();

        if (!currentText.isEmpty()) {
            display->setText(currentText + " " + button->text() + " ");
        }
    }
}

void Calculator::onEqualsPressed() {
    QString currentText = display->text();

    if (!currentText.isEmpty()) {
        QStringList tokens = currentText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        for (const QString &token : tokens) {
            if (token.contains(QRegularExpression("[0-9\\.]"))) {
                operandStack.push(token.toDouble());
            } else if (token.length() == 1) {
                while (!operatorStack.isEmpty() && precedence(token.at(0)) <= precedence(operatorStack.top())) {
                    calculate();
                }
                operatorStack.push(token.at(0));
            }
        }

        // Final calculation
        while (!operatorStack.isEmpty()) {
            calculate();
        }

        // Display the result
        if (!operandStack.isEmpty()) {
            double result = operandStack.pop();
            display->setText(QString::number(result));  // Show the final result
        } else {
            display->setText("Error: Invalid Calculation");  // Handle any invalid calculations
        }
    }
}


void Calculator::onClearPressed() {
    display->clear();
    operandStack.clear();
    operatorStack.clear();  // Clear stacks to reset the calculator
}


void Calculator::calculate() {
    // Ensure there are enough operands and operators
    if (operandStack.size() < 2 || operatorStack.isEmpty()) {
        return;  // Not enough data to perform a calculation
    }

    double b = operandStack.pop();
    double a = operandStack.pop();
    QChar op = operatorStack.pop();

    double result = applyOperator(a, b, op);

    // Check for NaN result which indicates an error
    if (std::isnan(result)) {
        return;  // Prevent further calculations if there's an error
    }

    operandStack.push(result);
}



int Calculator::precedence(QChar op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

double Calculator::applyOperator(double a, double b, QChar op) {
    // Check for division by zero
    if (op == '/' && b == 0) {
        display->setText("Error: Division by 0");  // Handle division by zero
        return std::numeric_limits<double>::quiet_NaN();
    }

    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/') return a / b;

    return 0;
}
