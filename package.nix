{ lib
, stdenv
, cmake
, extra-cmake-modules
, kwin
, wrapQtAppsHook
, qttools
}:

stdenv.mkDerivation rec {
  pname = "kwin-effects-startupfeedback-busy";
  version = "1.0.0";

  src = ./.;

  nativeBuildInputs = [
    cmake
    extra-cmake-modules
    wrapQtAppsHook
  ];

  buildInputs = [
    kwin
    qttools
  ];

  meta = with lib; {
    description = "Busy/wait cursor launch feedback for KDE Plasma 6 (Wayland only)";
    license = licenses.gpl3;
    homepage = "https://github.com/taj-ny/kwin-effects-startupfeedback-busy-cursor";
  };
}
