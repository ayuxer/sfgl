{
  perSystem = {pkgs, ...}: {
    packages.default = pkgs.callPackage ../../packages/felidae.nix {};
  };
}
