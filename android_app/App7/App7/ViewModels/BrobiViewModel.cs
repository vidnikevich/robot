using Sockets.Plugin;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Xamarin.Forms;

namespace App7.ViewModels
{
    public class BrobiViewModel : BaseViewModel
    {
        private UdpSocketClient _client = new UdpSocketClient();
        private int _port = 1000;
        byte _left = 0;
        byte _right = 0;
        byte _rightSpeed = 100;
        byte _leftSpeed = 100;
        byte _servoPosition = 90;
        byte _ledState = 0;

        private string address = "192.168.1.13";

        public BrobiViewModel()
        {
            Title = "БРОБИ";
            _client = new UdpSocketClient();



            ForwardCommand = new Command(async () =>
            {
                try
                {
                    var msg = "FORWARD   ";
                    var msgBytes = Encoding.UTF8.GetBytes(msg);
                    await _client.SendToAsync(msgBytes, address, _port);
                }
                catch (Exception e)
                {
                    var error = e;
                }

            });

            StopCommand = new Command(async () =>
            {
                try
                {
                    var msg = "STOP      ";
                    var msgBytes = Encoding.UTF8.GetBytes(msg);
                    await _client.SendToAsync(msgBytes, address, _port);
                }
                catch (Exception e)
                {
                    var error = e;
                }

            });
        }

        public ICommand ForwardCommand { get; private set; }

        public ICommand StopCommand { get; private set; }

        public byte Left
        {
            get { return _left; }
            set { SetProperty(ref _left, value, onChanged: () => { SendCommand(); }); }
        }

        public byte Right
        {
            get { return _right; }
            set { SetProperty(ref _right, value, onChanged: () => { SendCommand(); }); }
        }

        public byte LeftSpeed
        {
            get { return _leftSpeed; }
            set { SetProperty(ref _leftSpeed, value, onChanged: () => { SendCommand(); }); }
        }

        public byte RightSpeed
        {
            get { return _rightSpeed; }
            set { SetProperty(ref _rightSpeed, value, onChanged: () => { SendCommand(); }); }
        }

        public byte ServoPosition
        {
            get { return _servoPosition; }
            set { SetProperty(ref _servoPosition, value, onChanged: () => { SendCommand(); }); }
        }

        private bool _led;
        public bool Led
        {
            get { return _led; }
            set { SetProperty(ref _led, value, onChanged: () => { _ledState = (byte)(_led ? 1 : 0); SendCommand(); }); }
        }

        private bool needSendAgain = false;

        int count = 0;
        object _lock = new object();
        private async void SendCommand()
        {
            lock (_lock)
            {
                if (count > 1)
                {
                    needSendAgain = true;
                    return;
                }
                count++;
            }


            try
            {
                var cmd = new byte[] { (byte)'C', (byte)'M', (byte)'D', (byte)':', _left, _leftSpeed, _right, _rightSpeed, _servoPosition, _ledState };
                await _client.SendToAsync(cmd, address, _port);
                lock (_lock)
                {
                    if (needSendAgain)
                    {
                        needSendAgain = false;
                        count--;
                        SendCommand();
                    }
                }

            }
            catch (Exception e)
            {
                var error = e;
            }

            lock (_lock)
            {
                count--;
            }
        }
    }
}
