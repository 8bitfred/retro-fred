set(FRED_SOUNDS
    climbkok.wav climbtak.wav collision.wav
    exit_maze.wav fire.wav funeral_march.wav
    game_over.wav jump.wav pick_object.wav walk.wav
    loading1.wav loading2.wav loading3.wav loading4.wav)

file(COPY ${FRED_SOUNDS} DESTINATION .)
install(FILES ${FRED_SOUNDS} DESTINATION sounds)
