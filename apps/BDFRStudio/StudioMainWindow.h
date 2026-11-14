#pragma once

#include <QMainWindow>
#include <QImage>

#include <memory>

namespace bdfrpc {
class OpenCVCameraSource;
}

class QLabel;
class QPushButton;
class QComboBox;
class QTimer;
class QTabWidget;
class QTableWidget;
class QLineEdit;

class StudioMainWindow final : public QMainWindow {
public:
    explicit StudioMainWindow(QWidget* parent = nullptr);
    ~StudioMainWindow() override;

private:
    void open_calibration_wizard();
    void refresh_cameras();
    void start_camera();
    void stop_camera();
    void update_camera_frame();
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
    QComboBox* camera_selector_{nullptr};
    QLineEdit* sessions_directory_{nullptr};
    QTabWidget* tabs_{nullptr};
    QTableWidget* sessions_table_{nullptr};
    QTimer* preview_timer_{nullptr};
    std::unique_ptr<bdfrpc::OpenCVCameraSource> camera_;
    QImage last_frame_;
};
