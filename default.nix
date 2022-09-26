{ lib, stdenv, cmake }:
stdenv.mkDerivation {
  pname = "wmc";
  version = "0.0.1";

  nativeBuildInputs = [ cmake ];
  buildInputs = [];

  src = ./.;

  cmakeFlags = [
    "-DWMC_TESTS=ON"
    "-DWMC_EXAMPLES=OFF"
  ];

  enableParallelBuilding = true;

  doCheck = true;
  checkTarget = "test";
}