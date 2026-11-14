# BDFR Studio Workspace

This branch extends the unified Qt desktop application with two usable workflows:

## Live Capture

1. Open the **Live Capture** tab.
2. Select **Refresh cameras** to enumerate OpenCV video inputs 0–9.
3. Select a detected camera and press **Start preview**.
4. The preview displays the latest BGR camera frame. The status line reports frame dimensions, sequence number, measured FPS and dropped-frame count.
5. Press **Stop** before switching devices or closing the application.

The preview uses the migrated `OpenCVCameraSource` capture adapter. The GUI polls the adapter on a 16 ms Qt timer and copies each frame into a Qt-owned image before rendering, so the display does not retain a pointer into a recycled capture buffer.

The current UI intentionally opens one camera at a time. Multi-camera synchronized capture remains a later integration step; the existing rig calibration workflow remains available separately.

## Camera Calibration

Use **Camera & Rig Calibration** to open the calibration wizard. Single-camera mode estimates intrinsics; rig mode estimates relative extrinsics from synchronized observations. Camera calibration and live preview are independent workflows.

## Session Library

1. Open the **Session Library** tab.
2. Choose the directory containing `.bdfrtake` or legacy CSV session files.
3. Press **Scan sessions**.
4. Review format, file size, frame count, duration and validation status.
5. Select a row and choose **Inspect selected session** (or double-click it) to load the take and view its timeline bounds and truncated-tail recovery status.

The current session index validates each candidate by loading it through the existing `TakeReader`. For directories containing very large takes, scanning can take noticeable time. Session playback, timeline scrubbing and animation reconstruction are not part of this milestone.

## Build prerequisites

Qt 6 Widgets and OpenCV (core, imgproc, videoio, calib3d) are required. Configure the unified Studio target with:

```sh
cmake -S . -B build-studio \
  -DCMAKE_BUILD_TYPE=Release \
  -DBDFR_BUILD_TESTS=OFF \
  -DBDFR_ENABLE_OPENCV=ON \
  -DBDFR_BUILD_UNIFIED_STUDIO=ON
cmake --build build-studio --parallel
```

No release build is implied by this documentation. Live hardware behavior still requires validation on the target workstation and cameras.
