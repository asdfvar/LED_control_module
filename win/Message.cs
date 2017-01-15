using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace SolarsystemController
{
    class Message
    {
        internal const byte FRAME_START = 0xde;
        internal const byte FRAME_END = 0xad;

        internal const byte START_OFFSET = 0;
        internal const byte LEN_OFFSET = 1;
        internal const byte CMD_OFFSET = 2;
        internal const byte DATA_OFFSET = 3;

        internal enum MessageType : byte
        {
            CMD_START    = 0x01,
            CMD_VERSION  = 0x02,
            CMD_STOP     = 0xfe,
            CMD_NAK      = 0xff
        }

        internal enum MessageState
        {
            INCOMPLETE,
            ERROR,
            OK
        }

        internal int len;
        internal byte[] buf;
        internal int pos;

        internal Message(MessageType mtype) : this()
        {
            Push(FRAME_START);
            Push(0);
            Push((byte)mtype);
        }

        internal Message()
        {
            len = 256;
            buf = new byte[len];
            pos = 0;
        }

        internal void Reset()
        {
            pos = 0;
        }

        internal void Finish()
        {
            Push(FRAME_END);
            buf[LEN_OFFSET] = (byte)pos;
        }

        internal void Push(byte ch)
        {
            buf[pos++] = ch;
        }

        internal MessageState RXByte(byte ch)
        {
            if ((pos == 0) && (ch != FRAME_START))
                return MessageState.INCOMPLETE;
            buf[pos++] = ch;

            if ((pos < 3) || (buf[LEN_OFFSET] > pos))
                return MessageState.INCOMPLETE;

            if (buf[pos - 1] != FRAME_END)
                return MessageState.ERROR;

            return MessageState.OK;
        }

        public static Message CmdStartSession()
        {
            Message m = new Message(MessageType.CMD_START);
            m.Finish();
            return m;
        }

        public static Message CmdStopSession()
        {
            Message m = new Message(MessageType.CMD_STOP);
            m.Finish();
            return m;
        }

        public static Message CmdGetVersion()
        {
            Message m = new Message(MessageType.CMD_VERSION);
            m.Finish();
            return m;
        }
    }
}
