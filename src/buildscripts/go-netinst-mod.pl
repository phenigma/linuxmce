#!/usr/bin/perl

use diagnostics;

$debian_mirror="/home/mirrors";
$pluto_replacements="/home/samba/repositories/pluto/replacements";
$pluto_contrib="/home/samba/repositories/pluto/20dev/contrib";

%desired_packages = ();

sub build_desired_list
{
	map {
		chomp;
		print "Reading file in memory: $_\n";
		map {
			chomp;
			$desired_packages{$_} = 1;
		} split (/[ \n]/, `cat $_`);

		print "Total items: " . scalar keys (%desired_packages) . "\n";
	} split (/[ \n]/, $_[0]);
}

sub explore_repository
{
	print "Exploring $_[0]\n";

	my $all = 0;
	if (defined ($_[1]) && $_[1] eq "all")
	{
		$all = 1;
	}
	open(FILE_NAMES, "find $_[0] -name '*.deb' -follow |");
	
	while ( <FILE_NAMES> )
	{
		next if ( not /(.*)\/([^_\/]*)_([^\/]*)\.deb$/ );

		if ($all == 0)
		{
			next if ( not defined ( $desired_packages{$2} ))
		}
              
		print "$1/${2}_$3.deb\n"; 
		`cp $1/${2}_$3.deb image-netinst.mod/Debian-Cache`; 
    }
	
	close(FILE_NAMES);
}

sub remove_packages
{
	map {
		chomp;
		print "Processing exclude list: $_\n";
		map {
			chomp;
			print "Removing $_ from the image\n";
			`rm image-netinst.mod/Debian-Cache/$_` if ( ! /^$/ );
		} split (/[ \n]/, `cat $_`);
	} split (/[ \n]/, $_[0]);
}

die "You need to specify the build to create and (optionally) if you want to rebuild the cache.\n" if ( scalar @ARGV eq 0 ); 

$build_dir = $ARGV[0];

if ( defined $ARGV[1] && $ARGV[1] eq "cache" )
{
	`rm -rf "image-netinst.mod/Debian-Cache"`;
	mkdir "image-netinst.mod/Debian-Cache";

	build_desired_list `echo $build_dir/debian/*.list extra-packages.list kernels.list`;

#	explore_repository "$debian_mirror/Debian.ro"; 
#	explore_repository "$debian_mirror/MythTV";
#	explore_repository "$build_dir/debian";
#	explore_repository "$pluto_contrib";
#	explore_repository "$pluto_replacements";

#	remove_packages "exclude.list";

	# TODO: improve this part to do searches, walk dependencies
	`cp -v /old-server/home/radu/pkg/copy/*.deb image-netinst.mod/Debian-Cache`;
	`cp -v /old-server/home/radu/pkg/missing/*.deb image-netinst.mod/Debian-Cache`;
	`cp -v /home/builds/current/debian/main/binary-i386/*.deb image-netinst.mod/Debian-Cache`;
	`cp -v /home/samba/repositories/pluto/replacements/main/binary-i386/*.deb image-netinst.mod/Debian-Cache`;
	`cp -v /home/samba/repositories/pluto/20dev/contrib/binary-i386/*.deb image-netinst.mod/Debian-Cache`;

	`find image-netinst.mod/Debian-Cache -name 'kernel*' -not -name '*2.6.13.2*' -exec rm -f '{}' ';'`;
	`find image-netinst.mod/Debian-Cache '(' -name 'kernel-source*' -or -name 'kernel-headers*' -or -name 'kernel-doc*' ')' -exec rm '{}' ';'`;
	`find image-netinst.mod/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'`;
	`rm image-netinst.mod/Debian-Cache/pluto-skins-rse*`;
	`rm image-netinst.mod/Debian-Cache/pluto-marbella*`;
	`rm image-netinst.mod/Debian-Cache/pluto-audi*`;
	`rm image-netinst.mod/Debian-Cache/pluto-sample-media*`;
	`rm image-netinst.mod/Debian-Cache/monster-skin*`;

	print "Making cached packages Indexes\n";
	print `dpkg-scanpackages image-netinst.mod/Debian-Cache	/dev/null 2>/dev/null | sed s,image-netinst.mod/Debian-Cache/,dists/sarge/main/binary-i386/,g | gzip -9c >image-netinst.mod/Debian-Cache/Packages.gz`;
	
	`cd image-netinst.mod/Debian-Cache; mkdir keep; zgrep '^Filename: ' Packages.gz|cut -d' ' -f2|cut -d/ -f5|xargs -ifile mv file keep; rm *.deb; mv keep/* .; rmdir keep`;
}	

$version=`basename $build_dir`;

chomp $version;
print "Building startup CD for version $version\n";

print `sed 's/\%VERSION%/$version/g' templates/isolinux.txt.template > image-netinst.mod/isolinux/isolinux.txt`;

print "Updating installation scripts\n";
`svn co http://10.0.0.170/pluto/trunk/src/ConfirmDependencies_Script InstallScripts`;
`svn co http://10.0.0.170/pluto/trunk/src/ConfirmDependencies_Script_Offline InstallScripts_Offline`;
#`rm -rf image-netinst.mod/Pluto-Install`;
#$files = "Common.sh ConfirmDependencies_Debian.sh Download_CVS.sh Download_Direct.sh Download_SVN.sh Initial_Config.sh Initial_Config_Real.sh pkgsel post-debootstrap.sh preseed.cfg Install_Assistance.sh";
#map { `cp InstallScripts/$_ image-netinst.mod/Pluto-Install`; } split /[ \n]/, $files;
$files = "Common.sh ConfirmDependencies_Debian.sh Download_CVS.sh Download_Direct.sh Download_SVN.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh pkgsel post-debootstrap.sh preseed.cfg Install_Assistance.sh";
map { `cp InstallScripts_Offline/$_ image-netinst.mod/Pluto-Install`; } split /[ \n]/, $files;

print "Processing udeb config";
print `apt-ftparchive generate templates/config-netinst-udeb`;

print "Processsing deb config";
print `apt-ftparchive generate templates/config-netinst-deb`;

print "Making md5sums\n"; 
`cd image-netinst.mod; rm md5sum.txt; find -xtype f -exec md5sum \\{} \\; | awk '!/md5sum\\.txt/ && !/\\.\\/isolinux/' >>md5sum.txt`;

print "Building iso image $build_dir/installation-cd-mod.iso\n";
`mkisofs -V "$version" -J -o "$build_dir/installation-cd-mod.iso" -r -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table image-netinst.mod/`;

