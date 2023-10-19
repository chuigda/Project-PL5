#!/usr/bin/env bash

./mscm.exe \
  libmscmstd.dll \
  ./ext/vec/target/debug/mscm_vec.dll \
  sample/chess/logic.scm \
  sample/chess/chessboard.scm \
  sample/chess/attack-check.scm \
  sample/chess/chess.scm
