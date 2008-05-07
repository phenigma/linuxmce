#!/bin/bash

# Desc : This script creates a sql script used to add the hardy release in the database.

. /usr/pluto/bin/SQL_Ops.sh

## Add new distro
Q="INSERT INTO Distro(Description, FK_OperatingSystem, Installer, SourceCode, Confirmed, Core, MediaDirector, Orbiter, InstallerURL) VALUES ('Ubuntu Hurdy', 1, 'ConfirmDependencies_Debian.sh', 'src', 1, 0, 0, 0, 'installUbuntu.php');"
echo "$Q"

## Modify non-pluto debs (replacements, ubuntu)
Q="
	SELECT
		FK_Package,
		Name,
		FK_RepositorySource,
		Repository,
		Parms,
		Replaces,
		Conflicts,
		Provides
	FROM
		Package_Source
	WHERE
		FK_RepositorySource IN (20)
		AND
		(Repository LIKE '%gutsy%' OR Repository LIKE '%replacements_ubuntu%')
"
R=$(RunSQL "$Q")

for Package_Source in $R ;do
	FK_Package=$(Field 1 "$Package_Source")
	Name=$(Field 2 "$Package_Source")
	FK_RepositorySource=$(Field 3 "$Package_Source")
	Repository=$(Field 4 "$Package_Source" | sed 's/gutsy/hardy/g')
	Parms=$(Field 5 "$Package_Source")
	Replaces=$(Field 6 "$Package_Source")
	Conflicts=$(Field 7 "$Package_Source")
	Provides=$(Field 8 "$Package_Source")

	Q="
-- Package $Name
INSERT INTO Package_Source (FK_Package, Name, FK_RepositorySource, Repository, Replaces, Conflicts, Provides) VALUES ('$FK_Package', '$Name', '$FK_RepositorySource', '$Repository', '$Replaces', '$Conflicts', '$Provides');
INSERT INTO Package_Source_Compat(FK_Package_Source, FK_OperatingSystem, FK_Distro, Comments) VALUES(LAST_INSERT_ID(), 1, 16, '');
"
	echo "$Q"
done

## Modify pluto-* debs
Q="
	SELECT
		PK_Package_Source,
		Name
	FROM
		Package_Source
	WHERE
		FK_RepositorySource IN (21)
"
R=$(RunSQL "$Q")

for Package_Source in $R ;do
	PK_Package_Source=$(Field 1 "$Package_Source")
	Name=$(Field 2 "$Package_Source")

	Q="	
-- Package $Name
INSERT INTO Package_Source_Compat(FK_Package_Source, FK_OperatingSystem, FK_Distro, Comments) VALUES ('$PK_Package_Source', 1, 16, '');
"
	echo "$Q"
done
