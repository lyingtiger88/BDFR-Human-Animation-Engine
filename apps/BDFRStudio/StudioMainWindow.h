#pragma once

#include "bdfrpc/FrameSynchronizer.h"

#include <QMainWindow>
#include <QImage>

#include <memory>
#include <vector>

namespace bdfrpc { class OpenCVCameraSource; }

class QLabel;
class QPushButton;
class QTimer;
class QTabWidget;
class QTableWidget;
class QLineEdit;
class QListWidget;

class StudioMainWindow final : public QMainWindow {
public:
    explicit StudioMainWindow(QWidget* parent = nullptr);
    ~StudioMainWindow() override;

private:
    void open_calibration_wizard();
    void refresh_cameras();
    void start_camera();
    void stop_camera();
    void update_camera_frames();
    void show_preview_placeholder(const QString& message);
    void choose_sessions_directory();
    void refresh_sessions();
    void inspect_selected_session();

    QLabel* status_{nullptr};
    QLabel* preview_{nullptr};
    QLabel* camera_details_{nullptr};
    QLabel* session_summary_{nullptr};
    QPushButton* calibration_button_{nullptr};
    QPushButton* refresh_button_{nullptr};
    QPushButton* start_button_{nullptr};
    QPushButton* stop_button_{nullptr};
    QPushButton* choose_directory_button_{nullptr};
    QPushButton* refresh_sessions_button_{nullptr};
    QPushButton* inspect_session_button_{nullptr};
    QListWidget* camera_list_{nullptr};
    QLineEdit* sessions_directory_{nullptr};
    QTabWidget* tabs_{nullptr};
    QTableWidget* sessions_table_{nullptr};
    QTimer* preview_timer_{nullptr};
    std::vector<std::unique_ptr<bdfrpc::OpenCVCameraSource>> cameras_;
    bdfrpc::FrameSynchronizer synchronizer_{8'000'000, 8};
    QImage last_frame_;
    std::uint64_t synchronized_sets_{0};
};
