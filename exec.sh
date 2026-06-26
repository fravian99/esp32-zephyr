west build --sysbuild -p auto -b esp32c6_devkitc/esp32c6/hpcore  .  # -- -DZEPHYR_SCA_VARIANT=dtdoctor -DOPENOCD=/home/luis/.espressif/tools/openocd-esp32/v0.12.0-esp32-20260304/openocd-esp32/bin/openocd -DOPENOCD_DEFAULT_PATH=/home/luis/.espressif/tools/openocd-esp32/v0.12.0-esp32-20260304/openocd-esp32/share/openocd/scripts
west flash
west espressif monitor
