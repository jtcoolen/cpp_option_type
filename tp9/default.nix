{ pkgs ? import <nixpkgs> {} }:

with pkgs;
mkShell {
  buildInputs = 
  [ 
    cmake gcc clang valgrind
  ];
}
