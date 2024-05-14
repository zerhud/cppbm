{
  description = "collection of utilits for c++";
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs { inherit system; };
      snitch = pkgs.gcc13Stdenv.mkDerivation {
        name = "snitch_tests";
        buildInputs = [ ];
        nativeBuildInputs = with pkgs; [ cmake ninja python3 ];
        #cmakeFlags = [ "-D" "snitch:create_library=true" ];
        #cmakeFlags = [ "-DSNITCH_HEADER_ONLY=ON" ];
        src = pkgs.fetchzip {
          url = "https://github.com/cschreib/snitch/archive/refs/tags/v1.2.2.tar.gz";
          sha256 = "sha256-xeiGCQia0tId4GN/w6Kfz4Ga8u6pWSe6gi9VRz2Pwok=";
        };
      };
      serp = pkgs.gcc13Stdenv.mkDerivation {
        name = "serp";
        buildInputs = [pkgs.boost snitch];
        snitch_header = snitch.out;
        nativeBuildInputs = [pkgs.clang_15];
        CPATH = pkgs.lib.strings.concatStringsSep ":" [
          "${snitch}/include/snitch"
        ];
        LIBRARY_PATH = pkgs.lib.strings.concatStringsSep ":" [
          "${snitch}/lib"
        ];
        #installPhase = "mkdir -p \"$out/include\" && cp serp.hpp -t \"$out/include\" && cp -rt \"$out/include\" serp";
        #buildPhase = "g++ -std=c++23 -fwhole-program -march=native ./test.cpp -o serp && ./serp_test";
        meta.description = "cpp universal serialization library.";
        src = ./serp;
      };
      stfm = pkgs.gcc12Stdenv.mkDerivation {
        name = "stfmeta";
        src = ./stfmeta;
        buildPhase = "make && cp mkdir -p \"$out/include\" && cp -rt \"$out/include\" stfmeta";
      };
    in rec {
      devShell = serp;
      packages.default = serp;
      packages.serp = serp;
      packages.stfm = stfm;
      defaultPackage = serp;
    });
}
