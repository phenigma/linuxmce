call bldmake bldfiles
call abld build armi urel
call makesis PlutoBT.pkg
del PlutoMO.SIS
ren PlutoBT.SIS PlutoMO.SIS
