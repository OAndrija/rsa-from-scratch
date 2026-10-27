#ifndef VAJA1_MAINWINDOW_H
#define VAJA1_MAINWINDOW_H

// src/gui/MainWindow.h
#pragma once
#include <QMainWindow>
#include "core/LCG.h"

class QSpinBox; class QComboBox; class QLineEdit; class QPlainTextEdit; class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onGeneratePrime();
    void onRunKeygenBenchmark();

private:
    QWidget* createPrimeTab();
    QWidget* createBenchmarkTab();

    LCG lcg;   // the GUI owns the generator, like ConsoleApp did

    QSpinBox* bitsSpin = nullptr;
    QComboBox* methodCombo = nullptr;
    QSpinBox* sSpin = nullptr;
    QLineEdit* primeResult = nullptr;
    QPlainTextEdit* logView = nullptr;
    QPushButton* benchBtn = nullptr;
};

#endif //VAJA1_MAINWINDOW_H
