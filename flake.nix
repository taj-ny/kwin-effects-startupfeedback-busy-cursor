{
  description = "Busy cursor launch feedback for KDE Plasma 6 (Wayland only)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux" "aarch64-linux"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
  in {
    packages.default = pkgs.kdePackages.callPackage ./package.nix { };
  });
}
