ICEWM_CONFIG.txt
  - notes about Icewm config in Pluto

patch-icewm-<version>-<patch_description>.patch
  - various patches

icewm-<version>.orig
  - original version, kept for reference
  - unconfigured

icewm-<version>.patched
  - new version, patched, configured

configure examples:
cd icewm-<version>.patched/
./configure --prefix=/opt/icewm-1.2.21pre1 --with-cfgdir=/opt/icewm-1.2.21pre1/etc/icewm
./configure --prefix=/opt/icewm-1.2.26 --with-cfgdir=/opt/icewm-1.2.26/etc/icewm

EOF
