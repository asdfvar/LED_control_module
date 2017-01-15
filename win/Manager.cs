using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;

namespace SolarsystemController
{
    interface IManagerDelegate
    {
        // Manager calls this to update status .. so you can see whats going on
        void Status(string value);

        // Had a problem talking to the box (you should shutdown now)
        void Error(string value);

        // Connected! Awesome! You can now do other stuff with us.
        void Connected();

        // Shutdown! Yay! We are finished cleaning up.
        void Shutdown();
    }
    
    class Manager : ICommDelegate
    {
        // How long do we wait for the remote device to respond before deciding it isn't there?
        const int CommTimeout = 1500;

        Timer timeout;
        IManagerDelegate Delegate;
        SerialPort port;
        Comm comm;
        bool connected;
        bool idle;

        private void TimerCallback(object state)
        {
            Delegate.Error("comm timeout");
            Shutdown();
        }

        private void FrobWatchdog()
        {
            if (timeout == null)
            {
                TimerCallback tcb = TimerCallback;
                timeout = new Timer(tcb, null, CommTimeout, Timeout.Infinite);
            }
            else if (connected && idle)
            {
                comm.Send(Message.CmdGetVersion());
            }
            timeout.Change(CommTimeout, Timeout.Infinite);
        }

        public Manager(SerialPort port, IManagerDelegate Delegate)
        {
            this.port = port;
            this.Delegate = Delegate;
            this.comm = new Comm(port, this);
            this.connected = false;
            this.idle = false;

            FrobWatchdog();

            comm.Send(Message.CmdStartSession());
        }

        public void Shutdown()
        {
            if (connected)
                comm.Send(Message.CmdStopSession());
            comm.Shutdown();
        }







        void ICommDelegate.HandleRxMessage(Message m)
        {
            if (!this.connected)
            {
                this.connected = true;
                Delegate.Connected();
            }
            this.idle = true;
            FrobWatchdog();
        }

        void ICommDelegate.RXError(String err)
        {
            Delegate.Error(err);
        }

        void ICommDelegate.Shutdown()
        {
            if (timeout != null)
                timeout.Dispose();
            connected = false;
            Delegate.Shutdown();
        }
    }
}
