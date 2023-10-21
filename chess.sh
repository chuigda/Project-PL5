#!/usr/bin/env bash

# if on Win32 platform, use mscm.exe

if [ -f ./mscm.exe ]; then
  ./mscm.exe \
    libmscmstd.dll \
    ./ext/vec/target/debug/mscm_vec.dll \
    sample/chess/stdsup.scm \
    sample/chess/chessboard.scm \
    sample/chess/attack-check.scm \
    sample/chess/move-check.scm \
    sample/chess/move.scm \
    sample/chess/upgrade.scm \
    $@
  exit 0
else
  # if user specifies VALGRIND environment var
  if [ $VALGRIND ]; then
    LD_LIBRARY_PATH=. proxychains valgrind --leak-check=full \
      ./mscm \
      libmscmstd.so \
      ./ext/vec/target/debug/libmscm_vec.so \
      sample/chess/stdsup.scm \
      sample/chess/chessboard.scm \
      sample/chess/attack-check.scm \
      sample/chess/move-check.scm \
      sample/chess/move.scm \
      sample/chess/upgrade.scm \
      $@
  else
    LD_LIBRARY_PATH=. ./mscm \
      libmscmstd.so \
      ./ext/vec/target/debug/libmscm_vec.so \
      sample/chess/stdsup.scm \
      sample/chess/chessboard.scm \
      sample/chess/attack-check.scm \
      sample/chess/move-check.scm \
      sample/chess/move.scm \
      sample/chess/upgrade.scm \
      $@
  fi
fi
