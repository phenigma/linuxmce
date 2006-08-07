#!/usr/bin/perl
# edited by VooDoo on 4 aug 2006 for new cd build
use diagnostics;

$debian_mirror="/home/mirrors";
$pluto_replacements="/home/samba/repositories/pluto/replacements";
$pluto_contrib="/home/samba/repositories/pluto/20dev/contrib";

# new vars - 6 aug 2006 - VooDoo
$cd1rmpkgsdir="/old-server/home/radu/pkg/bonuscd-cache/cd1remove/";
$cd1addpkgsdir="/old-server/home/radu/pkg/bonuscd-cache/cd1add/";
$replacementsrepo="/home/samba/repositories/pluto/replacements/main/binary-i386/";
$devrepo="/home/samba/repositories/pluto/20dev/main/binary-i386/";

$mail_dest='testing@plutohome.com -c linux-team@plutohome.com';

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
		`cp $1/${2}_$3.deb image-netinst/Debian-Cache`; 
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
			`rm image-netinst/Debian-Cache/$_` if ( ! /^$/ );
		} split (/[ \n]/, `cat $_`);
	} split (/[ \n]/, $_[0]);
}

die "You need to specify the build to create and (optionally) if you want to rebuild the cache.\n" if ( scalar @ARGV eq 0 ); 

$build_dir = $ARGV[0];

if ( defined $ARGV[1] && $ARGV[1] eq "cache" )
{
	`rm -rf "image-netinst/Debian-Cache"`;
	mkdir "image-netinst/Debian-Cache";

    # Place pluto cd boot logo
	`cp image-netinst/isolinux/pluto.rle image-netinst/isolinux/splash.rle`;

	build_desired_list `echo $build_dir/debian/*.list extra-packages.list kernels.list`;

#	explore_repository "$debian_mirror/Debian.ro"; 
#	explore_repository "$debian_mirror/MythTV";
#	explore_repository "$build_dir/debian";
#	explore_repository "$pluto_contrib";
#	explore_repository "$pluto_replacements";

#	remove_packages "exclude.list";

	# TODO: improve this part to do searches, walk dependencies
	`cp -v /old-server/home/radu/pkg/copy/*.deb image-netinst/Debian-Cache`;
	`cp -v /old-server/home/radu/pkg/missing/*.deb image-netinst/Debian-Cache`;
	`cp -v /home/builds/current/debian/main/binary-i386/*.deb image-netinst/Debian-Cache`;
	#`cp -v /home/samba/repositories/pluto/replacements/main/binary-i386/*.deb image-netinst/Debian-Cache`;
	`find /home/samba/repositories/pluto/replacements/main/binary-i386/ -type f -name '*.deb' -exec cp '{}' image-netinst/Debian-Cache ';'`;
	`cp -v /home/samba/repositories/pluto/20dev/contrib/binary-i386/*.deb image-netinst/Debian-Cache`;

	`find image-netinst/Debian-Cache '(' -name 'kernel-image*' -or -name 'kernel-source*' -or -name 'kernel-headers*' -or -name 'kernel-doc*' ')' -exec rm '{}' ';'`;
	`find image-netinst/Debian-Cache -name 'linux-image*' -not -name '*2.6.16.20-pluto-1*' -exec rm -f '{}' ';'`;
	`find image-netinst/Debian-Cache '(' -name 'linux-source*' -or -name 'linux-headers*' -or -name 'linux-doc*' ')' -exec rm '{}' ';'`;
	`find image-netinst/Debian-Cache -name 'pluto-src-*' -exec rm '{}' ';'`;
	`rm image-netints/Debian-Cache/kernel-*`;
	`rm image-netinst/Debian-Cache/pluto-skins-rse*`;
	`rm image-netinst/Debian-Cache/pluto-marbella*`;
	`rm image-netinst/Debian-Cache/pluto-audi*`;
	`rm image-netinst/Debian-Cache/pluto-sample-media*`;
	`rm image-netinst/Debian-Cache/monster*`;
	`rm image-netinst/Debian-Cache/pluto-monster-database*`;
	`rm image-netinst/Debian-Cache/pluto-skins-bo*`;
	`rm image-netinst/Debian-Cache/libwxgtk2.6-dbg*`;
	`rm image-netinst/Debian-Cache/libwxgtk2.6-dev*`;
	`rm image-netinst/Debian-Cache/python-wx*`;
	`rm image-netinst/Debian-Cache/slim*`;
	`rm image-netinst/Debian-Cache/pluto-slim-server*`;
	`rm image-netinst/Debian-Cache/pluto-slimserver-plugin*`;
	`rm image-netinst/Debian-Cache/video-wizard-videos*`;
	`find image-netinst/Debian-Cache '(' -name '*-dev_*.deb' -not -name '*xutils*' ')' -exec rm '{}' ';'`;
	#`rm image-netinst/Debian-Cache/*-dev_*.deb`;
	`rm image-netinst/Debian-Cache/*-dbg_*.deb`;
	`rm image-netinst/Debian-Cache/vdr*.deb`;
	`rm image-netinst/Debian-Cache/vlc*.deb`;
	`rm image-netinst/Debian-Cache/pluto-liblinphone*.deb`;
	`rm image-netinst/Debian-Cache/xfonts*-transcoded*.deb`;
	`rm image-netinst/Debian-Cache/xspecs*.deb`;
	`rm image-netinst/Debian-Cache/xvfb*.deb`;
	`rm image-netinst/Debian-Cache/xnest*.deb`;
	`rm image-netinst/Debian-Cache/xdmx*.deb`;
	`rm image-netinst/Debian-Cache/xlibmesa-dri*.deb`;
	`rm image-netinst/Debian-Cache/*gdb*.deb`;
	`rm image-netinst/Debian-Cache/nvidia-kernel-source*.deb`;
	`rm image-netinst/Debian-Cache/*-src_*.deb`;
	`rm image-netinst/Debian-Cache/*-doc_*.deb`;
	`rm image-netinst/Debian-Cache/wx2.6-doc*.deb`;
	`rm image-netinst/Debian-Cache/pluto-avwizard-sounds*`;
	`rm image-netinst/Debian-Cache/libsdl1.2debian-{arts,esd,nas,oss,all}*`;
	`rm image-netinst/Debian-Cache/wx-common*`;
	`rm image-netinst/Debian-Cache/wx2.6*`;
	`rm image-netinst/Debian-Cache/icewm-lite*`;
	`rm image-netinst/Debian-Cache/pluto-avwizard-skin*`;
	`rm image-netinst/Debian-Cache/bootsplash-theme-monster*`;
	`rm image-netinst/Debian-Cache/bootsplash-theme-linuxmce*`;
	`rm image-netinst/Debian-Cache/*-doc-*.deb`;
	`rm image-netinst/Debian-Cache/qt3-dev-tools*`;
	`rm image-netinst/Debian-Cache/libwxgtk2.6*`;

	# ugly, ugly, ugly, hack
	`rm image-netinst/Debian-Cache/pluto-bluetooth-dongle*`;

	# changes made on 6 aug 2006 by VooDoo
	# removing old packages from cd1
	system ("cd $cd1rmpkgsdir; for i in \$\(ls); do rm -rf /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache/\$i; done");
	# adding new packages to cd1
	system ("cd $cd1addpkgsdir; cp -r *.deb /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache");
	# add important packages for cd1 to work with offline installation
	`cp $replacementsrepo/pluto-sample-media*.deb /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache`;	
	`cp $replacementsrepo/avwizard-sounds-dummy*.deb /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache`;
	`cp $devrepo/pluto-avwizard-skin-basic*.deb /home/installation-cd-kernel-2.6.12/image-netinst/Debian-Cache`;		
	`cd /home/installation-cd-kernel-2.6.12`;	
	print "Making cached packages Indexes\n";
	print `dpkg-scanpackages image-netinst/Debian-Cache	/dev/null 2>/dev/null | sed s,image-netinst/Debian-Cache/,dists/sarge/main/binary-i386/,g | gzip -9c >image-netinst/Debian-Cache/Packages.gz`;
	
	`cd image-netinst/Debian-Cache; mkdir keep; zgrep '^Filename: ' Packages.gz|cut -d' ' -f2|cut -d/ -f5|xargs -ifile mv file keep; rm *.deb; mv keep/* .; rmdir keep`;
}

$version=`basename $build_dir`;

chomp $version;
print "Building startup CD for version $version\n";

print `sed 's/\%VERSION%/$version/g' templates/isolinux.txt.template > image-netinst/isolinux/isolinux.txt`;

#print "Updating installation scripts\n";
#`svn co http://10.0.0.170/pluto/trunk/src/ConfirmDependencies_Script InstallScripts`;
#`svn co http://10.0.0.170/pluto/trunk/src/ConfirmDependencies_Script_Offline InstallScripts_Offline`;
#`rm -rf image-netinst/Pluto-Install`;
#$files = "Common.sh ConfirmDependencies_Debian.sh Download_CVS.sh Download_Direct.sh Download_SVN.sh Initial_Config.sh Initial_Config_Real.sh pkgsel post-debootstrap.sh preseed.cfg Install_Assistance.sh";
#map { `cp InstallScripts/$_ image-netinst/Pluto-Install`; } split /[ \n]/, $files;

$files = "BonusCdMenu.sh BonusCdAutoInst.sh Common.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh pkgsel post-debootstrap.sh preseed.cfg";
#map { `cp InstallScripts_Offline/$_ image-netinst/Pluto-Install`; } split /[ \n]/, $files;
map { `cp /home/MakeRelease/trunk/src/ConfirmDependencies_Script_Offline/$_ image-netinst/Pluto-Install`; } split /[ \n]/, $files;

print "Processing udeb config";
print `apt-ftparchive generate templates/config-netinst-udeb`;

print "Processsing deb config";
print `apt-ftparchive generate templates/config-netinst-deb`;

print "Making md5sums\n"; 
`cd image-netinst; rm md5sum.txt; find -xtype f -exec md5sum \\{} \\; | awk '!/md5sum\\.txt/ && !/\\.\\/isolinux/' >>md5sum.txt`;

print "Building iso image $build_dir/installation-cd.iso\n";
`mkisofs -V "$version" -J -o "$build_dir/installation-cd-1-$version.pluto.iso" -r -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table image-netinst/`;

print "Building bonuscd (cd2) iso image";
`/bin/bash /home/installation-cd-kernel-2.6.12/build-bonuscd.sh pluto $version`;

if (-e "message.txt")
{
	`(echo "New ISO version: $version"; echo; cat message.txt) | mail -s "New ISO ready (+info)" $mail_dest`;
	`mv message.txt message.txt.prev`;
}
else
{
	`echo "New ISO version: $version"| mail -s "New ISO ready" $mail_dest`;
}
