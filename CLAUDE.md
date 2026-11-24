# game-framework

C++23 game framework (OpenGL 4.5 core, GLFW, MSVC via CMake). The general rules (working style, C++ style) are in
`D:\frameworks\CLAUDE.md`; this file has what is specific to this framework.

## Layout

- `code/` - the framework app (`main.cpp`, sprites, atlases, fonts, text). `code/data/*.hpp` are the uniform block
  structs.
- `modules/` - graphics, mathematics, physics, platforms, utilities: **git submodules on `develop`**; check their
  own `git status` / `git diff` too.
- `libraries/` - vendored third-party code, trimmed by hand to what the framework needs.
- `build/` - the exe's working folder: shaders, textures, models, font. Run the exe from here.
- `tutorials/` - OpenGL tutorial copies, for reference.

## Build

Use CLion's cmake (the one on PATH may differ):

```powershell
& "C:\Program Files\JetBrains\CLion 2026.2.3\bin\cmake\win\x64\bin\cmake.exe" --build D:\frameworks\game-framework\cmake-build-default --target game-framework --config Release -- /m /v:minimal
```

Not while `game-framework.exe` is running (the exe is locked).

## Specifics

- The precompiled header is `code/headers.hpp` (each module has its own `headers.hpp`).
- OpenGL 4.5 core, DSA style (`glCreate*`, `glTextureStorage*`, `glNamedBuffer*`). Wrapper verbs:
  `create / allocate / upload / bind / destroy` (`allocate` = immutable storage, `upload` = sub-data).
  The vendored GLFW is core profile only, default context 4.5; no profile or version hints needed.
- Uniform blocks are std140: keep `code/data/*.hpp` (`alignas(16)`) in sync with the GLSL in `build/shaders/`.
  Binding slots come from the `texture_binding` / `data::binding` enums via `std::to_underlying`.
- Screen coordinates are pixels from the top left, y down.
- Line endings: `main.cpp` is LF, `font.cpp` and the LICENSE files CRLF (`core.autocrlf=true`).
- Check rendering questions in the running app, not just the code.
- Planned work goes as `// TODO - ...` at the end of `code/main.cpp` under `// VERSION 3`, `// VERSION 2`,
  `// VERSION 1`; add to the section the user names, with enough detail to act on later.
