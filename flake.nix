{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem flake-utils.lib.allSystems (system: let pkgs = import nixpkgs {
      inherit system;
      overlays = [];
      config.allowUnfree = false;
    };
    in {
      defaultPackage = pkgs.callPackage ./default.nix {};
    });
}