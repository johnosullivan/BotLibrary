#include <stdio.h>
#include <string.h>

char * append(char * string1, char * string2)
{
    char * result = NULL;
    asprintf(&result, "%s%s", string1, string2);
    return result;
}
int main()
{
  char *layout = "";
  layout = append(layout,"|==========================================================================|\n");
  layout = append(layout,"|Pin# | NAME     (Raspberry Pi GPIO Header A+,B+,Zero, Pi2)     NAME | Pin#|\n");
  layout = append(layout,"|--------------------------------------------------------------------------|\n");
  layout = append(layout,"| 01    3.3v DC Power             [\x1B[31m\u25A1|\x1B[31m\u2299\x1B[0m]                  DC Power 5v    02 |\n");
  layout = append(layout,"| 03    GPIO02 (SDA1,I2C)         [\x1B[34m\u2299\x1B[0m|\x1B[31m\u2299\x1B[0m]                  DC Power 5v    04 |\n");
  layout = append(layout,"| 05    GPIO03 (SCL1,I2C)         [\x1B[34m\u2299\x1B[0m|\u2299]                       Ground    06 |\n");
  layout = append(layout,"| 07    GPIO04 (GPIO_GCLK)        [\x1B[32m\u2299\x1B[0m|\x1B[33m\u2299\x1B[0m]                (TXD0) GPIO14    08 |\n");
  layout = append(layout,"| 09    Ground                    [\u2299|\x1B[33m\u2299\x1B[0m]                (RXD0) GPIO15    10 |\n");
  layout = append(layout,"| 11    GPIO17 (GPIO_GEN0)        [\x1B[32m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]           (GPIO_GEN1) GPIO18    12 |\n");
  layout = append(layout,"| 13    GPIO27 (GPIO_GEN2)        [\x1B[32m\u2299\x1B[0m|\u2299]                       Ground    14 |\n");
  layout = append(layout,"| 15    GPIO22 (GPIO_GEN3)        [\x1B[32m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]           (GPIO_GEN4) GPIO23    16 |\n");
  layout = append(layout,"| 17    3.3v DC Power             [\x1B[31m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]           (GPIO_GEN5) GPIO24    18 |\n");
  layout = append(layout,"| 19    GPIO10 (SPI_MOSI)         [\x1B[35m\u2299\x1B[0m|\u2299]                       Ground    20 |\n");
  layout = append(layout,"| 21    GPIO09 (SPI_MISO)         [\x1B[35m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]           (GPIO_GEN6) GPIO25    22 |\n");
  layout = append(layout,"| 23    GPIO11 (SPI_CLK)          [\x1B[35m\u2299\x1B[0m|\x1B[35m\u2299\x1B[0m]           (SPI_CE0_N) GPIO08    24 |\n");
  layout = append(layout,"| 25    Ground                    [\u2299|\x1B[35m\u2299\x1B[0m]           (SPI_CE1_N) GPIO07    26 |\n");
  layout = append(layout,"| 27    ID_SD (I2C ID EEPROM)     [\x1B[33m\u2299\x1B[0m|\x1B[33m\u2299\x1B[0m]        (I2C ID EEPROM) ID_SC    28 |\n");
  layout = append(layout,"| 29    GPIO05                    [\x1B[32m\u2299\x1B[0m|\u2299]                       Ground    30 |\n");
  layout = append(layout,"| 31    GPIO06                    [\x1B[32m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]                       GPIO12    32 |\n");
  layout = append(layout,"| 33    GPIO13                    [\x1B[32m\u2299\x1B[0m|\u2299]                       Ground    34 |\n");
  layout = append(layout,"| 35    GPIO19                    [\x1B[32m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]                       GPIO16    36 |\n");
  layout = append(layout,"| 37    GPIO26                    [\x1B[32m\u2299\x1B[0m|\x1B[32m\u2299\x1B[0m]                       GPIO20    38 |\n");
  layout = append(layout,"| 39    Ground                    [\u2299|\x1B[32m\u2299\x1B[0m]                       GPIO21    40 |\n");
  layout = append(layout,"|==========================================================================|\n");

  printf("%s\n",layout);
  return 0;
}
