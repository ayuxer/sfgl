{
  perSystem = {pkgs, ...}: {
    packages.default = pkgs.callPackage ../../packages/sfgl.nix {};
  };
}
