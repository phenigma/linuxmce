echo "<?xml version=\"1.0\"?>
<Default>
<callManagerGroup>
<members>
<member priority=\"0\">
<callManager>
<ports>
<ethernetPhonePort>2000</ethernetPhonePort>
</ports>
<processNodeName>192.168.80.1</processNodeName>
</callManager>
</member>
</members>
</callManagerGroup>
<servicesURL>http://192.168.80.1/pluto-admin/ServicesMenu.php</servicesURL>
</Default>" > /tftpboot/XmlDefault.cnf.xml

