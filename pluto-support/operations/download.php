<?
$out='
<p>Here are the precompiled binaries for this module, **package**.  Pluto contains many modules, so we do not recommend
installing the Pluto system by hand one module at a time.  If you want to a complete Pluto system, just login or register,
go to the My Pluto page and create a new installation.  This will install all the modules automatically.</p>
<p>The Pluto system is cross-platform.  This module can be downloaded for the following platforms.</p>
';

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>