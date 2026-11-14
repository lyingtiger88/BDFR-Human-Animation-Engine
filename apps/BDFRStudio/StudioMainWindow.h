#pragma once

#include <QMainWindow>

class QLabel;
class QPushButton;

class StudioMainWindow final : public QMainWindow {
public:
    explicit StudioMainWindow(QWidget* parent = nullptr);

private:
    void open_calibration_wizard();

    QLabel* status_{nullptr};
    QPushButton* calibration_button_{nullptr};
};
