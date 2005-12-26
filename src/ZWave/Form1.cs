 using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace zwave1
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		Socket m_Socket,m_Socket_Listen;
		ControlThink.ZWave.Controller zc = new ControlThink.ZWave.Controller();
		Hashtable mapDevices = new Hashtable();
		ArrayList listThreads = new ArrayList();
		bool m_bQuit,m_bThreadRunning;

		private System.Windows.Forms.Button iterate;
		private System.Windows.Forms.TextBox tbDevices;
		private System.Windows.Forms.Button addDevice;
		private System.Windows.Forms.Button on;
		private System.Windows.Forms.Button recv;
		private System.Windows.Forms.Button remove;
		private System.Windows.Forms.Button reset;
		private System.Windows.Forms.TextBox nodeID;
		private System.Windows.Forms.Button off;
		private System.Windows.Forms.Button exit;
		private System.Windows.Forms.Button clear;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.iterate = new System.Windows.Forms.Button();
			this.tbDevices = new System.Windows.Forms.TextBox();
			this.addDevice = new System.Windows.Forms.Button();
			this.on = new System.Windows.Forms.Button();
			this.recv = new System.Windows.Forms.Button();
			this.remove = new System.Windows.Forms.Button();
			this.reset = new System.Windows.Forms.Button();
			this.nodeID = new System.Windows.Forms.TextBox();
			this.off = new System.Windows.Forms.Button();
			this.exit = new System.Windows.Forms.Button();
			this.clear = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// iterate
			// 
			this.iterate.Location = new System.Drawing.Point(104, 16);
			this.iterate.Name = "iterate";
			this.iterate.TabIndex = 1;
			this.iterate.Text = "iterate";
			this.iterate.Click += new System.EventHandler(this.iterate_Click);
			// 
			// tbDevices
			// 
			this.tbDevices.Location = new System.Drawing.Point(16, 208);
			this.tbDevices.Multiline = true;
			this.tbDevices.Name = "tbDevices";
			this.tbDevices.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbDevices.Size = new System.Drawing.Size(368, 416);
			this.tbDevices.TabIndex = 9;
			this.tbDevices.Text = "textBox1";
			// 
			// addDevice
			// 
			this.addDevice.Location = new System.Drawing.Point(184, 16);
			this.addDevice.Name = "addDevice";
			this.addDevice.TabIndex = 2;
			this.addDevice.Text = "add";
			this.addDevice.Click += new System.EventHandler(this.addDevice_Click);
			// 
			// on
			// 
			this.on.Location = new System.Drawing.Point(200, 104);
			this.on.Name = "on";
			this.on.TabIndex = 8;
			this.on.Text = "on";
			this.on.Click += new System.EventHandler(this.on_Click);
			// 
			// recv
			// 
			this.recv.Location = new System.Drawing.Point(200, 56);
			this.recv.Name = "recv";
			this.recv.TabIndex = 4;
			this.recv.Text = "recv";
			this.recv.Click += new System.EventHandler(this.recv_Click);
			// 
			// remove
			// 
			this.remove.Location = new System.Drawing.Point(96, 48);
			this.remove.Name = "remove";
			this.remove.TabIndex = 3;
			this.remove.Text = "remove";
			this.remove.Click += new System.EventHandler(this.remove_Click);
			// 
			// reset
			// 
			this.reset.Location = new System.Drawing.Point(112, 80);
			this.reset.Name = "reset";
			this.reset.TabIndex = 5;
			this.reset.Text = "reset";
			this.reset.Click += new System.EventHandler(this.reset_Click);
			// 
			// nodeID
			// 
			this.nodeID.Location = new System.Drawing.Point(8, 112);
			this.nodeID.Name = "nodeID";
			this.nodeID.TabIndex = 6;
			this.nodeID.Text = "textBox1";
			// 
			// off
			// 
			this.off.Location = new System.Drawing.Point(120, 112);
			this.off.Name = "off";
			this.off.TabIndex = 7;
			this.off.Text = "off";
			this.off.Click += new System.EventHandler(this.off_Click);
			// 
			// exit
			// 
			this.exit.Location = new System.Drawing.Point(200, 168);
			this.exit.Name = "exit";
			this.exit.TabIndex = 10;
			this.exit.Text = "EXIT";
			this.exit.Click += new System.EventHandler(this.exit_Click);
			// 
			// clear
			// 
			this.clear.Location = new System.Drawing.Point(32, 176);
			this.clear.Name = "clear";
			this.clear.TabIndex = 11;
			this.clear.Text = "clear";
			this.clear.Click += new System.EventHandler(this.clear_Click);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(392, 630);
			this.Controls.Add(this.clear);
			this.Controls.Add(this.exit);
			this.Controls.Add(this.off);
			this.Controls.Add(this.nodeID);
			this.Controls.Add(this.tbDevices);
			this.Controls.Add(this.reset);
			this.Controls.Add(this.remove);
			this.Controls.Add(this.recv);
			this.Controls.Add(this.on);
			this.Controls.Add(this.addDevice);
			this.Controls.Add(this.iterate);
			this.Name = "Form1";
			this.Tag = "connect";
			this.Text = "Form1";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.Form1_Closing);
			this.Load += new System.EventHandler(this.Form1_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void Form1_Load(object sender, System.EventArgs e)
		{
			ConnectAndLoadDevices();
			Thread t = new Thread(new ThreadStart(listen_thread));
			t.Start();
			listThreads.Add(t);
		}

		private bool ConnectAndLoadDevices()
		{
			bool bReopenedConnection=true;
			for(int iRetries=0;iRetries<5;++iRetries)
			{
				try
				{
					zc.Disconnect();
				}
				catch(Exception ex)
				{
				}
				try
				{
					zc.Connect("COM1");
				}
				catch(Exception ex)
				{
					zc.Disconnect();
					zc = new ControlThink.ZWave.Controller();
					bReopenedConnection=false;
					Thread.Sleep(3000);
				}
				if( bReopenedConnection )
					break;
			}

			if( !bReopenedConnection )
				return false;

			mapDevices.Clear();
			for(System.Int32 i = 0; i < zc.Devices.Count; i++) 
			{
				ControlThink.ZWave.Devices.ZWaveDevice device = zc.Devices.GetByIndex(i);
				if( device==null )
					continue;

				mapDevices[device.NodeID] = device;
			}
			return true;
		}
		public void listen_thread()
		{
			m_Socket_Listen = new Socket(AddressFamily.InterNetwork,SocketType.Stream,ProtocolType.Tcp);
			m_Socket_Listen.Bind(new IPEndPoint(0,3999));
			m_Socket_Listen.Listen(10);
			while(true)
			{
				try
				{
					m_Socket = m_Socket_Listen.Accept();
					tbDevices.Text = DateTime.Now.ToLongTimeString() + "Incoming connection\r\n" + tbDevices.Text;
					m_bQuit=true;
					while(m_bThreadRunning)
					{
						tbDevices.Text = DateTime.Now.ToLongTimeString() + "Waiting for thread to stop\r\n" + tbDevices.Text;
						Thread.Sleep(3000);
					}
					m_bQuit=false;
					m_bThreadRunning=true;
					Thread t = new Thread(new ThreadStart(process_thread));
						t.Start();
					listThreads.Add(t);
				}
				catch(Exception ex)
				{
				}
			}
		}

		private void process_thread()
		{
			string sBuffer = "";
			byte[] bytes = new byte[1024];
			try
			{
				while(!m_bQuit)
				{
					int num = m_Socket.Receive(bytes);
					for(int i=0;i<num;++i)
					{
						sBuffer += Convert.ToChar(bytes[i]);
						if( Convert.ToChar(bytes[i])=='\n' || Convert.ToChar(bytes[i])=='\r' )
						{
							tbDevices.Text = DateTime.Now.ToLongTimeString() + "------>BEFORE: " + sBuffer.Substring(0,sBuffer.Length-1) + "\r\n" + tbDevices.Text;
							ProcessIncomingData(sBuffer.Substring(0,sBuffer.Length-1));
							tbDevices.Text = DateTime.Now.ToLongTimeString() + "------>AFTER: " + sBuffer.Substring(0,sBuffer.Length-1) + "\r\n" + tbDevices.Text;
							sBuffer="";
						}
					}
				}
			}
			catch(Exception e)
			{
				tbDevices.Text = DateTime.Now.ToLongTimeString() + "***EX 2*** " + e.Message + "\r\n" + tbDevices.Text;
			}
			tbDevices.Text = DateTime.Now.ToLongTimeString() + "Thread exiting\r\n" + tbDevices.Text;
			m_bThreadRunning=false;
		}

		private void ProcessIncomingData(string sIncoming)
		{
			if( sIncoming.Length==0 )
				return;
			tbDevices.Text = DateTime.Now.ToLongTimeString() + " RECV: " + sIncoming + "\r\n" + tbDevices.Text;
			if( sIncoming=="DEVICES" )
			{
				string sResult = "";
				IDictionaryEnumerator myEnumerator = mapDevices.GetEnumerator();
				while ( myEnumerator.MoveNext() )
				{
					int PK_DeviceTemplate=0;
					ControlThink.ZWave.Devices.ZWaveDevice device = (ControlThink.ZWave.Devices.ZWaveDevice) myEnumerator.Value;
					switch(device.GetType().Name)
					{
						case "StaticController":
						case "Controller":
							continue;
						case "MultilevelSwitch":
							PK_DeviceTemplate=38;
							break;
						default:
							PK_DeviceTemplate=37;
							break;
					}
					// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
					sResult += myEnumerator.Key + "\t\t\t" + PK_DeviceTemplate.ToString() + "\t|";  // Use pipe's instead of CR since this is line based
				}

				m_Socket.Send(Encoding.ASCII.GetBytes(sResult + "\n"));
			}
			else if( sIncoming=="PING" )
				m_Socket.Send(Encoding.ASCII.GetBytes("PONG\n"));
			else if( sIncoming=="RECEIVECONFIG" )
			{
				bool bresult = false;
				try
				{
					bresult = zc.ReceiveConfiguration();
				}
				catch(Exception ex)
				{
					bresult=false;
				}
				try
				{
					zc.Disconnect();
				}
				catch(Exception ex)
				{
				}

				zc = new ControlThink.ZWave.Controller();
				if( ConnectAndLoadDevices()==false )
					m_Socket.Send(Encoding.ASCII.GetBytes("CANT RECONNECT\n"));
				else if( bresult )
					m_Socket.Send(Encoding.ASCII.GetBytes("OK\n"));
				else
					m_Socket.Send(Encoding.ASCII.GetBytes("FAILED TO RECEIVE\n"));
			}
			else if( sIncoming.IndexOf(' ')>=0 )
			{
				try
				{
					byte iNode = Convert.ToByte( sIncoming.Substring( sIncoming.IndexOf(' ')) );
					ControlThink.ZWave.Devices.ZWaveDevice device = (ControlThink.ZWave.Devices.ZWaveDevice) mapDevices[iNode];
					if( device==null )
						return;

					if( sIncoming.StartsWith("ON") )
					{
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " ON: " + device.NodeID.ToString() + "\r\n" + tbDevices.Text;
						device.PowerOn();
						m_Socket.Send(Encoding.ASCII.GetBytes("OK " + sIncoming + "\n"));
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " REPLIED WITH OK " + sIncoming + "\r\n" + tbDevices.Text;
					}
					else if( sIncoming.StartsWith("OFF") )
					{
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " OFF: " + device.NodeID.ToString() + "\r\n" + tbDevices.Text;
						device.PowerOff();
						m_Socket.Send(Encoding.ASCII.GetBytes("OK " + sIncoming + "\n"));
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " REPLIED WITH OK " + sIncoming + "\r\n" + tbDevices.Text;
					}
					else if( sIncoming.StartsWith("LEVEL") )
					{
						byte level = Convert.ToByte(sIncoming.Substring(5));
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " LEVEL: " + device.NodeID.ToString() + " " + level.ToString() + "\r\n" + tbDevices.Text;
						device.Level=level;
						m_Socket.Send(Encoding.ASCII.GetBytes("OK " + sIncoming + "\n"));
						tbDevices.Text = DateTime.Now.ToLongTimeString() + " REPLIED WITH OK " + sIncoming + "\r\n" + tbDevices.Text;
					}
				}
				catch(Exception e)
				{
					tbDevices.Text = DateTime.Now.ToLongTimeString() + " ****EX*** " + e.Message + "(" + sIncoming + ")\r\n" + tbDevices.Text;
					m_Socket.Send(Encoding.ASCII.GetBytes("ERROR " + e.Message + " " + sIncoming + "\n"));
				}
			}
			tbDevices.Text = DateTime.Now.ToLongTimeString() + " DONE: " + sIncoming + "\r\n" + tbDevices.Text;
		}

		private void iterate_Click(object sender, System.EventArgs e)
		{
			tbDevices.Text = ""	;
			for(System.Int32 i = 0; i < zc.Devices.Count; i++) 
			{
				ControlThink.ZWave.Devices.ZWaveDevice device = zc.Devices.GetByIndex(i);
				if( device==null )
					continue;

				tbDevices.Text = DateTime.Now.ToLongTimeString() + device.NodeID.ToString() + "," + tbDevices.Text;
			}
		}

		private void addDevice_Click(object sender, System.EventArgs e)
		{
			ControlThink.ZWave.Devices.ZWaveDevice device = zc.AddDevice();
			Console.WriteLine("added Node ID: " +
				device.NodeID.ToString());
		}

		private void on_Click(object sender, System.EventArgs e)
		{
			ControlThink.ZWave.Devices.ZWaveDevice device = zc.Devices.GetByNodeID(Convert.ToByte(this.nodeID.Text));
			try
			{
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " Sending ON\r\n" + tbDevices.Text;
				device.PowerOn();
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " ON OK\r\n" + tbDevices.Text;
			}
			catch(Exception ex)
			{
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " ON FAILED\r\n" + tbDevices.Text;
			}
		}

		private void recv_Click(object sender, System.EventArgs e)
		{
			bool bresult = zc.ReceiveConfiguration();
			zc.Disconnect();
			zc = new ControlThink.ZWave.Controller();
			ConnectAndLoadDevices();
			tbDevices.Text = bresult.ToString();
		}

		private void remove_Click(object sender, System.EventArgs e)
		{
			ControlThink.ZWave.Devices.ZWaveDevice device = zc.RemoveDevice();
			int k=2;
		}

		private void reset_Click(object sender, System.EventArgs e)
		{
			zc.HardReset();
			zc.Disconnect();
			zc = new ControlThink.ZWave.Controller();
			ConnectAndLoadDevices();
		}

		private void off_Click(object sender, System.EventArgs e)
		{
			ControlThink.ZWave.Devices.ZWaveDevice device = zc.Devices.GetByNodeID(Convert.ToByte(this.nodeID.Text));
			try
			{
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " SENDING OFF\r\n" + tbDevices.Text;
				device.PowerOff();
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " OFF OK\r\n" + tbDevices.Text;
			}
			catch(Exception ex)
			{
				tbDevices.Text = DateTime.Now.ToLongTimeString() + " OFF FAILED\r\n" + tbDevices.Text;
			}
		
		}

		private void exit_Click(object sender, System.EventArgs e)
		{
			Close();
		}

		private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			System.Collections.IEnumerator myEnumerator = listThreads.GetEnumerator();
			try
			{
				m_Socket_Listen.Close();
				m_Socket.Close();
			}
			catch(Exception ex)
			{
			}
			while ( myEnumerator.MoveNext() )
			{
				Thread t = (Thread) myEnumerator.Current;
				t.Abort();
			}
		}

		private void clear_Click(object sender, System.EventArgs e)
		{
			tbDevices.Text="";
		}
	}
}
