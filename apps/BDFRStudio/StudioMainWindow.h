#pragma once

#include <QMainWindow>
#include <QImage>

#include <memory>

namespace bdfrpc { class OpenCVCameraSource; }

class QLabel;
class QPushButton;
class QComboBox;
class QTimer;

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

    QLabel* status_{nullptr};
    QLabel* preview_{nullptr};
    QLabel* camera_details_{nullptr};
    QPushButton* calibration_button_{nullptr};
    QPushButton* refresh_button_{nullptr};
    QPushButton* start_button_{nullptr};
    QPushButton* stop_button_{nullptr};
    QComboBox* camera_selector_{nullptr};
    QTimer* preview_timer_{nullptr};
    std::unique_ptr<bdfrpc::OpenCVCameraSource> camera_;
    QImage last_frame_;
};
