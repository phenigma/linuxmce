#!/bin/bash

echo 'UPDATE Orbiter set Modification_LastGen=0;' | mysql pluto_main || /bin/true
