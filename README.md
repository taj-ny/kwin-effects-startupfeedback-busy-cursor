# kwin-effects-startupfeedback-busy-cursor
A fork of the KWin Startup Feedback effect for KDE Plasma 6 with the ability to use the busy/wait cursor instead of drawing the program icon.

This effect only works on Wayland.

# Installation
## NixOS
``flake.nix``:
```nix
{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

    kwin-effects-startupfeedback-busy-cursor = {
      url = "github:taj-ny/kwin-effects-startupfeedback-busy-cursor";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };
}
```

```nix
{ inputs, pkgs, ... }:

{
  environment.systemPackages = [
    inputs.kwin-effects-startupfeedback-busy-cursor.packages.${pkgs.system}.default
  ];
}
```

# Building from source
### Dependencies
- CMake
- Extra CMake Modules
- Plasma 6
- Qt 6
- KF6
- KWin development packages

<details>
  <summary>Arch Linux</summary>
  <br>

  ```
  sudo pacman -S base-devel git extra-cmake-modules qt6-tools
  ```
</details>

<details>
  <summary>Fedora</summary>
  <br>

  ```
  sudo dnf install git cmake extra-cmake-modules gcc-g++ kf6-kwindowsystem-devel plasma-workspace-devel libplasma-devel qt6-qtbase-private-devel qt6-qtbase-devel cmake kwin-devel extra-cmake-modules kwin-devel kf6-knotifications-devel kf6-kio-devel kf6-kcrash-devel kf6-ki18n-devel kf6-kguiaddons-devel libepoxy-devel kf6-kglobalaccel-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel kf6-kdeclarative-devel kdecoration-devel kf6-kglobalaccel kf6-kdeclarative libplasma kf6-kio qt6-qtbase kf6-kguiaddons kf6-ki18n wayland-devel
  ```
</details>

<details>
  <summary>openSUSE</summary>
  <br>

  ```
  sudo zypper in git cmake-full gcc-c++ kf6-extra-cmake-modules kcoreaddons-devel kguiaddons-devel kconfigwidgets-devel kwindowsystem-devel ki18n-devel kiconthemes-devel kpackage-devel frameworkintegration-devel kcmutils-devel kirigami2-devel "cmake(KF6Config)" "cmake(KF6CoreAddons)" "cmake(KF6FrameworkIntegration)" "cmake(KF6GuiAddons)" "cmake(KF6I18n)" "cmake(KF6KCMUtils)" "cmake(KF6KirigamiPlatform)" "cmake(KF6WindowSystem)" "cmake(Qt6Core)" "cmake(Qt6DBus)" "cmake(Qt6Quick)" "cmake(Qt6Svg)" "cmake(Qt6Widgets)" "cmake(Qt6Xml)" "cmake(Qt6UiTools)" "cmake(KF6Crash)" "cmake(KF6GlobalAccel)" "cmake(KF6KIO)" "cmake(KF6Service)" "cmake(KF6Notifications)" libepoxy-devel kwin6-devel
  ```
</details>

### Building
```sh
git clone https://github.com/taj-ny/kwin-effects-startupfeedback-busy-cursor
cd kwin-effects-startupfeedback-busy-cursor
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

Remove the *build* directory when rebuilding the effect.

# Usage
> [!NOTE]  
> If the effect stops working after a system upgrade, you will need to rebuild it.

1. Install the effect.
2. Open the *Desktop Effects* page in *System Settings*.
3. Enable the *Startup Feedback (Busy Cursor)* effect.
4. Go to *Colors & Themes* -> *Cursors* -> *Configure Launch Feedback...* and set *Cursor feedback* to *None*.
