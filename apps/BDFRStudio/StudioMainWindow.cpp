#include "StudioMainWindow.h"
#include "CalibrationWizardDialog.h"

#include "bdfrpc/OpenCVCameraSource.h"
#include "bdfrpc/OpenCVDeviceEnumerator.h"
#include "bdfrpc/TakeReader.h"
#include "bdfrpc/TakeSessionIndex.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>

#include <algorithm>
#include <utility>

StudioMainWindow::StudioMainWindow(QWidget* parent)
    : QMainWindow(parent), preview_timer_(new QTimer(this)) {
    setWindowTitle("BDFR Human Animation Studio");
    resize(1220, 800);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(24, 22, 24, 22);
    root->setSpacing(14);

    auto* heading = new QLabel("BDFR Human Animation Studio", central);
    QFont heading_font = heading->font();
    heading_font.setPointSize(20);
    heading_font.setBold(true);
    heading->setFont(heading_font);
    root->addWidget(heading);

    auto* description = new QLabel(
        "Unified workspace for live capture, calibration and recorded-session review.",
        central);
    description->setWordWrap(true);
    root->addWidget(description);

    tabs_ = new QTabWidget(central);
    root->addWidget(tabs_, 1);

    auto* capture_page = new QWidget(tabs_);
    auto* capture_layout = new QVBoxLayout(capture_page);
    capture_layout->setContentsMargins(8, 12, 8, 8);
    capture_layout->setSpacing(12);

    auto* toolbar = new QHBoxLayout();
    camera_selector_ = new QComboBox(capture_page);
    camera_selector_->setMinimumWidth(240);
    camera_selector_->setPlaceholderText("Select a camera");
    refresh_button_ = new QPushButton("Refresh cameras", capture_page);
    start_button_ = new QPushButton("Start preview", capture_page);
    stop_button_ = new QPushButton("Stop", capture_page);
    stop_button_->setEnabled(false);
    calibration_button_ = new QPushButton("Camera & Rig Calibration", capture_page);
    toolbar->addWidget(new QLabel("Video input:", capture_page));
    toolbar->addWidget(camera_selector_, 1);
    toolbar->addWidget(refresh_button_);
    toolbar->addWidget(start_button_);
    toolbar->addWidget(stop_button_);
    toolbar->addWidget(calibration_button_);
    capture_layout->addLayout(toolbar);

    preview_ = new QLabel(capture_page);
    preview_->setMinimumSize(640, 420);
    preview_->setAlignment(Qt::AlignCenter);
    preview_->setStyleSheet(
        "QLabel { background: #11151c; color: #aab4c3; "
        "border: 1px solid #354052; border-radius: 8px; }");
    preview_->setText("Camera preview is stopped");
    preview_->setScaledContents(false);
    capture_layout->addWidget(preview_, 1);

    camera_details_ = new QLabel(
        "No camera selected · 64-bit engine core connected", capture_page);
    capture_layout->addWidget(camera_details_);
    tabs_->addTab(capture_page, "Live Capture");

    auto* sessions_page = new QWidget(tabs_);
    auto* sessions_layout = new QVBoxLayout(sessions_page);
    sessions_layout->setContentsMargins(8, 12, 8, 8);
    sessions_layout->setSpacing(10);

    auto* directory_row = new QHBoxLayout();
    sessions_directory_ = new QLineEdit(QDir::homePath(), sessions_page);
    sessions_directory_->setPlaceholderText("Folder containing .bdfrtake or CSV sessions");
    choose_directory_button_ = new QPushButton("Choose folder…", sessions_page);
    refresh_sessions_button_ = new QPushButton("Scan sessions", sessions_page);
    directory_row->addWidget(new QLabel("Session folder:", sessions_page));
    directory_row->addWidget(sessions_directory_, 1);
    directory_row->addWidget(choose_directory_button_);
    directory_row->addWidget(refresh_sessions_button_);
    sessions_layout->addLayout(directory_row);

    sessions_table_ = new QTableWidget(sessions_page);
    sessions_table_->setColumnCount(6);
    sessions_table_->setHorizontalHeaderLabels(
        {"Session", "Format", "Size", "Frames", "Duration", "Validation"});
    sessions_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    sessions_table_->setSelectionMode(QAbstractItemView::SingleSelection);
    sessions_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sessions_table_->setAlternatingRowColors(true);
    sessions_table_->verticalHeader()->setVisible(false);
    sessions_table_->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    for (int column = 1; column < 6; ++column) {
        sessions_table_->horizontalHeader()->setSectionResizeMode(
            column, QHeaderView::ResizeToContents);
    }
    sessions_layout->addWidget(sessions_table_, 1);

    auto* session_actions = new QHBoxLayout();
    session_summary_ = new QLabel("Choose a folder and scan for recorded sessions.", sessions_page);
    inspect_session_button_ = new QPushButton("Inspect selected session", sessions_page);
    inspect_session_button_->setEnabled(false);
    session_actions->addWidget(session_summary_, 1);
    session_actions->addWidget(inspect_session_button_);
    sessions_layout->addLayout(session_actions);
    tabs_->addTab(sessions_page, "Session Library");

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
    connect(choose_directory_button_, &QPushButton::clicked, this, [this] {
        choose_sessions_directory();
    });
    connect(refresh_sessions_button_, &QPushButton::clicked, this, [this] {
        refresh_sessions();
    });
    connect(inspect_session_button_, &QPushButton::clicked, this, [this] {
        inspect_selected_session();
    });
    connect(sessions_table_, &QTableWidget::itemSelectionChanged, this, [this] {
        inspect_session_button_->setEnabled(
            !sessions_table_->selectedItems().isEmpty());
    });
    connect(sessions_table_, &QTableWidget::itemDoubleClicked, this, [this] {
        inspect_selected_session();
    });

    preview_timer_->setInterval(16);
    refresh_cameras();
    refresh_sessions();
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

void StudioMainWindow::choose_sessions_directory() {
    const QString path = QFileDialog::getExistingDirectory(
        this, "Select session folder", sessions_directory_->text());
    if (!path.isEmpty()) {
        sessions_directory_->setText(path);
        refresh_sessions();
    }
}

void StudioMainWindow::refresh_sessions() {
    const QString directory = sessions_directory_->text().trimmed();
    sessions_table_->setRowCount(0);
    if (directory.isEmpty() || !QDir(directory).exists()) {
        session_summary_->setText("Session folder does not exist.");
        inspect_session_button_->setEnabled(false);
        return;
    }

    const auto sessions = bdfrpc::TakeSessionIndex::scan(
        directory.toStdString(), true);
    sessions_table_->setRowCount(static_cast<int>(sessions.size()));

    std::size_t valid_count = 0;
    for (std::size_t row = 0; row < sessions.size(); ++row) {
        const auto& session = sessions[row];
        if (session.valid) ++valid_count;

        const QStringList values{
            QString::fromStdString(session.filename),
            QString::fromLatin1(bdfrpc::TakeSessionIndex::format_name(session.format)),
            QString("%1 MB").arg(
                static_cast<double>(session.size_bytes) / (1024.0 * 1024.0), 0, 'f', 2),
            QString::number(session.frame_count),
            QString("%1 s").arg(
                static_cast<double>(session.duration_ns) / 1.0e9, 0, 'f', 2),
            session.valid ? "Valid" : QString("Invalid: %1").arg(
                QString::fromStdString(session.error))
        };

        for (int column = 0; column < values.size(); ++column) {
            auto* item = new QTableWidgetItem(values[column]);
            item->setData(Qt::UserRole, QString::fromStdString(session.path));
            if (!session.valid) item->setToolTip(values[5]);
            sessions_table_->setItem(static_cast<int>(row), column, item);
        }
    }

    session_summary_->setText(
        QString("%1 session file(s) · %2 valid · %3 invalid")
            .arg(sessions.size())
            .arg(valid_count)
            .arg(sessions.size() - valid_count));
    inspect_session_button_->setEnabled(false);
}

void StudioMainWindow::inspect_selected_session() {
    const int row = sessions_table_->currentRow();
    if (row < 0) return;
    auto* item = sessions_table_->item(row, 0);
    if (!item) return;

    const QString path = item->data(Qt::UserRole).toString();
    bdfrpc::TakeReader reader;
    if (!reader.load(path.toStdString())) {
        QMessageBox::warning(
            this, "Session inspection failed",
            QString::fromStdString(reader.last_error()));
        return;
    }

    const QString report = QString(
        "File: %1\nFrames: %2\nDuration: %3 seconds\nStart: %4 ns\nEnd: %5 ns\nRecovered truncated tail: %6")
        .arg(path)
        .arg(reader.frames().size())
        .arg(static_cast<double>(reader.duration_ns()) / 1.0e9, 0, 'f', 3)
        .arg(reader.start_time_ns())
        .arg(reader.end_time_ns())
        .arg(reader.recovered_truncated_tail() ? "Yes" : "No");

    QMessageBox::information(this, "Session details", report);
}
