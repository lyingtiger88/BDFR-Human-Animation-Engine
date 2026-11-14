#pragma once

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QPlainTextEdit;
class QProcess;
class QPushButton;
class QSpinBox;

class CalibrationWizardDialog final : public QDialog {
public:
    explicit CalibrationWizardDialog(QWidget* parent = nullptr);
    ~CalibrationWizardDialog() override;

private:
    void update_mode();
    void browse_input_profile();
    void browse_output_profile();
    void start_calibration();
    void stop_calibration();
    void append_output();

    QComboBox* mode_{nullptr};
    QSpinBox* camera_index_{nullptr};
    QSpinBox* reference_index_{nullptr};
    QSpinBox* columns_{nullptr};
    QSpinBox* rows_{nullptr};
    QSpinBox* samples_{nullptr};
    QDoubleSpinBox* square_size_{nullptr};
    QLineEdit* input_profile_{nullptr};
    QLineEdit* output_profile_{nullptr};
    QPushButton* browse_input_{nullptr};
    QPushButton* browse_output_{nullptr};
    QPushButton* start_{nullptr};
    QPushButton* stop_{nullptr};
    QPlainTextEdit* log_{nullptr};
    QProcess* process_{nullptr};
};
