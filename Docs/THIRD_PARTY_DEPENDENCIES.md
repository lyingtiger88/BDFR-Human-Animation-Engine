# Third-Party Dependencies and Asset Review

This register is a migration hold-point, not a legal opinion. Neither source tree contained a root license file or an obvious license/notice file at the time the source snapshots were pinned. No project-wide license is inferred or added by this migration.

| Dependency / asset family | Source use | Current handling | Required action before release |
|---|---|---|---|
| Qt 6 | Desktop Studio UI | Optional build dependency | Confirm Qt version, distribution mode and applicable license notices |
| OpenCV | Camera capture, calibration and image processing | Optional dependency | Pin version and include the applicable license/third-party notices |
| MediaPipe | Facial tracking and Android capture | Adapter / runtime integration | Verify exact distribution and model-bundle terms |
| EasyMocap | Body/hand reconstruction worker | External Python worker | Verify upstream license, model/data licenses and modifications |
| SMPL / SMPL-X / MANO | Body and hand model formats | Parsers/mappers only; no model weights copied | Verify model-specific academic/commercial use restrictions and obtain authorized assets |
| eos | Optional photo-to-3D FaceBuilder | CMake FetchContent option, disabled by default | Pin immutable revision and preserve upstream notices |
| OpenRigLogic / MetaHuman DNA | MetaHuman rig runtime | Optional CMake FetchContent option, disabled by default | Review Epic terms and DNA/model access restrictions |
| Unreal Engine / LiveLink | Runtime integration | Plugin source snapshot | Verify Unreal Engine EULA, plugin distribution and LiveLink compatibility |
| CUDA / GPU runtimes | ML inference acceleration | Not linked into core | Pin supported runtime matrix and review redistribution terms |
| AndroidX / CameraX | Android camera capture | Gradle dependencies in source snapshot | Generate dependency/license report from resolved Gradle graph |

## Asset findings from the pinned Git trees

- The two source trees contained 326 tracked files in total.
- No Git submodules or Git LFS pointer files were reported in the source trees.
- No obvious model-weight files (for example ONNX, PyTorch checkpoint or safetensors files) were present in the tracked source paths.
- The facial repository contains model-management/setup documentation; actual model assets may be downloaded separately at runtime or installed by the user.
- The EasyMocap and SMPL-family integrations must not be treated as permission to redistribute upstream code, pretrained weights or datasets.

## Release gate

Do not publish a distributable binary, bundle third-party model weights, or delete the original repositories until the project owner has selected an explicit project license and the dependency/asset notices and permissions have been reviewed.
