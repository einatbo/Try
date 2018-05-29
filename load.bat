setlocal
set path=%path%;\\tanap1\proj_sw\sw_dev_tools\Nuvoton\FlashLoader\v4.21.01\Rider
rem RiderLoader.exe %1 -e -offset 0x1F5000 -i \\tanap1\proj_sw\sw_dev_tools\Nuvoton\FlashLoader\v4.21.01\Rider\spi_cfg\MXIC_MX25L1605D.cfg -g
RiderLoader.exe %1 -partial_erase -offset 0x1F5000 -i \\tanap1\proj_sw\sw_dev_tools\Nuvoton\FlashLoader\v4.21.01\Rider\spi_cfg\SST_25VF016B.cfg -g

endlocal
