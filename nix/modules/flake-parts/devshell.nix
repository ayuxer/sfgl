{
  perSystem = {pkgs, ...}:
    with pkgs; {
      devShells.default = mkShell {
        nativeBuildInputs = [xorg.libxcb.dev libGL.dev ninja pkg-config];
        buildInputs = [meson];
      };
    };
}
