call bldmake bldfiles
call abld build armi urel
call makesis PlutoMO.pkg
copy PlutoMO.sis Pluto_S60.sis
del PlutoMO.sis