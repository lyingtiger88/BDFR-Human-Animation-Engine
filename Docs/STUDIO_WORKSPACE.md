# BDFR Studio Workspace

This branch extends the unified Qt desktop application with two usable workflows:

## Live Capture

1. Open the **Live Capture** tab.
2. Select **Refresh cameras** to enumerate OpenCV video inputs 0–9.
3. Check one or more detected cameras in the device list.
4. Press **Start preview**. All selected cameras are opened as one capture group; if any selected camera fails to open, the partial group is stopped and capture does not start.
5. The preview renders a camera mosaic. Frames are matched by their host monotonic timestamps with an 8 ms synchronization tolerance. The status line reports current and maximum observed set skew, average measured FPS, source-buffer drops and synchronized set count.
6. Press **Stop** before changing the selected devices or closing the application.

Each source uses the migrated `OpenCVCameraSource` adapter. The GUI drains bounded source queues on a 16 ms Qt timer and feeds the migrated `FrameSynchronizer`; the mosaic copies image memory before rendering. This is host-clock synchronization, not hardware-trigger/genlock synchronization. The 8 ms tolerance is configurable in code and must be validated against the actual camera combination and frame rates.

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

A synthetic regression test covers pairing, skew limits and stale-frame dropping. The Studio feature branch has not been built or tested with physical cameras. Host-clock alignment is not a substitute for hardware-trigger synchronization; validate frame skew, USB bandwidth and capture stability on the target workstation before production use.
