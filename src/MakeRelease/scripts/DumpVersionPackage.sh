rm dumpvp.tar.gz
mysqldump --quote-names --allow-keywords --add-drop-table --tables pluto_main Distro Document Document_Comment InstallWizard InstallWizard_Distro Package PackageType Package_Compat Package_Directory Package_Directory_File Package_Package Package_Source Package_Source_Compat Package_Users Package_Version RepositorySource RepositorySource_URL RepositoryType Version > versionpackage.dump
tar zcvf dumpvp.tar.gz versionpackage.dump
