dnl ALSA soundcard configuration
dnl Find out which cards to compile driver for
dnl Copyright (c) by Jaroslav Kysela <perex@suse.cz>,
dnl                  Anders Semb Hermansen <ahermans@vf.telia.no>

AC_DEFUN([ALSA_TOPLEVEL_INIT], [
	CONFIG_SOUND=""
	CONFIG_SND=""
	CONFIG_SND_TIMER=""
	CONFIG_SND_PCM=""
	CONFIG_SND_HWDEP=""
	CONFIG_SND_RAWMIDI=""
	CONFIG_SND_SEQUENCER=""
	CONFIG_SND_SEQ_DUMMY=""
	CONFIG_SND_OSSEMUL=""
	CONFIG_SND_MIXER_OSS=""
	CONFIG_SND_PCM_OSS=""
	CONFIG_SND_PCM_OSS_PLUGINS=""
	CONFIG_SND_SEQUENCER_OSS=""
	CONFIG_SND_RTCTIMER=""
	CONFIG_RTC=""
	CONFIG_SND_SEQ_RTCTIMER_DEFAULT=""
	CONFIG_SND_DYNAMIC_MINORS=""
	CONFIG_EXPERIMENTAL=""
	CONFIG_SND_SUPPORT_OLD_API=""
	CONFIG_SND_VERBOSE_PROCFS=""
	CONFIG_SND_VERBOSE_PRINTK=""
	CONFIG_SND_DEBUG=""
	CONFIG_SND_DEBUG_DETECT=""
	CONFIG_SND_BIT32_EMUL=""
	CONFIG_SND_DEBUG_MEMORY=""
	CONFIG_SND_HPET=""
	CONFIG_HPET=""
	CONFIG_SND_MPU401_UART=""
	CONFIG_SND_OPL3_LIB=""
	CONFIG_SND_OPL4_LIB=""
	CONFIG_SND_VX_LIB=""
	CONFIG_SND_AC97_CODEC=""
	CONFIG_SND_AC97_BUS=""
	CONFIG_SND_DUMMY=""
	CONFIG_SND_VIRMIDI=""
	CONFIG_SND_MTPAV=""
	CONFIG_SND_SERIAL_U16550=""
	CONFIG_SND_MPU401=""
	CONFIG_SND_SERIALMIDI=""
	CONFIG_SND_LOOPBACK=""
	CONFIG_SND_PORTMAN2X4=""
	CONFIG_PARPORT=""
	CONFIG_SND_PCSP=""
	CONFIG_X86=""
	CONFIG_HAVE_PCSP_HOOK=""
	CONFIG_ISA=""
	CONFIG_ISA_DMA_API=""
	CONFIG_SND_AD1848_LIB=""
	CONFIG_SND_CS4231_LIB=""
	CONFIG_SND_ADLIB=""
	CONFIG_SND_AD1816A=""
	CONFIG_PNP=""
	CONFIG_ISAPNP=""
	CONFIG_SND_AD1848=""
	CONFIG_SND_ALS100=""
	CONFIG_SND_AZT2320=""
	CONFIG_SND_CMI8330=""
	CONFIG_SND_CS4231=""
	CONFIG_SND_CS4232=""
	CONFIG_SND_CS4236=""
	CONFIG_SND_DT019X=""
	CONFIG_SND_ES968=""
	CONFIG_SND_ES1688=""
	CONFIG_SND_ES18XX=""
	CONFIG_SND_GUS_SYNTH=""
	CONFIG_SND_GUSCLASSIC=""
	CONFIG_SND_GUSEXTREME=""
	CONFIG_SND_GUSMAX=""
	CONFIG_SND_INTERWAVE=""
	CONFIG_SND_INTERWAVE_STB=""
	CONFIG_SND_OPL3SA2=""
	CONFIG_SND_OPTI92X_AD1848=""
	CONFIG_SND_OPTI92X_CS4231=""
	CONFIG_SND_OPTI93X=""
	CONFIG_SND_MIRO=""
	CONFIG_SND_SB8=""
	CONFIG_SND_SB16=""
	CONFIG_SND_SBAWE=""
	CONFIG_SND_SB16_CSP=""
	CONFIG_BROKEN=""
	CONFIG_PPC=""
	CONFIG_SND_SGALAXY=""
	CONFIG_SND_SSCAPE=""
	CONFIG_SND_WAVEFRONT=""
	CONFIG_SND_PC98_CS4232=""
	CONFIG_X86_PC9800=""
	CONFIG_SND_MSND_PINNACLE=""
	CONFIG_PCI=""
	CONFIG_SND_AD1889=""
	CONFIG_SND_ALS300=""
	CONFIG_SND_ALS4000=""
	CONFIG_SND_ALI5451=""
	CONFIG_SND_ATIIXP=""
	CONFIG_SND_ATIIXP_MODEM=""
	CONFIG_SND_AU8810=""
	CONFIG_SND_AU8820=""
	CONFIG_SND_AU8830=""
	CONFIG_SND_AZT3328=""
	CONFIG_SND_BT87X=""
	CONFIG_SND_BT87X_OVERCLOCK=""
	CONFIG_SND_CA0106=""
	CONFIG_SND_CMIPCI=""
	CONFIG_SND_CS4281=""
	CONFIG_SND_CS46XX=""
	CONFIG_SND_CS46XX_NEW_DSP=""
	CONFIG_SND_CS5535AUDIO=""
	CONFIG_X86_64=""
	CONFIG_SND_EMU10K1=""
	CONFIG_SND_EMU10K1X=""
	CONFIG_SND_ENS1370=""
	CONFIG_SND_ENS1371=""
	CONFIG_SND_ES1938=""
	CONFIG_SND_ES1968=""
	CONFIG_SND_FM801=""
	CONFIG_SND_FM801_TEA575X=""
	CONFIG_VIDEO_DEV=""
	CONFIG_SND_HDA_INTEL=""
	CONFIG_SND_HDSP=""
	CONFIG_SND_HDSPM=""
	CONFIG_SND_ICE1712=""
	CONFIG_SND_ICE1724=""
	CONFIG_SND_INTEL8X0=""
	CONFIG_SND_INTEL8X0M=""
	CONFIG_SND_KORG1212=""
	CONFIG_SND_MAESTRO3=""
	CONFIG_SND_MIXART=""
	CONFIG_SND_NM256=""
	CONFIG_SND_PCXHR=""
	CONFIG_SND_RIPTIDE=""
	CONFIG_FW_LOADER=""
	CONFIG_SND_RME32=""
	CONFIG_SND_RME96=""
	CONFIG_SND_RME9652=""
	CONFIG_SND_SONICVIBES=""
	CONFIG_SND_TRIDENT=""
	CONFIG_SND_VIA82XX=""
	CONFIG_SND_VIA82XX_MODEM=""
	CONFIG_SND_VX222=""
	CONFIG_SND_YMFPCI=""
	CONFIG_SND_PDPLUS=""
	CONFIG_SND_ASIHPI=""
	CONFIG_SND_DARLA20=""
	CONFIG_SND_GINA20=""
	CONFIG_SND_LAYLA20=""
	CONFIG_SND_DARLA24=""
	CONFIG_SND_GINA24=""
	CONFIG_SND_LAYLA24=""
	CONFIG_SND_MONA=""
	CONFIG_SND_MIA=""
	CONFIG_SND_ECHO3G=""
	CONFIG_SND_INDIGO=""
	CONFIG_SND_INDIGOIO=""
	CONFIG_SND_INDIGODJ=""
	CONFIG_SND_POWERMAC=""
	CONFIG_I2C=""
	CONFIG_INPUT=""
	CONFIG_PPC_PMAC=""
	CONFIG_SND_POWERMAC_AUTO_DRC=""
	CONFIG_ARM=""
	CONFIG_SND_SA11XX_UDA1341=""
	CONFIG_ARCH_SA1100=""
	CONFIG_L3=""
	CONFIG_SND_ARMAACI=""
	CONFIG_ARM_AMBA=""
	CONFIG_SND_PXA2XX_PCM=""
	CONFIG_SND_PXA2XX_AC97=""
	CONFIG_ARCH_PXA=""
	CONFIG_SND_S3C2410=""
	CONFIG_ARCH_S3C2410=""
	CONFIG_I2C_SENSOR=""
	CONFIG_SND_PXA2XX_I2SOUND=""
	CONFIG_MIPS=""
	CONFIG_SND_AU1X00=""
	CONFIG_SOC_AU1000=""
	CONFIG_SOC_AU1100=""
	CONFIG_SOC_AU1500=""
	CONFIG_USB=""
	CONFIG_SND_USB_AUDIO=""
	CONFIG_SND_USB_USX2Y=""
	CONFIG_ALPHA=""
	CONFIG_PCMCIA=""
	CONFIG_SND_VXPOCKET=""
	CONFIG_SND_PDAUDIOCF=""
	CONFIG_SPARC=""
	CONFIG_SND_SUN_AMD7930=""
	CONFIG_SBUS=""
	CONFIG_SND_SUN_CS4231=""
	CONFIG_SND_SUN_DBRI=""
	CONFIG_GSC=""
	CONFIG_SND_HARMONY=""
	CONFIG_SOUND_PRIME=""
])

AC_DEFUN([ALSA_TOPLEVEL_SELECT], [
dnl Check for which cards to compile driver for...
AC_MSG_CHECKING(for which soundcards to compile driver for)
AC_ARG_WITH(cards,
  [  --with-cards=<list>     compile driver for cards in <list>; ]
  [                        cards may be separated with commas; ]
  [                        'all' compiles all drivers; ]
  [                        Possible cards are: ]
  [                          seq-dummy, dummy, virmidi, mtpav, serial-u16550, ]
  [                          mpu401, serialmidi, loopback, portman2x4, pcsp, ]
  [                          ad1848-lib, cs4231-lib, adlib, ad1816a, ad1848, ]
  [                          als100, azt2320, cmi8330, cs4231, cs4232, cs4236, ]
  [                          dt019x, es968, es1688, es18xx, gusclassic, ]
  [                          gusextreme, gusmax, interwave, interwave-stb, ]
  [                          opl3sa2, opti92x-ad1848, opti92x-cs4231, opti93x, ]
  [                          miro, sb8, sb16, sbawe, sgalaxy, sscape, ]
  [                          wavefront, pc98-cs4232, msnd-pinnacle, ad1889, ]
  [                          als300, als4000, ali5451, atiixp, atiixp-modem, ]
  [                          au8810, au8820, au8830, azt3328, bt87x, ca0106, ]
  [                          cmipci, cs4281, cs46xx, cs5535audio, emu10k1, ]
  [                          emu10k1x, ens1370, ens1371, es1938, es1968, ]
  [                          fm801, fm801-tea575x, hda-intel, hdsp, hdspm, ]
  [                          ice1712, ice1724, intel8x0, intel8x0m, korg1212, ]
  [                          maestro3, mixart, nm256, pcxhr, riptide, rme32, ]
  [                          rme96, rme9652, sonicvibes, trident, via82xx, ]
  [                          via82xx-modem, vx222, ymfpci, pdplus, asihpi, ]
  [                          darla20, gina20, layla20, darla24, gina24, ]
  [                          layla24, mona, mia, echo3g, indigo, indigoio, ]
  [                          indigodj, powermac, sa11xx-uda1341, armaaci, ]
  [                          pxa2xx-ac97, s3c2410, pxa2xx-i2sound, au1x00, ]
  [                          usb-audio, usb-usx2y, vxpocket, pdaudiocf, ]
  [                          sun-amd7930, sun-cs4231, sun-dbri, harmony ],
  cards="$withval", cards="all")
SELECTED_CARDS=`echo $cards | sed 's/,/ /g'`
for card in $SELECTED_CARDS; do
  probed=
  if test "$card" = "all" -o "$card" = "seq-dummy"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_SEQ_DUMMY="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "dummy"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DUMMY="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "virmidi"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_VIRMIDI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "mtpav"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MTPAV="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "serial-u16550"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIAL_U16550="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "mpu401"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_MPU401="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "serialmidi"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIALMIDI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "loopback"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LOOPBACK="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "portman2x4"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PARPORT" = "y" -o "$CONFIG_PARPORT" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PORTMAN2X4="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pcsp"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_HAVE_PCSP_HOOK" = "y" -o "$CONFIG_HAVE_PCSP_HOOK" = "m" ); then
      CONFIG_SND_PCSP="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ad1848-lib"; then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1848_LIB="m"
      probed=1
  fi
  if test "$card" = "all" -o "$card" = "cs4231-lib"; then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_CS4231_LIB="m"
      probed=1
  fi
  if test "$card" = "all" -o "$card" = "adlib"; then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ADLIB="m"
      probed=1
  fi
  if test "$card" = "all" -o "$card" = "ad1816a"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1816A="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ad1848"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1848_LIB="m"
      CONFIG_SND_AD1848="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "als100"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ALS100="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "azt2320"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_AZT2320="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cmi8330"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1848_LIB="m"
      CONFIG_SND_CMI8330="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs4231"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_CS4231="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs4232"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_CS4232="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs4236"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_CS4236="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "dt019x"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DT019X="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "es968"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES968="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "es1688"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES1688="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "es18xx"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES18XX="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "gusclassic"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUS_SYNTH="m"
      CONFIG_SND_GUSCLASSIC="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "gusextreme"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUS_SYNTH="m"
      CONFIG_SND_GUSEXTREME="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "gusmax"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_GUS_SYNTH="m"
      CONFIG_SND_GUSMAX="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "interwave"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_GUS_SYNTH="m"
      CONFIG_SND_INTERWAVE="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "interwave-stb"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_GUS_SYNTH="m"
      CONFIG_SND_INTERWAVE_STB="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "opl3sa2"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_OPL3SA2="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "opti92x-ad1848"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AD1848_LIB="m"
      CONFIG_SND_OPTI92X_AD1848="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "opti92x-cs4231"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_OPTI92X_CS4231="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "opti93x"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPTI93X="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "miro"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIRO="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sb8"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB8="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sb16"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB16="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sbawe"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SBAWE="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sgalaxy"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1848_LIB="m"
      CONFIG_SND_SGALAXY="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sscape"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_SSCAPE="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "wavefront"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CS4231_LIB="m"
      CONFIG_SND_WAVEFRONT="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pc98-cs4232"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_X86_PC9800" = "y" -o "$CONFIG_X86_PC9800" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PC98_CS4232="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "msnd-pinnacle"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MSND_PINNACLE="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ad1889"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AD1889="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "als300"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ALS300="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "als4000"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ALS4000="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ali5451"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ALI5451="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "atiixp"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "atiixp-modem"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP_MODEM="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "au8810"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8810="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "au8820"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8820="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "au8830"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8830="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "azt3328"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AZT3328="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "bt87x"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_BT87X="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ca0106"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_CA0106="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cmipci"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_CMIPCI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs4281"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS4281="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs46xx"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS46XX="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs5535audio"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
       ! ( test "$CONFIG_X86_64" = "y" -o "$CONFIG_X86_64" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS5535AUDIO="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "emu10k1"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_EMU10K1="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "emu10k1x"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_EMU10K1X="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ens1370"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ENS1370="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ens1371"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ENS1371="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "es1938"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1938="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "es1968"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1968="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "fm801"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_FM801="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "fm801-tea575x"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_VIDEO_DEV" = "y" -o "$CONFIG_VIDEO_DEV" = "m" ); then
      CONFIG_SND_FM801_TEA575X="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "hda-intel"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDA_INTEL="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "hdsp"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSP="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "hdspm"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSPM="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ice1712"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ICE1712="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ice1724"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ICE1724="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "intel8x0"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "intel8x0m"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0M="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "korg1212"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_KORG1212="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "maestro3"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_MAESTRO3="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "mixart"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIXART="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "nm256"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_NM256="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pcxhr"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCXHR="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "riptide"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RIPTIDE="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "rme32"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME32="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "rme96"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME96="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "rme9652"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME9652="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sonicvibes"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SONICVIBES="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "trident"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_TRIDENT="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "via82xx"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "via82xx-modem"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX_MODEM="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "vx222"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VX222="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "ymfpci"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_YMFPCI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pdplus"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDPLUS="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "asihpi"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ASIHPI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "darla20"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA20="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "gina20"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA20="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "layla20"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA20="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "darla24"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA24="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "gina24"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA24="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "layla24"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA24="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "mona"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MONA="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "mia"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIA="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "echo3g"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ECHO3G="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "indigo"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGO="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "indigoio"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGOIO="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "indigodj"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGODJ="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "powermac"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$CONFIG_PPC_PMAC" = "y" -o "$CONFIG_PPC_PMAC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_POWERMAC="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sa11xx-uda1341"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_SA1100" = "y" -o "$CONFIG_ARCH_SA1100" = "m" ) &&
      ( test "$CONFIG_L3" = "y" -o "$CONFIG_L3" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SA11XX_UDA1341="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "armaaci"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ) &&
      ( test "$CONFIG_ARM_AMBA" = "y" -o "$CONFIG_ARM_AMBA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ARMAACI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pxa2xx-ac97"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_PXA" = "y" -o "$CONFIG_ARCH_PXA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      test $kpatchlevel = 6 && CONFIG_SND_PXA2XX_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_AC97="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "s3c2410"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ) &&
      ( test "$CONFIG_I2C_SENSOR" = "y" -o "$CONFIG_I2C_SENSOR" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_S3C2410="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pxa2xx-i2sound"; then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PXA2XX_I2SOUND="m"
      probed=1
  fi
  if test "$card" = "all" -o "$card" = "au1x00"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_MIPS" = "y" -o "$CONFIG_MIPS" = "m" ) &&
      ( ( test "$CONFIG_SOC_AU1000" = "y" -o "$CONFIG_SOC_AU1000" = "m" ) ||
      ( test "$CONFIG_SOC_AU1100" = "y" -o "$CONFIG_SOC_AU1100" = "m" ) ||
      ( test "$CONFIG_SOC_AU1500" = "y" -o "$CONFIG_SOC_AU1500" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kpatchlevel = 6 && CONFIG_SND_AC97_BUS="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU1X00="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "usb-audio"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_AUDIO="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "usb-usx2y"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) ||
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ||
      ( test "$CONFIG_ALPHA" = "y" -o "$CONFIG_ALPHA" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_USX2Y="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "vxpocket"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VXPOCKET="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "pdaudiocf"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) &&
      ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDAUDIOCF="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sun-amd7930"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SPARC" = "y" -o "$CONFIG_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_AMD7930="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sun-cs4231"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SPARC" = "y" -o "$CONFIG_SPARC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_CS4231="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sun-dbri"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SPARC" = "y" -o "$CONFIG_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_DBRI="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "harmony"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_GSC" = "y" -o "$CONFIG_GSC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HARMONY="m"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test -z "$probed"; then
    AC_MSG_ERROR(Unknown soundcard $card)
  elif test "$probed" = "0"; then
    AC_MSG_ERROR(Unsupported soundcard $card)
  fi
done

AC_ARG_WITH(card_options,
  [  --with-card-options=<list> enable driver options in <list>; ]
  [                        options may be separated with commas; ]
  [                        'all' enables all options; ]
  [                        Possible options are: ]
  [                          seq-rtctimer-default, support-old-api, sb16-csp, ]
  [                          bt87x-overclock, cs46xx-new-dsp, ]
  [                          powermac-auto-drc ],
  cards="$withval", cards="all")
SELECTED_OPTIONS=`echo $cards | sed 's/,/ /g'`
for card in $SELECTED_OPTIONS; do
  probed=
  if test "$card" = "all" -o "$card" = "seq-rtctimer-default"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_SND_RTCTIMER" = "y" -o "$CONFIG_SND_RTCTIMER" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_SEQ_RTCTIMER_DEFAULT="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "support-old-api"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ); then
      CONFIG_SND_SUPPORT_OLD_API="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "sb16-csp"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ) &&
      ( ( test "$CONFIG_SND_SB16" = "y" -o "$CONFIG_SND_SB16" = "m" ) ||
      ( test "$CONFIG_SND_SBAWE" = "y" -o "$CONFIG_SND_SBAWE" = "m" ) ) &&
      ( ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ) ||
       ! ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ); then
      CONFIG_SND_SB16_CSP="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "bt87x-overclock"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_SND_BT87X" = "y" -o "$CONFIG_SND_BT87X" = "m" ); then
      CONFIG_SND_BT87X_OVERCLOCK="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "cs46xx-new-dsp"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) &&
      ( test "$CONFIG_SND_CS46XX" = "y" -o "$CONFIG_SND_CS46XX" = "m" ); then
      CONFIG_SND_CS46XX_NEW_DSP="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test "$card" = "all" -o "$card" = "powermac-auto-drc"; then
    if ( test "$CONFIG_SOUND" = "y" -o "$CONFIG_SOUND" = "m" ) &&
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) &&
      ( test "$CONFIG_SND_POWERMAC" = "y" -o "$CONFIG_SND_POWERMAC" = "m" ); then
      CONFIG_SND_POWERMAC_AUTO_DRC="y"
      probed=1
    elif test -z "$probed"; then
      probed=0
    fi
  fi
  if test -z "$probed"; then
    AC_MSG_ERROR(Unknown option $card)
  elif test "$probed" = "0" -a "$card" != "all"; then
    AC_MSG_ERROR(Unsupported option $card)
  fi
done

AC_MSG_RESULT($SELECTED_CARDS)

CONFIG_SND="m"
if test -n "$CONFIG_SOUND"; then
  AC_DEFINE(CONFIG_SOUND_MODULE)
fi
if test -n "$CONFIG_SND_TIMER"; then
  AC_DEFINE(CONFIG_SND_TIMER_MODULE)
fi
if test -n "$CONFIG_SND_PCM"; then
  AC_DEFINE(CONFIG_SND_PCM_MODULE)
fi
if test -n "$CONFIG_SND_HWDEP"; then
  AC_DEFINE(CONFIG_SND_HWDEP_MODULE)
fi
if test -n "$CONFIG_SND_RAWMIDI"; then
  AC_DEFINE(CONFIG_SND_RAWMIDI_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_DUMMY"; then
  AC_DEFINE(CONFIG_SND_SEQ_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_RTCTIMER_DEFAULT"; then
  AC_DEFINE(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
fi
if test -n "$CONFIG_SND_SUPPORT_OLD_API"; then
  AC_DEFINE(CONFIG_SND_SUPPORT_OLD_API)
fi
if test -n "$CONFIG_HPET"; then
  AC_DEFINE(CONFIG_HPET_MODULE)
fi
if test -n "$CONFIG_SND_MPU401_UART"; then
  AC_DEFINE(CONFIG_SND_MPU401_UART_MODULE)
fi
if test -n "$CONFIG_SND_OPL3_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL3_LIB_MODULE)
fi
if test -n "$CONFIG_SND_OPL4_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL4_LIB_MODULE)
fi
if test -n "$CONFIG_SND_VX_LIB"; then
  AC_DEFINE(CONFIG_SND_VX_LIB_MODULE)
fi
if test -n "$CONFIG_SND_AC97_CODEC"; then
  AC_DEFINE(CONFIG_SND_AC97_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_AC97_BUS"; then
  AC_DEFINE(CONFIG_SND_AC97_BUS_MODULE)
fi
if test -n "$CONFIG_SND_DUMMY"; then
  AC_DEFINE(CONFIG_SND_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_VIRMIDI"; then
  AC_DEFINE(CONFIG_SND_VIRMIDI_MODULE)
fi
if test -n "$CONFIG_SND_MTPAV"; then
  AC_DEFINE(CONFIG_SND_MTPAV_MODULE)
fi
if test -n "$CONFIG_SND_SERIAL_U16550"; then
  AC_DEFINE(CONFIG_SND_SERIAL_U16550_MODULE)
fi
if test -n "$CONFIG_SND_MPU401"; then
  AC_DEFINE(CONFIG_SND_MPU401_MODULE)
fi
if test -n "$CONFIG_SND_SERIALMIDI"; then
  AC_DEFINE(CONFIG_SND_SERIALMIDI_MODULE)
fi
if test -n "$CONFIG_SND_LOOPBACK"; then
  AC_DEFINE(CONFIG_SND_LOOPBACK_MODULE)
fi
if test -n "$CONFIG_SND_PORTMAN2X4"; then
  AC_DEFINE(CONFIG_SND_PORTMAN2X4_MODULE)
fi
if test -n "$CONFIG_PARPORT"; then
  AC_DEFINE(CONFIG_PARPORT_MODULE)
fi
if test -n "$CONFIG_SND_PCSP"; then
  AC_DEFINE(CONFIG_SND_PCSP_MODULE)
fi
if test -n "$CONFIG_HAVE_PCSP_HOOK"; then
  AC_DEFINE(CONFIG_HAVE_PCSP_HOOK_MODULE)
fi
if test -n "$CONFIG_SND_AD1848_LIB"; then
  AC_DEFINE(CONFIG_SND_AD1848_LIB_MODULE)
fi
if test -n "$CONFIG_SND_CS4231_LIB"; then
  AC_DEFINE(CONFIG_SND_CS4231_LIB_MODULE)
fi
if test -n "$CONFIG_SND_ADLIB"; then
  AC_DEFINE(CONFIG_SND_ADLIB_MODULE)
fi
if test -n "$CONFIG_SND_AD1816A"; then
  AC_DEFINE(CONFIG_SND_AD1816A_MODULE)
fi
if test -n "$CONFIG_PNP"; then
  AC_DEFINE(CONFIG_PNP_MODULE)
fi
if test -n "$CONFIG_SND_AD1848"; then
  AC_DEFINE(CONFIG_SND_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_ALS100"; then
  AC_DEFINE(CONFIG_SND_ALS100_MODULE)
fi
if test -n "$CONFIG_SND_AZT2320"; then
  AC_DEFINE(CONFIG_SND_AZT2320_MODULE)
fi
if test -n "$CONFIG_SND_CMI8330"; then
  AC_DEFINE(CONFIG_SND_CMI8330_MODULE)
fi
if test -n "$CONFIG_SND_CS4231"; then
  AC_DEFINE(CONFIG_SND_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_CS4232"; then
  AC_DEFINE(CONFIG_SND_CS4232_MODULE)
fi
if test -n "$CONFIG_SND_CS4236"; then
  AC_DEFINE(CONFIG_SND_CS4236_MODULE)
fi
if test -n "$CONFIG_SND_DT019X"; then
  AC_DEFINE(CONFIG_SND_DT019X_MODULE)
fi
if test -n "$CONFIG_SND_ES968"; then
  AC_DEFINE(CONFIG_SND_ES968_MODULE)
fi
if test -n "$CONFIG_SND_ES1688"; then
  AC_DEFINE(CONFIG_SND_ES1688_MODULE)
fi
if test -n "$CONFIG_SND_ES18XX"; then
  AC_DEFINE(CONFIG_SND_ES18XX_MODULE)
fi
if test -n "$CONFIG_SND_GUS_SYNTH"; then
  AC_DEFINE(CONFIG_SND_GUS_SYNTH_MODULE)
fi
if test -n "$CONFIG_SND_GUSCLASSIC"; then
  AC_DEFINE(CONFIG_SND_GUSCLASSIC_MODULE)
fi
if test -n "$CONFIG_SND_GUSEXTREME"; then
  AC_DEFINE(CONFIG_SND_GUSEXTREME_MODULE)
fi
if test -n "$CONFIG_SND_GUSMAX"; then
  AC_DEFINE(CONFIG_SND_GUSMAX_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE_STB"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_STB_MODULE)
fi
if test -n "$CONFIG_SND_OPL3SA2"; then
  AC_DEFINE(CONFIG_SND_OPL3SA2_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_AD1848"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_CS4231"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_OPTI93X"; then
  AC_DEFINE(CONFIG_SND_OPTI93X_MODULE)
fi
if test -n "$CONFIG_SND_MIRO"; then
  AC_DEFINE(CONFIG_SND_MIRO_MODULE)
fi
if test -n "$CONFIG_SND_SB8"; then
  AC_DEFINE(CONFIG_SND_SB8_MODULE)
fi
if test -n "$CONFIG_SND_SB16"; then
  AC_DEFINE(CONFIG_SND_SB16_MODULE)
fi
if test -n "$CONFIG_SND_SBAWE"; then
  AC_DEFINE(CONFIG_SND_SBAWE_MODULE)
fi
if test -n "$CONFIG_SND_SB16_CSP"; then
  AC_DEFINE(CONFIG_SND_SB16_CSP)
fi
if test -n "$CONFIG_BROKEN"; then
  AC_DEFINE(CONFIG_BROKEN_MODULE)
fi
if test -n "$CONFIG_SND_SGALAXY"; then
  AC_DEFINE(CONFIG_SND_SGALAXY_MODULE)
fi
if test -n "$CONFIG_SND_SSCAPE"; then
  AC_DEFINE(CONFIG_SND_SSCAPE_MODULE)
fi
if test -n "$CONFIG_SND_WAVEFRONT"; then
  AC_DEFINE(CONFIG_SND_WAVEFRONT_MODULE)
fi
if test -n "$CONFIG_SND_PC98_CS4232"; then
  AC_DEFINE(CONFIG_SND_PC98_CS4232_MODULE)
fi
if test -n "$CONFIG_SND_MSND_PINNACLE"; then
  AC_DEFINE(CONFIG_SND_MSND_PINNACLE_MODULE)
fi
if test -n "$CONFIG_SND_AD1889"; then
  AC_DEFINE(CONFIG_SND_AD1889_MODULE)
fi
if test -n "$CONFIG_SND_ALS300"; then
  AC_DEFINE(CONFIG_SND_ALS300_MODULE)
fi
if test -n "$CONFIG_SND_ALS4000"; then
  AC_DEFINE(CONFIG_SND_ALS4000_MODULE)
fi
if test -n "$CONFIG_SND_ALI5451"; then
  AC_DEFINE(CONFIG_SND_ALI5451_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP_MODEM"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_AU8810"; then
  AC_DEFINE(CONFIG_SND_AU8810_MODULE)
fi
if test -n "$CONFIG_SND_AU8820"; then
  AC_DEFINE(CONFIG_SND_AU8820_MODULE)
fi
if test -n "$CONFIG_SND_AU8830"; then
  AC_DEFINE(CONFIG_SND_AU8830_MODULE)
fi
if test -n "$CONFIG_SND_AZT3328"; then
  AC_DEFINE(CONFIG_SND_AZT3328_MODULE)
fi
if test -n "$CONFIG_SND_BT87X"; then
  AC_DEFINE(CONFIG_SND_BT87X_MODULE)
fi
if test -n "$CONFIG_SND_BT87X_OVERCLOCK"; then
  AC_DEFINE(CONFIG_SND_BT87X_OVERCLOCK)
fi
if test -n "$CONFIG_SND_CA0106"; then
  AC_DEFINE(CONFIG_SND_CA0106_MODULE)
fi
if test -n "$CONFIG_SND_CMIPCI"; then
  AC_DEFINE(CONFIG_SND_CMIPCI_MODULE)
fi
if test -n "$CONFIG_SND_CS4281"; then
  AC_DEFINE(CONFIG_SND_CS4281_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX"; then
  AC_DEFINE(CONFIG_SND_CS46XX_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX_NEW_DSP"; then
  AC_DEFINE(CONFIG_SND_CS46XX_NEW_DSP)
fi
if test -n "$CONFIG_SND_CS5535AUDIO"; then
  AC_DEFINE(CONFIG_SND_CS5535AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1"; then
  AC_DEFINE(CONFIG_SND_EMU10K1_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1X"; then
  AC_DEFINE(CONFIG_SND_EMU10K1X_MODULE)
fi
if test -n "$CONFIG_SND_ENS1370"; then
  AC_DEFINE(CONFIG_SND_ENS1370_MODULE)
fi
if test -n "$CONFIG_SND_ENS1371"; then
  AC_DEFINE(CONFIG_SND_ENS1371_MODULE)
fi
if test -n "$CONFIG_SND_ES1938"; then
  AC_DEFINE(CONFIG_SND_ES1938_MODULE)
fi
if test -n "$CONFIG_SND_ES1968"; then
  AC_DEFINE(CONFIG_SND_ES1968_MODULE)
fi
if test -n "$CONFIG_SND_FM801"; then
  AC_DEFINE(CONFIG_SND_FM801_MODULE)
fi
if test -n "$CONFIG_SND_FM801_TEA575X"; then
  AC_DEFINE(CONFIG_SND_FM801_TEA575X_MODULE)
fi
if test -n "$CONFIG_SND_HDA_INTEL"; then
  AC_DEFINE(CONFIG_SND_HDA_INTEL_MODULE)
fi
if test -n "$CONFIG_SND_HDSP"; then
  AC_DEFINE(CONFIG_SND_HDSP_MODULE)
fi
if test -n "$CONFIG_SND_HDSPM"; then
  AC_DEFINE(CONFIG_SND_HDSPM_MODULE)
fi
if test -n "$CONFIG_SND_ICE1712"; then
  AC_DEFINE(CONFIG_SND_ICE1712_MODULE)
fi
if test -n "$CONFIG_SND_ICE1724"; then
  AC_DEFINE(CONFIG_SND_ICE1724_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0M"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0M_MODULE)
fi
if test -n "$CONFIG_SND_KORG1212"; then
  AC_DEFINE(CONFIG_SND_KORG1212_MODULE)
fi
if test -n "$CONFIG_SND_MAESTRO3"; then
  AC_DEFINE(CONFIG_SND_MAESTRO3_MODULE)
fi
if test -n "$CONFIG_SND_MIXART"; then
  AC_DEFINE(CONFIG_SND_MIXART_MODULE)
fi
if test -n "$CONFIG_SND_NM256"; then
  AC_DEFINE(CONFIG_SND_NM256_MODULE)
fi
if test -n "$CONFIG_SND_PCXHR"; then
  AC_DEFINE(CONFIG_SND_PCXHR_MODULE)
fi
if test -n "$CONFIG_SND_RIPTIDE"; then
  AC_DEFINE(CONFIG_SND_RIPTIDE_MODULE)
fi
if test -n "$CONFIG_SND_RME32"; then
  AC_DEFINE(CONFIG_SND_RME32_MODULE)
fi
if test -n "$CONFIG_SND_RME96"; then
  AC_DEFINE(CONFIG_SND_RME96_MODULE)
fi
if test -n "$CONFIG_SND_RME9652"; then
  AC_DEFINE(CONFIG_SND_RME9652_MODULE)
fi
if test -n "$CONFIG_SND_SONICVIBES"; then
  AC_DEFINE(CONFIG_SND_SONICVIBES_MODULE)
fi
if test -n "$CONFIG_SND_TRIDENT"; then
  AC_DEFINE(CONFIG_SND_TRIDENT_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX_MODEM"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_VX222"; then
  AC_DEFINE(CONFIG_SND_VX222_MODULE)
fi
if test -n "$CONFIG_SND_YMFPCI"; then
  AC_DEFINE(CONFIG_SND_YMFPCI_MODULE)
fi
if test -n "$CONFIG_SND_PDPLUS"; then
  AC_DEFINE(CONFIG_SND_PDPLUS_MODULE)
fi
if test -n "$CONFIG_SND_ASIHPI"; then
  AC_DEFINE(CONFIG_SND_ASIHPI_MODULE)
fi
if test -n "$CONFIG_SND_DARLA20"; then
  AC_DEFINE(CONFIG_SND_DARLA20_MODULE)
fi
if test -n "$CONFIG_SND_GINA20"; then
  AC_DEFINE(CONFIG_SND_GINA20_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA20"; then
  AC_DEFINE(CONFIG_SND_LAYLA20_MODULE)
fi
if test -n "$CONFIG_SND_DARLA24"; then
  AC_DEFINE(CONFIG_SND_DARLA24_MODULE)
fi
if test -n "$CONFIG_SND_GINA24"; then
  AC_DEFINE(CONFIG_SND_GINA24_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA24"; then
  AC_DEFINE(CONFIG_SND_LAYLA24_MODULE)
fi
if test -n "$CONFIG_SND_MONA"; then
  AC_DEFINE(CONFIG_SND_MONA_MODULE)
fi
if test -n "$CONFIG_SND_MIA"; then
  AC_DEFINE(CONFIG_SND_MIA_MODULE)
fi
if test -n "$CONFIG_SND_ECHO3G"; then
  AC_DEFINE(CONFIG_SND_ECHO3G_MODULE)
fi
if test -n "$CONFIG_SND_INDIGO"; then
  AC_DEFINE(CONFIG_SND_INDIGO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGOIO"; then
  AC_DEFINE(CONFIG_SND_INDIGOIO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGODJ"; then
  AC_DEFINE(CONFIG_SND_INDIGODJ_MODULE)
fi
if test -n "$CONFIG_SND_POWERMAC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_MODULE)
fi
if test -n "$CONFIG_SND_POWERMAC_AUTO_DRC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_AUTO_DRC)
fi
if test -n "$CONFIG_SND_SA11XX_UDA1341"; then
  AC_DEFINE(CONFIG_SND_SA11XX_UDA1341_MODULE)
fi
if test -n "$CONFIG_SND_ARMAACI"; then
  AC_DEFINE(CONFIG_SND_ARMAACI_MODULE)
fi
if test -n "$CONFIG_ARM_AMBA"; then
  AC_DEFINE(CONFIG_ARM_AMBA_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_PCM"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_PCM_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_AC97_MODULE)
fi
if test -n "$CONFIG_SND_S3C2410"; then
  AC_DEFINE(CONFIG_SND_S3C2410_MODULE)
fi
if test -n "$CONFIG_ARCH_S3C2410"; then
  AC_DEFINE(CONFIG_ARCH_S3C2410_MODULE)
fi
if test -n "$CONFIG_I2C_SENSOR"; then
  AC_DEFINE(CONFIG_I2C_SENSOR_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_I2SOUND"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_I2SOUND_MODULE)
fi
if test -n "$CONFIG_SND_AU1X00"; then
  AC_DEFINE(CONFIG_SND_AU1X00_MODULE)
fi
if test -n "$CONFIG_SOC_AU1000"; then
  AC_DEFINE(CONFIG_SOC_AU1000_MODULE)
fi
if test -n "$CONFIG_SOC_AU1100"; then
  AC_DEFINE(CONFIG_SOC_AU1100_MODULE)
fi
if test -n "$CONFIG_SOC_AU1500"; then
  AC_DEFINE(CONFIG_SOC_AU1500_MODULE)
fi
if test -n "$CONFIG_SND_USB_AUDIO"; then
  AC_DEFINE(CONFIG_SND_USB_AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_USB_USX2Y"; then
  AC_DEFINE(CONFIG_SND_USB_USX2Y_MODULE)
fi
if test -n "$CONFIG_ALPHA"; then
  AC_DEFINE(CONFIG_ALPHA_MODULE)
fi
if test -n "$CONFIG_SND_VXPOCKET"; then
  AC_DEFINE(CONFIG_SND_VXPOCKET_MODULE)
fi
if test -n "$CONFIG_SND_PDAUDIOCF"; then
  AC_DEFINE(CONFIG_SND_PDAUDIOCF_MODULE)
fi
if test -n "$CONFIG_SPARC"; then
  AC_DEFINE(CONFIG_SPARC_MODULE)
fi
if test -n "$CONFIG_SND_SUN_AMD7930"; then
  AC_DEFINE(CONFIG_SND_SUN_AMD7930_MODULE)
fi
if test -n "$CONFIG_SND_SUN_CS4231"; then
  AC_DEFINE(CONFIG_SND_SUN_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_SUN_DBRI"; then
  AC_DEFINE(CONFIG_SND_SUN_DBRI_MODULE)
fi
if test -n "$CONFIG_GSC"; then
  AC_DEFINE(CONFIG_GSC_MODULE)
fi
if test -n "$CONFIG_SND_HARMONY"; then
  AC_DEFINE(CONFIG_SND_HARMONY_MODULE)
fi
])

AC_DEFUN([ALSA_TOPLEVEL_OUTPUT], [
dnl output all subst
AC_SUBST(CONFIG_SOUND)
AC_SUBST(CONFIG_SND)
AC_SUBST(CONFIG_SND_TIMER)
AC_SUBST(CONFIG_SND_PCM)
AC_SUBST(CONFIG_SND_HWDEP)
AC_SUBST(CONFIG_SND_RAWMIDI)
AC_SUBST(CONFIG_SND_SEQUENCER)
AC_SUBST(CONFIG_SND_SEQ_DUMMY)
AC_SUBST(CONFIG_SND_OSSEMUL)
AC_SUBST(CONFIG_SND_MIXER_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS_PLUGINS)
AC_SUBST(CONFIG_SND_SEQUENCER_OSS)
AC_SUBST(CONFIG_SND_RTCTIMER)
AC_SUBST(CONFIG_RTC)
AC_SUBST(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
AC_SUBST(CONFIG_SND_DYNAMIC_MINORS)
AC_SUBST(CONFIG_EXPERIMENTAL)
AC_SUBST(CONFIG_SND_SUPPORT_OLD_API)
AC_SUBST(CONFIG_SND_VERBOSE_PROCFS)
AC_SUBST(CONFIG_SND_VERBOSE_PRINTK)
AC_SUBST(CONFIG_SND_DEBUG)
AC_SUBST(CONFIG_SND_DEBUG_DETECT)
AC_SUBST(CONFIG_SND_BIT32_EMUL)
AC_SUBST(CONFIG_SND_DEBUG_MEMORY)
AC_SUBST(CONFIG_SND_HPET)
AC_SUBST(CONFIG_HPET)
AC_SUBST(CONFIG_SND_MPU401_UART)
AC_SUBST(CONFIG_SND_OPL3_LIB)
AC_SUBST(CONFIG_SND_OPL4_LIB)
AC_SUBST(CONFIG_SND_VX_LIB)
AC_SUBST(CONFIG_SND_AC97_CODEC)
AC_SUBST(CONFIG_SND_AC97_BUS)
AC_SUBST(CONFIG_SND_DUMMY)
AC_SUBST(CONFIG_SND_VIRMIDI)
AC_SUBST(CONFIG_SND_MTPAV)
AC_SUBST(CONFIG_SND_SERIAL_U16550)
AC_SUBST(CONFIG_SND_MPU401)
AC_SUBST(CONFIG_SND_SERIALMIDI)
AC_SUBST(CONFIG_SND_LOOPBACK)
AC_SUBST(CONFIG_SND_PORTMAN2X4)
AC_SUBST(CONFIG_PARPORT)
AC_SUBST(CONFIG_SND_PCSP)
AC_SUBST(CONFIG_X86)
AC_SUBST(CONFIG_HAVE_PCSP_HOOK)
AC_SUBST(CONFIG_ISA)
AC_SUBST(CONFIG_ISA_DMA_API)
AC_SUBST(CONFIG_SND_AD1848_LIB)
AC_SUBST(CONFIG_SND_CS4231_LIB)
AC_SUBST(CONFIG_SND_ADLIB)
AC_SUBST(CONFIG_SND_AD1816A)
AC_SUBST(CONFIG_PNP)
AC_SUBST(CONFIG_ISAPNP)
AC_SUBST(CONFIG_SND_AD1848)
AC_SUBST(CONFIG_SND_ALS100)
AC_SUBST(CONFIG_SND_AZT2320)
AC_SUBST(CONFIG_SND_CMI8330)
AC_SUBST(CONFIG_SND_CS4231)
AC_SUBST(CONFIG_SND_CS4232)
AC_SUBST(CONFIG_SND_CS4236)
AC_SUBST(CONFIG_SND_DT019X)
AC_SUBST(CONFIG_SND_ES968)
AC_SUBST(CONFIG_SND_ES1688)
AC_SUBST(CONFIG_SND_ES18XX)
AC_SUBST(CONFIG_SND_GUS_SYNTH)
AC_SUBST(CONFIG_SND_GUSCLASSIC)
AC_SUBST(CONFIG_SND_GUSEXTREME)
AC_SUBST(CONFIG_SND_GUSMAX)
AC_SUBST(CONFIG_SND_INTERWAVE)
AC_SUBST(CONFIG_SND_INTERWAVE_STB)
AC_SUBST(CONFIG_SND_OPL3SA2)
AC_SUBST(CONFIG_SND_OPTI92X_AD1848)
AC_SUBST(CONFIG_SND_OPTI92X_CS4231)
AC_SUBST(CONFIG_SND_OPTI93X)
AC_SUBST(CONFIG_SND_MIRO)
AC_SUBST(CONFIG_SND_SB8)
AC_SUBST(CONFIG_SND_SB16)
AC_SUBST(CONFIG_SND_SBAWE)
AC_SUBST(CONFIG_SND_SB16_CSP)
AC_SUBST(CONFIG_BROKEN)
AC_SUBST(CONFIG_PPC)
AC_SUBST(CONFIG_SND_SGALAXY)
AC_SUBST(CONFIG_SND_SSCAPE)
AC_SUBST(CONFIG_SND_WAVEFRONT)
AC_SUBST(CONFIG_SND_PC98_CS4232)
AC_SUBST(CONFIG_X86_PC9800)
AC_SUBST(CONFIG_SND_MSND_PINNACLE)
AC_SUBST(CONFIG_PCI)
AC_SUBST(CONFIG_SND_AD1889)
AC_SUBST(CONFIG_SND_ALS300)
AC_SUBST(CONFIG_SND_ALS4000)
AC_SUBST(CONFIG_SND_ALI5451)
AC_SUBST(CONFIG_SND_ATIIXP)
AC_SUBST(CONFIG_SND_ATIIXP_MODEM)
AC_SUBST(CONFIG_SND_AU8810)
AC_SUBST(CONFIG_SND_AU8820)
AC_SUBST(CONFIG_SND_AU8830)
AC_SUBST(CONFIG_SND_AZT3328)
AC_SUBST(CONFIG_SND_BT87X)
AC_SUBST(CONFIG_SND_BT87X_OVERCLOCK)
AC_SUBST(CONFIG_SND_CA0106)
AC_SUBST(CONFIG_SND_CMIPCI)
AC_SUBST(CONFIG_SND_CS4281)
AC_SUBST(CONFIG_SND_CS46XX)
AC_SUBST(CONFIG_SND_CS46XX_NEW_DSP)
AC_SUBST(CONFIG_SND_CS5535AUDIO)
AC_SUBST(CONFIG_X86_64)
AC_SUBST(CONFIG_SND_EMU10K1)
AC_SUBST(CONFIG_SND_EMU10K1X)
AC_SUBST(CONFIG_SND_ENS1370)
AC_SUBST(CONFIG_SND_ENS1371)
AC_SUBST(CONFIG_SND_ES1938)
AC_SUBST(CONFIG_SND_ES1968)
AC_SUBST(CONFIG_SND_FM801)
AC_SUBST(CONFIG_SND_FM801_TEA575X)
AC_SUBST(CONFIG_VIDEO_DEV)
AC_SUBST(CONFIG_SND_HDA_INTEL)
AC_SUBST(CONFIG_SND_HDSP)
AC_SUBST(CONFIG_SND_HDSPM)
AC_SUBST(CONFIG_SND_ICE1712)
AC_SUBST(CONFIG_SND_ICE1724)
AC_SUBST(CONFIG_SND_INTEL8X0)
AC_SUBST(CONFIG_SND_INTEL8X0M)
AC_SUBST(CONFIG_SND_KORG1212)
AC_SUBST(CONFIG_SND_MAESTRO3)
AC_SUBST(CONFIG_SND_MIXART)
AC_SUBST(CONFIG_SND_NM256)
AC_SUBST(CONFIG_SND_PCXHR)
AC_SUBST(CONFIG_SND_RIPTIDE)
AC_SUBST(CONFIG_FW_LOADER)
AC_SUBST(CONFIG_SND_RME32)
AC_SUBST(CONFIG_SND_RME96)
AC_SUBST(CONFIG_SND_RME9652)
AC_SUBST(CONFIG_SND_SONICVIBES)
AC_SUBST(CONFIG_SND_TRIDENT)
AC_SUBST(CONFIG_SND_VIA82XX)
AC_SUBST(CONFIG_SND_VIA82XX_MODEM)
AC_SUBST(CONFIG_SND_VX222)
AC_SUBST(CONFIG_SND_YMFPCI)
AC_SUBST(CONFIG_SND_PDPLUS)
AC_SUBST(CONFIG_SND_ASIHPI)
AC_SUBST(CONFIG_SND_DARLA20)
AC_SUBST(CONFIG_SND_GINA20)
AC_SUBST(CONFIG_SND_LAYLA20)
AC_SUBST(CONFIG_SND_DARLA24)
AC_SUBST(CONFIG_SND_GINA24)
AC_SUBST(CONFIG_SND_LAYLA24)
AC_SUBST(CONFIG_SND_MONA)
AC_SUBST(CONFIG_SND_MIA)
AC_SUBST(CONFIG_SND_ECHO3G)
AC_SUBST(CONFIG_SND_INDIGO)
AC_SUBST(CONFIG_SND_INDIGOIO)
AC_SUBST(CONFIG_SND_INDIGODJ)
AC_SUBST(CONFIG_SND_POWERMAC)
AC_SUBST(CONFIG_I2C)
AC_SUBST(CONFIG_INPUT)
AC_SUBST(CONFIG_PPC_PMAC)
AC_SUBST(CONFIG_SND_POWERMAC_AUTO_DRC)
AC_SUBST(CONFIG_ARM)
AC_SUBST(CONFIG_SND_SA11XX_UDA1341)
AC_SUBST(CONFIG_ARCH_SA1100)
AC_SUBST(CONFIG_L3)
AC_SUBST(CONFIG_SND_ARMAACI)
AC_SUBST(CONFIG_ARM_AMBA)
AC_SUBST(CONFIG_SND_PXA2XX_PCM)
AC_SUBST(CONFIG_SND_PXA2XX_AC97)
AC_SUBST(CONFIG_ARCH_PXA)
AC_SUBST(CONFIG_SND_S3C2410)
AC_SUBST(CONFIG_ARCH_S3C2410)
AC_SUBST(CONFIG_I2C_SENSOR)
AC_SUBST(CONFIG_SND_PXA2XX_I2SOUND)
AC_SUBST(CONFIG_MIPS)
AC_SUBST(CONFIG_SND_AU1X00)
AC_SUBST(CONFIG_SOC_AU1000)
AC_SUBST(CONFIG_SOC_AU1100)
AC_SUBST(CONFIG_SOC_AU1500)
AC_SUBST(CONFIG_USB)
AC_SUBST(CONFIG_SND_USB_AUDIO)
AC_SUBST(CONFIG_SND_USB_USX2Y)
AC_SUBST(CONFIG_ALPHA)
AC_SUBST(CONFIG_PCMCIA)
AC_SUBST(CONFIG_SND_VXPOCKET)
AC_SUBST(CONFIG_SND_PDAUDIOCF)
AC_SUBST(CONFIG_SPARC)
AC_SUBST(CONFIG_SND_SUN_AMD7930)
AC_SUBST(CONFIG_SBUS)
AC_SUBST(CONFIG_SND_SUN_CS4231)
AC_SUBST(CONFIG_SND_SUN_DBRI)
AC_SUBST(CONFIG_GSC)
AC_SUBST(CONFIG_SND_HARMONY)
AC_SUBST(CONFIG_SOUND_PRIME)
])

