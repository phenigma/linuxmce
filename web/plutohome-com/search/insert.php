<?
 include("../include/config/database.inc.php");
$sql="insert into PageContent(PageContenttext,PageName) values(' Video Conferencing Camera
      Video Conferencing Microphone<br>
      IP Telephone
      DVD/CD Jukebox
      8 port Gigabit Ethernet Switch
      Wireless Access Point (wi-fi) with built-in 4 port 10/100 Switch
      Interface Module 
      The Interface Modules offer the flexibility of controlling virtually any device anywhere. It has RS232, Infrared outputs, Relays, and sensor inputs. The Interface Modules communicate with your Pluto system via an Ethernet connection. So you can put one anywhere, whether or not you have other Pluto equipment in that location. 
      If, for example, you have a room with some tv and stereo equipment that you want to control, you only need to put an interface module in that room. Plug it into the wall jack, and then stick IR emitters on your equipment. <br>
        Many sub-systems, like lighting, alarm panels, and so on, communicate with an RS232 port. If these devices are near the Core, you can plug them in directly to the Core`s RS232 port. But, if they`re not, just plug an interface module into the nearest Ethernet jack, and then plug the device into the Interface Module`s RS232 port.</p>
      <p>Each of the IR emitter jacks is also a TTL input. This means you can hook up sensors, like motion detectors or contact switches, and other devices like intercoms and push buttons. The Pluto Core will fire an event whenever one of the inputs changes, and using the web interface, you can easily tell the Core how you want it to respond. For example, if you hook a doorbell to a sensor input, the Core can flash lights in addition to sounding the bell when the doorbell is pushed.</p>
      <p><strong>TOAD devices</strong><br>
      The sensors can also be used to make it easier to turn TOAD devices. For an explanation of TOAD devices vs. discrete inputs, click here. There are AC sensors and Video sensors that determine if a piece of equipment is on, based on whether or not it is drawing power or outputting a video signal. This would allow Pluto to determine whether or not the tv is already on, for example, so that it knows whether to send a power command to it. Refer to the aforementioned link if you do not understand what this means.</p>
      <p><strong>Capacities</strong><br>
        The Interface Module is available in 3 sizes: 6&quot;, which has 1 RS232 port and 3 Infrared emitters/TTL inputs. 12&quot;, which has 2 RS232 ports, 3 Relay Outputs, and 6 Infrared emitters/TTL inputs. And 18&quot;, which comes standard with the same inputs/outputs as the 12&quot; module, but has room to add several more modules and can be rack-mounted.</p></td>
  </tr>','accesories.php')";
  $r=mysql_query($sql) or die("dfsdf".mysql_error());
  ?>