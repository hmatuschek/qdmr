# Building `qdmr`

## Overview

`qdmr` is a Qt6-based application. This document provides instructions to build the project and details the required dependencies for Ubuntu/Debian, Fedora, Arch Linux, and macOS (with Homebrew).

---

## Dependencies

The following dependencies are required:

- `qt6-base-dev`
- `qt6-tools-dev`
- `qt6-tools-dev-tools`
- `qt6-positioning-dev`
- `qt6-serialport-dev`
- `libyaml-cpp-dev`
- `librsvg2-bin`
- `libusb-1.0-0-dev`

Equivalent packages are listed for Fedora, Arch Linux, and macOS.

---

## 1. Ubuntu / Debian

### **Install Dependencies**

```sh
sudo apt update
sudo apt install -y \
  qt6-base-dev qt6-tools-dev qt6-tools-dev-tools \
  qt6-positioning-dev qt6-serialport-dev \
  libyaml-cpp-dev librsvg2-bin libusb-1.0-0-dev \
  build-essential cmake git
```

---

## 2. Fedora

### **Install Dependencies**

```sh
sudo dnf install -y \
  qt6-qtbase-devel qt6-qttools-devel qt6-qtpositioning-devel qt6-qtserialport-devel \
  yaml-cpp-devel librsvg2-tools libusb1-devel \
  make gcc-c++ cmake git
```

---

## 3. Arch Linux

### **Install Dependencies**

```sh
sudo pacman -S --needed \
  qt6-base qt6-tools qt6-positioning qt6-serialport \
  yaml-cpp librsvg libusb cmake git base-devel
```

---

## 4. macOS (Homebrew)

### **Install Dependencies**

```sh
brew install \
  qt@6 yaml-cpp librsvg libusb cmake git
```

> **Note:** You may need to set environment variables for Qt6 tools if Homebrew does not symlink them into your PATH:
>
> ```sh
> export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
> export LDFLAGS="-L/opt/homebrew/opt/qt@6/lib"
> export CPPFLAGS="-I/opt/homebrew/opt/qt@6/include"
> ```

---

## Build Instructions (All Platforms)

1. **Clone the repository**

   ```sh
   git clone https://github.com/billbonney/qdmr.git
   cd qdmr
   ```

2. **Create a build directory**

   ```sh
   mkdir build
   cd build
   ```

3. **Configure the project**

   On some systems, you may need to specify the location of your Qt6 installation:
   ```sh
   cmake .. -DQt6_ROOT=/path/to/qt6
   ```
   The `Qt6_ROOT` specifies the base-path for searching Qt6 related dependencies.

   > On macOS, you may need to help CMake find Qt6:
   > ```sh
   > cmake .. -DQt6_ROOT=$(brew --prefix qt@6)
   > ```

   If you want to specify an install prefix , run e.g. 
   ```sh
   cmake .. -DCMAKE_INSTALL_PREFIX=~/.local/
   ```
   The latter will prepare an user-local installation.

4. **Build the project**

   ```sh
   cmake --build .
   ```

5. **(Optional) Run the application**

   ```sh
   ./src/qdmr
   ```

6. **(Optional) Install**

   ```sh 
   cmake --install .
   ```

   This installs everything under the aforementioned install prefix.

---

## Troubleshooting

- **Missing Packages:** Ensure all dependencies are installed as per your OS section.
- **CMake Can't Find Qt6:** Double-check your `PATH`, `Qt6_ROOT`, and Qt6 installation.
- **Permission Errors:** Try using `sudo` if needed for installation steps, but not for building the project.

---

## License

See [LICENSE](LICENSE) for details.
