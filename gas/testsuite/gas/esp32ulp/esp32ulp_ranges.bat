..\test_esp32ulp\gas\as-new.exe esp32ulp_ranges.s -al -o esp32ulp_ranges.out
..\test_esp32ulp\gas\as-new.exe esp32ulp_globals.s -al -o esp32ulp_globals.out
..\test_esp32ulp\ld\ld-new.exe -o esp32ulp_ranges.e -A elf32-esp32ulp -Map=esp32ulp_ranges.map esp32ulp_ranges.out esp32ulp_globals.out