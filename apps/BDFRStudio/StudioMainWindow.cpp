#include "StudioMainWindow.h"
#include "CalibrationWizardDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

StudioMainWindow::StudioMainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("BDFR Human Animation Studio");
    resize(980, 680);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(16);

    auto* heading = new QLabel("BDFR Human Animation Studio", central);
    QFont heading_font = heading->font();
    heading_font.setPointSize(20);
    heading_font.setBold(true);
    heading->setFont(heading_font);

    auto* description = new QLabel(
        "Unified workspace for camera setup, human-performance capture, "
        "session review and animation pipeline tools.", central);
    description->setWordWrap(true);

    calibration_button_ = new QPushButton("Camera & Rig Calibration", central);
    calibration_button_->setMinimumHeight(48);
    calibration_button_->setToolTip(
        "Calibrate camera intrinsics or solve synchronized multi-camera rig extrinsics.");

    status_ = new QLabel(
        "Ready · 64-bit engine core connected", central);

    layout->addWidget(heading);
    layout->addWidget(description);
    layout->addSpacing(12);
    layout->addWidget(calibration_button_);
    layout->addStretch(1);
    layout->addWidget(status_);

    setCentralWidget(central);
    statusBar()->showMessage("BDFR Studio ready");

    connect(calibration_button_, &QPushButton::clicked, this, [this] {
        open_calibration_wizard();
    });
}

void StudioMainWindow::open_calibration_wizard() {
    CalibrationWizardDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        status_->setText("Calibration workflow closed");
    }
}
