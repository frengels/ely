{ lib, stdenv, cmake, ninja, doctest }:
stdenv.mkDerivation {
  pname = "mlisp";
  version = "0.0.1";

  nativeBuildInputs = [ cmake doctest ];
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