#include "CalibrationWizardDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProcess>
#include <QStringList>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

CalibrationWizardDialog::CalibrationWizardDialog(QWidget* parent)
    : QDialog(parent), process_(new QProcess(this)) {
    setWindowTitle("Camera & Rig Calibration");
    resize(760, 600);
    process_->setProcessChannelMode(QProcess::MergedChannels);

    auto* root = new QVBoxLayout(this);
    auto* intro = new QLabel(
        "First calibrate each camera's intrinsics using a checkerboard. "
        "Then use the saved profile to solve synchronized rig extrinsics. "
        "Keep the same checkerboard dimensions and square size throughout.",
        this);
    intro->setWordWrap(true);
    root->addWidget(intro);

    auto* form = new QFormLayout();
    mode_ = new QComboBox(this);
    mode_->addItem("Single camera · Intrinsics");
    mode_->addItem("Multi-camera rig · Extrinsics");
    form->addRow("Calibration mode:", mode_);

    camera_index_ = new QSpinBox(this);
    camera_index_->setRange(0, 63);
    form->addRow("Camera device index:", camera_index_);

    reference_index_ = new QSpinBox(this);
    reference_index_->setRange(0, 63);
    form->addRow("Reference camera index:", reference_index_);

    columns_ = new QSpinBox(this);
    columns_->setRange(2, 30);
    columns_->setValue(9);
    form->addRow("Checkerboard inner columns:", columns_);

    rows_ = new QSpinBox(this);
    rows_->setRange(2, 30);
    rows_->setValue(6);
    form->addRow("Checkerboard inner rows:", rows_);

    square_size_ = new QDoubleSpinBox(this);
    square_size_->setRange(0.0001, 10.0);
    square_size_->setDecimals(5);
    square_size_->setSingleStep(0.005);
    square_size_->setValue(0.025);
    square_size_->setSuffix(" m");
    form->addRow("Physical square size:", square_size_);

    samples_ = new QSpinBox(this);
    samples_->setRange(5, 200);
    samples_->setValue(20);
    form->addRow("Accepted samples:", samples_);

    auto* input_row = new QHBoxLayout();
    input_profile_ = new QLineEdit(this);
    input_profile_->setPlaceholderText("Existing intrinsics profile for rig mode");
    browse_input_ = new QPushButton("Browse…", this);
    input_row->addWidget(input_profile_, 1);
    input_row->addWidget(browse_input_);
    form->addRow("Input profile:", input_row);

    auto* output_row = new QHBoxLayout();
    output_profile_ = new QLineEdit("cameras.bdfrcal", this);
    browse_output_ = new QPushButton("Browse…", this);
    output_row->addWidget(output_profile_, 1);
    output_row->addWidget(browse_output_);
    form->addRow("Output profile:", output_row);
    root->addLayout(form);

    auto* actions = new QHBoxLayout();
    start_ = new QPushButton("Start calibration", this);
    stop_ = new QPushButton("Stop", this);
    stop_->setEnabled(false);
    actions->addWidget(start_);
    actions->addWidget(stop_);
    actions->addStretch(1);
    root->addLayout(actions);

    log_ = new QPlainTextEdit(this);
    log_->setReadOnly(true);
    log_->setPlaceholderText("Calibration progress and diagnostics appear here.");
    root->addWidget(log_, 1);

    auto* close = new QDialogButtonBox(QDialogButtonBox::Close, this);
    root->addWidget(close);

    connect(mode_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { update_mode(); });
    connect(browse_input_, &QPushButton::clicked,
            this, [this] { browse_input_profile(); });
    connect(browse_output_, &QPushButton::clicked,
            this, [this] { browse_output_profile(); });
    connect(start_, &QPushButton::clicked,
            this, [this] { start_calibration(); });
    connect(stop_, &QPushButton::clicked,
            this, [this] { stop_calibration(); });
    connect(process_, &QProcess::readyRead,
            this, [this] { append_output(); });
    connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus status) {
                append_output();
                start_->setEnabled(true);
                stop_->setEnabled(false);
                if (status == QProcess::NormalExit && code == 0) {
                    log_->appendPlainText("\nCalibration completed successfully.");
                } else {
                    log_->appendPlainText(
                        QString("\nCalibration failed or stopped (exit %1).").arg(code));
                }
            });
    connect(close, &QDialogButtonBox::rejected, this, &QDialog::reject);
    update_mode();
}

CalibrationWizardDialog::~CalibrationWizardDialog() {
    stop_calibration();
}

void CalibrationWizardDialog::update_mode() {
    const bool rig = mode_->currentIndex() == 1;
    camera_index_->setEnabled(!rig);
    reference_index_->setEnabled(rig);
    input_profile_->setEnabled(rig);
    browse_input_->setEnabled(rig);
    if (rig) {
        if (output_profile_->text() == "cameras.bdfrcal")
            output_profile_->setText("rig.bdfrcal");
    } else if (output_profile_->text() == "rig.bdfrcal") {
        output_profile_->setText("cameras.bdfrcal");
    }
}

void CalibrationWizardDialog::browse_input_profile() {
    const auto path = QFileDialog::getOpenFileName(
        this, "Open camera calibration profile", input_profile_->text(),
        "BDFR calibration (*.bdfrcal *.txt);;All files (*)");
    if (!path.isEmpty()) input_profile_->setText(path);
}

void CalibrationWizardDialog::browse_output_profile() {
    const auto path = QFileDialog::getSaveFileName(
        this, "Save calibration profile", output_profile_->text(),
        "BDFR calibration (*.bdfrcal);;Text (*.txt)");
    if (!path.isEmpty()) output_profile_->setText(path);
}

void CalibrationWizardDialog::start_calibration() {
    if (process_->state() != QProcess::NotRunning) return;
    const bool rig = mode_->currentIndex() == 1;
    const QString output = output_profile_->text().trimmed();
    if (output.isEmpty()) {
        log_->appendPlainText("Choose an output profile path.");
        return;
    }

    QString program;
    QStringList args;
    if (!rig) {
        program = QString::fromUtf8(BDFR_CAMERA_CALIBRATE_EXE);
        args = {QString::number(camera_index_->value()),
                QString::number(columns_->value()),
                QString::number(rows_->value()),
                QString::number(square_size_->value(), 'f', 6),
                QString::number(samples_->value()), output};
    } else {
        const QString input = input_profile_->text().trimmed();
        if (input.isEmpty() || !QFileInfo::exists(input)) {
            log_->appendPlainText("Select a valid intrinsics profile before rig calibration.");
            return;
        }
        program = QString::fromUtf8(BDFR_RIG_CALIBRATE_EXE);
        args = {input, QString::number(reference_index_->value()),
                QString::number(columns_->value()),
                QString::number(rows_->value()),
                QString::number(square_size_->value(), 'f', 6),
                QString::number(samples_->value()), output};
    }

    if (!QFileInfo::exists(program)) {
        log_->appendPlainText(QString("Calibration executable not found: %1").arg(program));
        return;
    }

    log_->clear();
    log_->appendPlainText(QString("Launching %1").arg(program));
    process_->setProgram(program);
    process_->setArguments(args);
    process_->start();
    if (!process_->waitForStarted(3000)) {
        log_->appendPlainText("Unable to start calibration process.");
        return;
    }
    start_->setEnabled(false);
    stop_->setEnabled(true);
}

void CalibrationWizardDialog::stop_calibration() {
    if (!process_ || process_->state() == QProcess::NotRunning) return;
    process_->terminate();
    if (!process_->waitForFinished(1000)) {
        process_->kill();
        process_->waitForFinished(1000);
    }
}

void CalibrationWizardDialog::append_output() {
    const auto bytes = process_->readAll();
    if (!bytes.isEmpty())
        log_->appendPlainText(QString::fromUtf8(bytes).trimmed());
}
