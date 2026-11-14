#include "StudioMainWindow.h"
#include "CalibrationWizardDialog.h"

#include "bdfrpc/OpenCVCameraSource.h"
#include "bdfrpc/OpenCVDeviceEnumerator.h"

#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>

#include <algorithm>
#include <utility>

StudioMainWindow::StudioMainWindow(QWidget* parent)
    : QMainWindow(parent), preview_timer_(new QTimer(this)) {
    setWindowTitle("BDFR Human Animation Studio");
    resize(1180, 760);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(24, 22, 24, 22);
    root->setSpacing(14);

    auto* heading = new QLabel("BDFR Human Animation Studio", central);
    QFont heading_font = heading->font();
    heading_font.setPointSize(20);
    heading_font.setBold(true);
    heading->setFont(heading_font);

    auto* description = new QLabel(
        "Live capture, camera calibration and human-performance workflow.",
        central);
    description->setWordWrap(true);
    root->addWidget(heading);
    root->addWidget(description);

    auto* toolbar = new QHBoxLayout();
    camera_selector_ = new QComboBox(central);
    camera_selector_->setMinimumWidth(260);
    camera_selector_->setPlaceholderText("Select a camera");
    refresh_button_ = new QPushButton("Refresh cameras", central);
    start_button_ = new QPushButton("Start preview", central);
    stop_button_ = new QPushButton("Stop", central);
    stop_button_->setEnabled(false);
    calibration_button_ = new QPushButton("Camera & Rig Calibration", central);
    toolbar->addWidget(new QLabel("Video input:", central));
    toolbar->addWidget(camera_selector_, 1);
    toolbar->addWidget(refresh_button_);
    toolbar->addWidget(start_button_);
    toolbar->addWidget(stop_button_);
    toolbar->addWidget(calibration_button_);
    root->addLayout(toolbar);

    preview_ = new QLabel(central);
    preview_->setMinimumSize(640, 420);
    preview_->setAlignment(Qt::AlignCenter);
    preview_->setStyleSheet(
        "QLabel { background: #11151c; color: #aab4c3; "
        "border: 1px solid #354052; border-radius: 8px; }");
    preview_->setText("Camera preview is stopped");
    preview_->setScaledContents(false);
    root->addWidget(preview_, 1);

    camera_details_ = new QLabel(
        "No camera selected · 64-bit engine core connected", central);
    root->addWidget(camera_details_);

    status_ = new QLabel("Ready", central);
    root->addWidget(status_);

    setCentralWidget(central);
    statusBar()->showMessage("BDFR Studio ready");

    connect(calibration_button_, &QPushButton::clicked, this, [this] {
        open_calibration_wizard();
    });
    connect(refresh_button_, &QPushButton::clicked, this, [this] {
        refresh_cameras();
    });
    connect(start_button_, &QPushButton::clicked, this, [this] {
        start_camera();
    });
    connect(stop_button_, &QPushButton::clicked, this, [this] {
        stop_camera();
    });
    connect(preview_timer_, &QTimer::timeout, this, [this] {
        update_camera_frame();
    });

    preview_timer_->setInterval(16);
    refresh_cameras();
}

StudioMainWindow::~StudioMainWindow() {
    stop_camera();
}

void StudioMainWindow::open_calibration_wizard() {
    CalibrationWizardDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        status_->setText("Calibration workflow closed");
    }
}

void StudioMainWindow::refresh_cameras() {
    const int previous_index = camera_selector_->currentData().toInt();
    camera_selector_->clear();
    status_->setText("Scanning camera inputs…");
    refresh_button_->setEnabled(false);

    const auto devices = bdfrpc::OpenCVDeviceEnumerator::probe(0, 9);
    for (const auto& device : devices) {
        const QString label = QString("%1 · %2x%3 · %4")
            .arg(QString::fromStdString(device.name))
            .arg(device.width)
            .arg(device.height)
            .arg(QString::fromStdString(device.backend));
        camera_selector_->addItem(label, device.device_index);
    }

    const int previous_row = camera_selector_->findData(previous_index);
    if (previous_row >= 0) camera_selector_->setCurrentIndex(previous_row);

    refresh_button_->setEnabled(true);
    if (devices.empty()) {
        status_->setText("No camera detected. Connect a camera and refresh.");
        camera_details_->setText("No available camera inputs");
        show_preview_placeholder("No camera detected");
        start_button_->setEnabled(false);
    } else {
        status_->setText(QString("%1 camera input(s) detected").arg(devices.size()));
        camera_details_->setText("Select a camera and start live preview");
        start_button_->setEnabled(camera_ == nullptr);
    }
}

void StudioMainWindow::start_camera() {
    if (camera_ || camera_selector_->currentIndex() < 0) return;

    bdfrpc::CameraConfig config;
    config.device_index = camera_selector_->currentData().toInt();
    config.width = 1280;
    config.height = 720;
    config.fps = 30.0;
    config.buffer_frames = 2;
    config.source_id = "studio_camera_" + std::to_string(config.device_index);
    config.stream_id = "studio_camera:" + std::to_string(config.device_index);

    auto candidate = std::make_unique<bdfrpc::OpenCVCameraSource>(config);
    if (!candidate->start()) {
        status_->setText("Unable to open the selected camera");
        show_preview_placeholder("Camera could not be opened");
        return;
    }

    camera_ = std::move(candidate);
    camera_selector_->setEnabled(false);
    refresh_button_->setEnabled(false);
    start_button_->setEnabled(false);
    stop_button_->setEnabled(true);
    camera_details_->setText(
        QString("Camera %1 · requesting 1280×720 at 30 FPS")
            .arg(config.device_index));
    status_->setText("Live camera stream started");
    preview_timer_->start();
}

void StudioMainWindow::stop_camera() {
    if (preview_timer_) preview_timer_->stop();
    if (camera_) {
        camera_->stop();
        camera_.reset();
    }
    if (camera_selector_) camera_selector_->setEnabled(true);
    if (refresh_button_) refresh_button_->setEnabled(true);
    if (start_button_)
        start_button_->setEnabled(camera_selector_ &&
                                  camera_selector_->currentIndex() >= 0);
    if (stop_button_) stop_button_->setEnabled(false);
    if (camera_details_) camera_details_->setText("Camera stream stopped");
    if (preview_) show_preview_placeholder("Camera preview is stopped");
}

void StudioMainWindow::update_camera_frame() {
    if (!camera_) return;
    auto frame = camera_->poll();
    if (!frame || !frame->image.valid()) return;

    const auto& image = frame->image;
    if (image.format != bdfrpc::PixelFormat::BGR8) {
        status_->setText("Unsupported camera pixel format");
        return;
    }

    last_frame_ = QImage(
        image.bytes->data(),
        image.width,
        image.height,
        image.stride_bytes,
        QImage::Format_BGR888).copy();

    preview_->setPixmap(QPixmap::fromImage(last_frame_).scaled(
        preview_->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    camera_details_->setText(
        QString("Live · %1×%2 · sequence %3 · measured %4 FPS · dropped %5")
            .arg(image.width)
            .arg(image.height)
            .arg(frame->sequence)
            .arg(camera_->measured_fps(), 0, 'f', 1)
            .arg(camera_->dropped_frames()));
    status_->setText("Receiving live camera frames");
}

void StudioMainWindow::show_preview_placeholder(const QString& message) {
    if (!preview_) return;
    last_frame_ = {};
    preview_->setPixmap(QPixmap());
    preview_->setText(message);
}
