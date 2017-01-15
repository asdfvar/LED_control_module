using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Collections.Concurrent;
using System.Threading;
using System.Windows.Forms;

namespace SolarsystemController
{
    interface ICommDelegate
    {
        // Valid message received, pass it up to whmoever wants to handle it
        void HandleRxMessage(Message m);

        // serial port failed for some reason.
        void RXError(String error);

        // you asked me to shut down and I'm finished.
        // will be called if an error causes us to shutdown.
        void Shutdown();
    }

    class Comm
    {
        private Thread me;
        private Message m = new Message();
        private SerialPort rx;
        private ICommDelegate Delegate;
        private bool PleaseStop;

        private void RXOne()
        {
            try {
                int b = rx.ReadByte();

                if (b == -1)
                {
                    throw new Exception("EOF on port!");
                }

                switch (m.RXByte((byte)b))
                {
                    case Message.MessageState.OK:
                        Delegate.HandleRxMessage(m);
                        m = new Message();
                        break;
                    case Message.MessageState.ERROR:
                        m.Reset();
                        break;
                    case Message.MessageState.INCOMPLETE:
                        break;
                }
            }
            catch (TimeoutException)
            {
                ;
            }
        }
        
        private void workProc()
        {
            try
            {
                while (rx.IsOpen && !PleaseStop)
                    RXOne();
            }
            catch (Exception ex)
            {
                Delegate.RXError("RX Error: " + ex.ToString());
            }
            Delegate.Shutdown();
        }

        internal Comm(SerialPort rx, ICommDelegate d)
        {
            Delegate = d;
            me = new Thread(new ThreadStart(workProc));
            me.IsBackground = true;
            this.rx = rx;
            rx.ReadTimeout = 1000;
            PleaseStop = false;
            me.Start();
        }

        internal void Shutdown()
        {
            PleaseStop = true;
        }

        internal void Send(Message m)
        {
            rx.Write(m.buf, 0, m.pos);
        }
    }
}
