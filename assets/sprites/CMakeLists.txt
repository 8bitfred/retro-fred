set(FRED_SPRITES
    aciddrop.png block.png bullet.png controller.png chameleon.png
    fred_logo.png fred.png fred_puffing.png ghost.png mummy.png object.png
    power.png rat.png skeleton.png small_digits.png smoke.png
    splash_screen.png todays_greatest.png tomb.png
    vampire.png window.png zx_font.png fred_icon.png
    loading1.png loading2.png loading3.png loading4.png)
file(COPY ${FRED_SPRITES} DESTINATION .)
install(FILES ${FRED_SPRITES} DESTINATION sprites)
