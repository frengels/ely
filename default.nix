{ lib, stdenv, cmake, ninja, doctest, boost }:
stdenv.mkDerivation {
  pname = "mlisp";
  version = "0.0.1";

  nativeBuildInputs = [ cmake doctest boost ];
  buildInputs = [];

  src = ./.;

  cmakeFlags = [
    "-DMLI_TESTS=ON"
    "-DMLI_EXAMPLES=OFF"
  ];

  enableParallelBuilding = true;

  doCheck = true;
  checkTarget = "test";
}