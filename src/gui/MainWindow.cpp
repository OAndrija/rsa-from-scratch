#include "MainWindow.h"

#include <QTabWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QtConcurrent>

#include "benchmarks/Benchmark.h"
#include "core/MathUtils.h"
#include "core/Primes.h"

namespace {
// std::ostream -> callback per line. Lets the benchmarks' `log` parameter feed a Qt widget.
class LineStreamBuf : public std::streambuf {
public:
    explicit LineStreamBuf(std::function<void(QString)> onLine) : onLine(std::move(onLine)) {}
protected:
    int_type overflow(int_type ch) override {
        if (ch == traits_type::eof()) return ch;
        if (ch == '\n') { onLine(QString::fromStdString(line)); line.clear(); }
        else line.push_back(static_cast<char>(ch));
        return ch;
    }
private:
    std::function<void(QString)> onLine;
    std::string line;
};
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    auto* tabs = new QTabWidget;
    tabs->addTab(createPrimeTab(), "Prastevila");
    tabs->addTab(createBenchmarkTab(), "Testi");
    setCentralWidget(tabs);
    setWindowTitle("Vaja 1 - RSA");
}

QWidget* MainWindow::createPrimeTab() {
    bitsSpin = new QSpinBox;  bitsSpin->setRange(2, 32);  bitsSpin->setValue(16);
    sSpin = new QSpinBox;     sSpin->setRange(1, 100);    sSpin->setValue(10);
    methodCombo = new QComboBox;
    methodCombo->addItems({"Naivna metoda", "Miller-Rabin"});
    primeResult = new QLineEdit;  primeResult->setReadOnly(true);
    auto* btn = new QPushButton("Generiraj");

    auto* form = new QFormLayout;
    form->addRow("Stevilo bitov:", bitsSpin);
    form->addRow("Metoda:", methodCombo);
    form->addRow("Parameter s:", sSpin);
    form->addRow(btn);
    form->addRow("Prastevilo:", primeResult);

    // s only matters for Miller-Rabin
    connect(methodCombo, &QComboBox::currentIndexChanged, this,
            [this](int i) { sSpin->setEnabled(i == 1); });
    sSpin->setEnabled(false);

    connect(btn, &QPushButton::clicked, this, &MainWindow::onGeneratePrime);

    auto* page = new QWidget;
    page->setLayout(form);
    return page;
}

void MainWindow::onGeneratePrime() {
    unsigned long long min = min_from_bits(bitsSpin->value());
    unsigned long long max = max_from_bits(bitsSpin->value());

    unsigned long long p = (methodCombo->currentIndex() == 0)
        ? naive_generate(lcg, min, max)
        : miller_rabin_generate(lcg, min, max, sSpin->value());

    primeResult->setText(QString::number(p));
}

QWidget* MainWindow::createBenchmarkTab() {
    benchBtn = new QPushButton("Test casovne zahtevnosti generiranja kljucev");
    logView = new QPlainTextEdit;  logView->setReadOnly(true);

    auto* layout = new QVBoxLayout;
    layout->addWidget(benchBtn);
    layout->addWidget(logView);
    connect(benchBtn, &QPushButton::clicked, this, &MainWindow::onRunKeygenBenchmark);

    auto* page = new QWidget;
    page->setLayout(layout);
    return page;
}

void MainWindow::onRunKeygenBenchmark() {
    benchBtn->setEnabled(false);
    logView->clear();

    LCG localLcg = lcg;                 // worker gets its own copy, so no data race with the UI thread
    QPlainTextEdit* view = logView;

    auto* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher] {
        benchBtn->setEnabled(true);
        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run([localLcg, view]() mutable {
        LineStreamBuf buf([view](QString line) {
            // called from the worker thread -> queue it onto the UI thread
            QMetaObject::invokeMethod(view, [view, line] { view->appendPlainText(line); },
                                      Qt::QueuedConnection);
        });
        std::ostream log(&buf);
        keygen_timing_test(localLcg);
    }));
}