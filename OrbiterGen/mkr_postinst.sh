# hardcoded
OrbiterDev=1
SkinDir=/usr/pluto/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto

export SDL_VIDEODRIVER=dummy
/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost
/usr/pluto/bin/OrbiterGen -d 27 -g "$SkinDir" -f "$FontDir" -o "$OutDir" -h localhost
