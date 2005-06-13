    dialog  --clear --backtitle "$BACKTITLE" \
            --title "Select your driver" \
            --menu "$CONFIG_DRIVER_TEXT" 18 74 11 \
               \
			1 "Home-brew (16x50 UART compatible serial port)" \
			2 "Home-brew (parallel port)" \
			3 "Home-brew (soundcard input)" \
			4 "Other serial port devices" \
			5 "TV card" \
			6 "IrDA hardware" \
			7 "PDAs" \
			8 "USB devices" \
			9 "Network (UDP)" \
			0 "Other (MIDI, Bluetooth, etc.)" \
			a "None (network connections only)" 2> $TEMP
    if test "$?" = "0"; then
        {
        set `cat $TEMP`
        if false; then :
        elif test "$1" = "1"; then LIRC_DRIVER=serial;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
        elif test "$1" = "2"; then LIRC_DRIVER=parallel;	DRIVER_PARAMETER=lpt1; DRIVER_PARAM_TYPE=lpt;
        elif test "$1" = "3"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 10 74 3 \
                       \
			1 "Simple IR diode (EXPERIMENTAL)" \
			2 "IR receiver IC connected to auido input (EXPERIMENTAL)" \
			3 "IR receiver IC connected to auido input using ALSA (EXPERIMENTAL)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=dsp;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=audio;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=audio_alsa;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "4"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 19 74 12 \
                       \
			1 "Anir Multimedia Magic" \
			2 "CARACA" \
			3 "Creative Infra Receiver/CIMR100" \
			4 "IRdeo" \
			5 "IRdeo Remote" \
			6 "Irman / UIR" \
			7 "Logitech/AST" \
			8 "PCMAK serial receiver" \
			9 "Miro PCTV receiver" \
			0 "Packard Bell receiver" \
			a "Pinnacle Systems PCTV (pro) receiver" \
			b "PixelView RemoteMaster RC2000/RC3000" \
			c "REALmagic (bundled with Hollywood Plus DVD card)" \
			d "Slink-e (receive only)" \
			e "Silitek SM-1000" \
			f "Tekram Irmate 210 (16x50 UART compatible serial port)" \
			g "UIRT2 (receive only, UIR mode)" \
			h "UIRT2 (receive and transmit)" \
			i "X10 MP3 Anywhere RF Receiver" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=animax;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "2"; then LIRC_DRIVER=caraca;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "3"; then LIRC_DRIVER=creative;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "4"; then LIRC_DRIVER=irdeo;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "5"; then LIRC_DRIVER=irdeo_remote;	DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "6"; then LIRC_DRIVER=irman;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "7"; then LIRC_DRIVER=logitech;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "8"; then LIRC_DRIVER=pcmak;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "9"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "0"; then LIRC_DRIVER=packard_bell;	DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "a"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "b"; then LIRC_DRIVER=remotemaster;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "c"; then LIRC_DRIVER=realmagic;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "d"; then LIRC_DRIVER=slinke;		DRIVER_PARAMETER=tty3; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "e"; then LIRC_DRIVER=silitek;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "f"; then LIRC_DRIVER=tekram;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "g"; then LIRC_DRIVER=uirt2;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "h"; then LIRC_DRIVER=uirt2_raw;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "i"; then LIRC_DRIVER=mp3anywhere;	DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "5"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 19 74 12 \
                       \
			1 "Askey Magic TView CPH03x (card=1)" \
			2 "Askey/Typhoon/Anubis Magic TView CPH051/061 (bt878) (card=24)" \
			3 "Asus TV-Box" \
			4 "AverMedia TV card (TVCapture, TVPhone) (card=6)" \
			5 "AverMedia TV card (TVCapture98, TVPhone98) (card=13/41)" \
			6 "AverMedia TV card (VDOMATE) (use card=13)" \
			7 "BestBuy Easy TV (BT848) (card=55)" \
			8 "BestBuy Easy TV (BT878) (card=62)" \
			9 "Chronos Video Shuttle II (card=35)" \
			0 "Creative BreakOut-Box" \
			a "Dynalink Magic TView (card=48)" \
			b "FlyVideo II (card=8)" \
			c "FlyVideo 98 (card=30)" \
			d "FlyVideo 98/FM /2000S (card=56)" \
			e "Flyvideo 98FM (LR50Q) / Typhoon TView TV/FM Tuner (card=36)" \
			f "Hauppauge TV card (new I2C layer required)" \
			g "Hauppauge DVB-s card (ver. 2.1)" \
			h "Hercules Smart TV Stereo (card=100)" \
			i "I-O Data Co. GV-BCTV5/PCI (card=81)" \
			j "Jetway TV/Capture JW-TV878-FBK, Kworld KW-TV878RF (card=78)" \
			k "KNC ONE TV Station (-/SE/PRO/RDS)" \
			l "Lenco MXTV-9578 CP (card=50)" \
			m "Miro PCTV serial port receiver" \
			n "Phoebe Tv Master + FM (card=22)" \
			o "PixelView PlayTV PAK (card=50)" \
			p "Pixelview PlayTV pro (card=37)" \
			q "Pixelview PlayTV (bt878) (Prolink PV-BT878P+, card=16)" \
			r "Prolink Pixelview PV-BT878P+ (Rev.4C,8E, card=70)" \
			s "Prolink PV-BT878P+4E (card=50)" \
			t "Prolink PV-BT878P+9B (PlayTV Pro rev.9B FM+NICAM) (card=72)" \
			u "ProVideo PV951 (card=42)" \
			v "Technisat MediaFocus I" \
			w "Tekram M230 Mach64 (and others bt829 based)" \
			x "TriTan Technology TView95 CPH03x (card=1)" \
			y "TView99 CPH063 (card=38)" \
			z "Typhoon TView RDS / FM Stereo (card=53)" \
			A "Winfast TV2000/XP (card=34)" \
			B "WinView 601 (card=17)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=tvbox;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=avermedia;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=avermedia98;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "6"; then LIRC_DRIVER=avermedia_vdomate;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "7"; then LIRC_DRIVER=bestbuy;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "8"; then LIRC_DRIVER=bestbuy2;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "9"; then LIRC_DRIVER=chronos;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "0"; then LIRC_DRIVER=breakoutbox;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "a"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "b"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "c"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "d"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "e"; then LIRC_DRIVER=flyvideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "f"; then LIRC_DRIVER=hauppauge;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "g"; then LIRC_DRIVER=hauppauge_dvb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "h"; then LIRC_DRIVER=hercules_smarttv_stereo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "i"; then LIRC_DRIVER=gvbctv5pci;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "j"; then LIRC_DRIVER=kworld;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "k"; then LIRC_DRIVER=knc_one;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "l"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "m"; then LIRC_DRIVER=pctv;		DRIVER_PARAMETER=tty1; DRIVER_PARAM_TYPE=tty;
                elif test "$1" = "n"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "o"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "p"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "q"; then LIRC_DRIVER=pixelview_bt878;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "r"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "s"; then LIRC_DRIVER=pixelview_pak;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "t"; then LIRC_DRIVER=pixelview_pro;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "u"; then LIRC_DRIVER=provideo;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "v"; then LIRC_DRIVER=mediafocusI;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "w"; then LIRC_DRIVER=tekram_bt829;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "x"; then LIRC_DRIVER=cph03x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "y"; then LIRC_DRIVER=cph06x;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "z"; then LIRC_DRIVER=knc_one;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "A"; then LIRC_DRIVER=leadtek_0010;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "B"; then LIRC_DRIVER=leadtek_0007;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "6"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 11 74 4 \
                       \
			1 "SIR IrDA (built-in IR ports)" \
			2 "Tekram Irmate 210 (16x50 UART compatible serial port)" \
			3 "ITE IT8712/IT8705 CIR port (experimental, 2.4.x required)" \
			4 "Actisys Act200L SIR driver support" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=sir;		DRIVER_PARAMETER=com3; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "2"; then LIRC_DRIVER=tekram;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                elif test "$1" = "3"; then LIRC_DRIVER=it87;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=act200l;		DRIVER_PARAMETER=com1; DRIVER_PARAM_TYPE=com;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "7"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 9 74 2 \
                       \
			1 "HP iPAQ" \
			2 "Sharp Zaurus" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=sa1100;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=sa1100;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "8"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 16 74 9 \
                       \
			1 "ATI/NVidia/X10 I & II RF Remote" \
			2 "ATI/NVidia/X10 RF Remote (userspace)" \
			3 "Dign HV5 HTPC IR/VFD Module" \
			4 "Home Electronics Tira USB device" \
			5 "Igor Cesko's USB IR Receiver" \
			6 "PCMAK USB receiver" \
			7 "Sasem OnAir Remocon-V" \
			8 "Sound Blaster Extigy USB sound card" \
			9 "Windows Media Center Remotes" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=atiusb;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=atilibusb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=sasem;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=tira;		DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "5"; then LIRC_DRIVER=igorplugusb;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "6"; then LIRC_DRIVER=pcmak_usb;	DRIVER_PARAMETER=ttyUSB1; DRIVER_PARAM_TYPE=ttyUSB;
                elif test "$1" = "7"; then LIRC_DRIVER=sasem;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "8"; then LIRC_DRIVER=exaudio;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "9"; then LIRC_DRIVER=mceusb;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "9"; then LIRC_DRIVER=udp;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
        elif test "$1" = "0"; then 
            dialog  --clear --backtitle "$BACKTITLE" \
                    --title "Select your driver" \
                    --menu "$CONFIG_DRIVER_TEXT" 12 74 5 \
                       \
			1 "Creative LiveDrive midi" \
			2 "Creative LiveDrive sequencer" \
			3 "Creative iNFRA CDROM" \
			4 "Ericsson mobile phone via Bluetooth" \
			5 "Linux input layer (/dev/input/eventX)" 2> $TEMP
            if test "$?" = "0"; then
                {
                set `cat $TEMP`
                if false; then :
                elif test "$1" = "1"; then LIRC_DRIVER=livedrive_midi;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "2"; then LIRC_DRIVER=livedrive_seq;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "3"; then LIRC_DRIVER=creative_infracd;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "4"; then LIRC_DRIVER=bte;		DRIVER_PARAMETER=btty; DRIVER_PARAM_TYPE=none;
                elif test "$1" = "5"; then LIRC_DRIVER=devinput;	DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
                fi
                }
                else
                    return;
                fi;
        elif test "$1" = "a"; then LIRC_DRIVER=none;		DRIVER_PARAMETER=none; DRIVER_PARAM_TYPE=none;
        fi
        }
        else
            return;
        fi;
