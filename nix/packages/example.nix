{
  lib,
  stdenv,
  meson,
  ninja,
  pkg-config,
  pkgs,
}: let
  pname = "felidae-example";
in
  stdenv.mkDerivation {
    inherit pname;
    version = "nightly";
    src = ../..;
    outputs = ["out" "dev"];
    mesonBuildType = "release";
    nativeBuildInputs = [
      meson
      ninja
      pkg-config
    ];
    buildInputs = with pkgs; [
      xorg.libxcb
      libGL
    ];
    installPhase = ''
      mkdir -p $out/lib
    '';
    meta = with lib; {
      license = with licenses; [bsd3];
      maintainers = ["Lívia Gonçalves"];
      platforms = platforms.unix;
    };
  }
