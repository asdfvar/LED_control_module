using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace SolarsystemController
{
    public partial class Form1 : Form, IManagerDelegate
    {
        private List<String> serialPortList;
        private Manager DeviceManager;

        private void refreshSerialPort()
        {
            serialPortList = new List<String>(SerialPort.GetPortNames());
            serialPortCombo.DataSource = serialPortList;
        }

        public Form1()
        {
            InitializeComponent();
            refreshSerialPort();
            statusLabel.Text = "Idle";

        }

        delegate void setTextCallback(string value);

        public void SetStatus(string value)
        {
            if (statusStrip.InvokeRequired)
            {
                setTextCallback d = new setTextCallback(SetStatus);
                statusStrip.Invoke(d, new object[] { value });
            }
            else
            {
                statusLabel.Text = value;
            }
        }

        
        private void writeButton_Click(object sender, EventArgs e)
        {

        }

        private void closePort()
        {
            DeviceManager = null;
            serialPort.Close();
            serialPortCombo.Enabled = true;
            refreshButton.Enabled = true;
            connectButton.Enabled = true;
            connectButton.Text = "Connect";
        }

        private void startConnecting()
        {
            String port = (String)serialPortCombo.SelectedItem;

            try
            {
                SetStatus("connecting ... ");
                serialPort.PortName = port;
                serialPort.Open();
                DeviceManager = new Manager(serialPort, this);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Can't open " + port + ": " + ex.ToString());
                DeviceManager = null;
                return;
            }

            serialPortCombo.Enabled = false;
            refreshButton.Enabled = false;
            connectButton.Enabled = false;
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (serialPort.IsOpen)
            {
                SetStatus("disconnecting ... ");
                DeviceManager.Shutdown();
                connectButton.Enabled = false;
            }
            else
            {
                startConnecting();
            }
        }

        private void refreshButton_Click(object sender, EventArgs e)
        {
            refreshSerialPort();
        }

        private void saveButton_Click(object sender, EventArgs e)
        {

        }

        private void readButton_Click(object sender, EventArgs e)
        {

        }

        private void loadButton_Click(object sender, EventArgs e)
        {

        }










        delegate void ConnectedCallback();

        void IManagerDelegate.Connected()
        {
            IManagerDelegate wtf = this;

            if (statusStrip.InvokeRequired)
            {
                ConnectedCallback d = new ConnectedCallback(wtf.Connected);
                statusStrip.Invoke(d);
            }
            else
            {
                connectButton.Enabled = true;
                connectButton.Text = "Disconnect";
                SetStatus("connected !! ");
            }
        }

        void IManagerDelegate.Status(string value)
        {
            SetStatus(value);
        }

        void IManagerDelegate.Error(string value)
        {
            SetStatus(value);
        }

        delegate void ShutdownCallback();

        void IManagerDelegate.Shutdown()
        {
            if (statusStrip.InvokeRequired)
            {
                IManagerDelegate wtf = this;

                ShutdownCallback d = new ShutdownCallback(wtf.Shutdown);
                statusStrip.Invoke(d);
            }
            else
            {
                closePort();
            }
        }
    }
}
