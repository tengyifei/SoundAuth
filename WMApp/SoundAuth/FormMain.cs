using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Media;
using System.IO;

namespace SoundAuth
{
    public partial class FormMain : Form
    {
        string key = "HKEY_CURRENT_USER\\Auth\\2359072\\2347235";
        MemoryStream[] wav=new MemoryStream[16];
        MemoryStream fin_wav;
        System.Media.SoundPlayer player = new SoundPlayer();
        int[] pass = new int[32];
        uint[] auth = new uint[99];
        int[] circlex = new int[9];
        int[] circley = new int[9];
        uint authnum = 0;
        uint currnum = 0;
        static public Int32 counter = 0;
        static public Int64 message = 0;
        Graphics g;
        Graphics bg;
        Pen era_pen = new Pen(Color.Green, 14);
        Pen blk_pen = new Pen(Color.Black, 12);
        Pen wht_pen = new Pen(Color.White, 14);
        Pen red_pen = new Pen(Color.Red, 16);
        Rectangle[] auth_rects = new Rectangle[9];
        int prevX = 0;
        int prevY = 0;
        int cirwid = 55;//140;
        Bitmap buffer=new Bitmap(800,800);
        SolidBrush era_brush = new SolidBrush(Color.Green);
        bool authonly = false;
        
        public FormMain()
        {
            InitializeComponent();

            bg = Graphics.FromImage(buffer);
            g = this.CreateGraphics();

            bg.FillRectangle(new SolidBrush(Color.White), new Rectangle(0, 0, 800, 800));

            try
            {
                counter = ((Int32)Microsoft.Win32.Registry.GetValue(key, "Counter", new Int32()));
            }
            catch (NullReferenceException)
            {
                Microsoft.Win32.Registry.SetValue(key, "Counter", 0, Microsoft.Win32.RegistryValueKind.DWord);
                counter = ((Int32)Microsoft.Win32.Registry.GetValue(key, "Counter", new Int32()));
            }

            try
            {
                message = ((Int64)Microsoft.Win32.Registry.GetValue(key, "Message", new Int64()));
            }
            catch (NullReferenceException)
            {
                Microsoft.Win32.Registry.SetValue(key, "Message", 0, Microsoft.Win32.RegistryValueKind.QWord);
                message = ((Int64)Microsoft.Win32.Registry.GetValue(key, "Message", new Int64()));
            }

            for (int i = 0; i < 16; i++)
                wav[i] = new MemoryStream();

            wav[0].Write(SoundAuth.Properties.Resources._5, 0, SoundAuth.Properties.Resources._5.Length);
            wav[1].Write(SoundAuth.Properties.Resources._6, 0, SoundAuth.Properties.Resources._6.Length);
            wav[2].Write(SoundAuth.Properties.Resources._7, 0, SoundAuth.Properties.Resources._7.Length);
            wav[3].Write(SoundAuth.Properties.Resources._8, 0, SoundAuth.Properties.Resources._8.Length);
            wav[4].Write(SoundAuth.Properties.Resources._9, 0, SoundAuth.Properties.Resources._9.Length);
            wav[5].Write(SoundAuth.Properties.Resources._10, 0, SoundAuth.Properties.Resources._10.Length);
            wav[6].Write(SoundAuth.Properties.Resources._11, 0, SoundAuth.Properties.Resources._11.Length);
            wav[7].Write(SoundAuth.Properties.Resources._12, 0, SoundAuth.Properties.Resources._12.Length);
            wav[8].Write(SoundAuth.Properties.Resources._13, 0, SoundAuth.Properties.Resources._13.Length);
            wav[9].Write(SoundAuth.Properties.Resources._14, 0, SoundAuth.Properties.Resources._14.Length);
            wav[10].Write(SoundAuth.Properties.Resources._15, 0, SoundAuth.Properties.Resources._15.Length);
            wav[11].Write(SoundAuth.Properties.Resources._16, 0, SoundAuth.Properties.Resources._16.Length);
            wav[12].Write(SoundAuth.Properties.Resources._17, 0, SoundAuth.Properties.Resources._17.Length);
            wav[13].Write(SoundAuth.Properties.Resources._18, 0, SoundAuth.Properties.Resources._18.Length);
            wav[14].Write(SoundAuth.Properties.Resources._19, 0, SoundAuth.Properties.Resources._19.Length);
            wav[15].Write(SoundAuth.Properties.Resources._20, 0, SoundAuth.Properties.Resources._20.Length);

            for (int i = 0; i < 16; i++)
            {
                wav[i].Position = 0;
            }

            generate_pass();
        }

        public FormMain(bool getauth)
        {
            InitializeComponent();

            if (getauth) { authonly = true; button2.Visible = false; button1.Visible = false; mainMenu1.MenuItems.Remove(menuItem2); }

            bg = Graphics.FromImage(buffer);
            g = this.CreateGraphics();
            bg.FillRectangle(new SolidBrush(Color.White), new Rectangle(0, 0, 800, 800));
        }

        private void menuItem1_Click(object sender, EventArgs e)
        {
            if (authonly)
                this.Close();
            else
                Application.Exit();
        }

        private void menuItem2_Click(object sender, EventArgs e)
        {
            FormAdmin adm = new FormAdmin();
            adm.ShowDialog();
            g.Clear(Color.White);
            counter = ((Int32)Microsoft.Win32.Registry.GetValue(key, "Counter", new Int32()));
            message = ((Int64)Microsoft.Win32.Registry.GetValue(key, "Message", new Int64()));
        }

        private void produce(bool update)
        {
            fin_wav = new MemoryStream();
            fin_wav.Write(wav[0].GetBuffer(), 0, 44);
            /*fin_wav.GetBuffer()[40] = 0xA0;
            fin_wav.GetBuffer()[41] = 0x13;
            fin_wav.GetBuffer()[42] = 0x01;
            fin_wav.GetBuffer()[4] = 0xC4;
            fin_wav.GetBuffer()[5] = 0x13;
            fin_wav.GetBuffer()[6] = 0x01;*/

            fin_wav.GetBuffer()[40] = 0x00;
            fin_wav.GetBuffer()[41] = 0x58;
            fin_wav.GetBuffer()[42] = 0x02;
            fin_wav.GetBuffer()[4] = 0x24;
            fin_wav.GetBuffer()[5] = 0x58;
            fin_wav.GetBuffer()[6] = 0x02;

            for (int i = 0; i < 32; i++)
            {
                fin_wav.Write(wav[pass[i]].GetBuffer(), 44, 4800);
            }

            string resstring="";
            for (int i=0; i<32; i++){
                resstring+=Convert.ToString(pass[i], 16);
            }

            FileStream res_wav = new FileStream("res"+resstring+".wav", FileMode.CreateNew);
            fin_wav.WriteTo(res_wav);
            res_wav.Close();

            fin_wav.Position = 0;
            player.Stream = fin_wav;
            player.Play();
            if (update) counter++;
            Microsoft.Win32.Registry.SetValue(key, "Counter", counter, Microsoft.Win32.RegistryValueKind.DWord);
            generate_pass();
        }

        private void generate_pass()
        {
            System.Security.Cryptography.SHA1 hash = new System.Security.Cryptography.SHA1CryptoServiceProvider();
            byte[] data = new byte[12];
            int j = 0;

            Int64 bakmess = message;
            while (bakmess > 0)
            {
                data[j++] = Convert.ToByte((bakmess & 0xFF));
                bakmess >>= 8;
            }
            int bakcnt = counter;
            j = 8;
            while (bakcnt > 0)
            {
                data[j++] = Convert.ToByte((bakcnt & 0xFF));
                bakcnt >>= 8;
            }
            byte[] result = hash.ComputeHash(data);

            //Int64 pass_tmp = 0, pass_tmp_2 = 0;
            for (int i = 0; i < 16; i++)
            {
                pass[i*2+1] = (result[i] & 0xF);
                pass[i*2] = ((result[i] & 0xF0) >> 4);
            }

            /*for (int i = 0; i < 8; i++)
            {
                pass_tmp += (long)Math.Pow(0x100, i) * result[i];
            }

            for (int i = 8; i < 16; i++)
            {
                pass_tmp_2 += (long)Math.Pow(0x100, i-8) * result[i];
            }*/

        }

        private void button1_Click(object sender, EventArgs e)
        {
            produce(false);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int upwid = 30;//64;
            int gapwid = 30;
            if (authonly) upwid = 16;
            for (int i = 0; i < 3; i++)     //draw rows
            {
                for (int j = 0; j < 3; j++) //draw columns
                {
                    bg.DrawEllipse(blk_pen, new Rectangle(5 + (22 + cirwid) * i, upwid + (cirwid + gapwid) * j, cirwid, cirwid));
                    circlex[i * 3 + j] = 5 + (22 + cirwid) * i + cirwid / 2;
                    circley[i * 3 + j] = upwid + (cirwid + gapwid) * j + cirwid / 2;
                    auth_rects[i * 3 + j] = new Rectangle(5 + (22 + cirwid + 8) * i, upwid + (cirwid + 8 + gapwid) * j, cirwid - 8, cirwid - 8);
                }
            }
            g.DrawImage(buffer, 0, 0, Screen.PrimaryScreen.Bounds, GraphicsUnit.Pixel);
            timer1.Enabled = false;
        }

        private void FormMain_MouseMove(object sender, MouseEventArgs e)
        {
            if (timer1.Enabled) return;

            if (prevX!=0)
                bg.DrawLine(wht_pen, prevX, prevY, circlex[currnum], circley[currnum]);

            if (incircle(e.X, e.Y))
            {
                if (getcircle(e.X, e.Y) != currnum)
                {      //moved from one to another
                    currnum = getcircle(e.X, e.Y);
                    auth[authnum] = currnum;
                    authnum++;
                }
            }
            else
            {
                if (authnum == 0)
                {
                    return;
                }
            }

            for (int i = 0; i < 9; i++)
            {
                bg.DrawEllipse(blk_pen, new Rectangle(circlex[i] - cirwid / 2, circley[i] - cirwid / 2, cirwid, cirwid));
            }

            bg.DrawEllipse(red_pen, new Rectangle(circlex[currnum] - cirwid / 2, circley[currnum] - cirwid / 2, cirwid, cirwid));   //draw current circle
            
            bg.DrawLine(era_pen, e.X, e.Y, circlex[currnum], circley[currnum]);

            if (authnum > 1)
                bg.DrawLine(era_pen, circlex[auth[authnum-1]], circley[auth[authnum-1]], circlex[currnum], circley[currnum]);

            if (authnum > 1)
            {
                for (int i = 0; i < authnum - 1; i++)       //draw historic lines
                {
                    bg.DrawLine(era_pen, circlex[auth[i]], circley[auth[i]], circlex[auth[i+1]], circley[auth[i+1]]);
                }
            }

            for (int i = 0; i < authnum; i++)
            {
                bg.FillEllipse(era_brush, circlex[auth[i]] - 12, circley[auth[i]] - 12, 24, 24);       //draw hoitoric emphasis
            }

            bg.FillEllipse(era_brush, circlex[currnum] - 12, circley[currnum] - 12, 24, 24);       //draw current emphasis
            
            prevX = e.X; prevY = e.Y;
            g.DrawImage(buffer, 0, 0, Screen.PrimaryScreen.Bounds, GraphicsUnit.Pixel);
        }

        private void FormMain_MouseDown(object sender, MouseEventArgs e)
        {
            if (incircle(e.X, e.Y))
            {
                currnum = getcircle(e.X, e.Y);
                auth[0] = currnum;
                authnum++;
            }
        }

        private uint getcircle(int p, int p_2)
        {
            Point pt = new Point(p, p_2);
            for (uint i = 0; i < 9; i++)
            {
                if (auth_rects[i].Contains(pt))
                    return i;
            }
            return 0;
        }

        private bool incircle(int p, int p_2)
        {
            Point pt = new Point(p, p_2);
            for (int i = 0; i < 9; i++)
            {
                if (auth_rects[i].Contains(pt))
                    return true;
            }
            return false;
        }

        private void FormMain_MouseUp(object sender, MouseEventArgs e)
        {
            if (authonly)
            {                   //refresh authcircle
                UInt64 authcode = 0;
                for (uint i = 0; i < authnum; i++)
                {
                    authcode += auth[i] * (UInt64)Math.Pow(i+1, 15);
                }
                authcode += authnum * (UInt64)Math.Pow(10, 16);
                Microsoft.Win32.Registry.SetValue(key, "Auth", authcode, Microsoft.Win32.RegistryValueKind.QWord);
            }
            else
            {                   //check authcircle
                UInt64 authcode = 0;
                for (uint i = 0; i < authnum; i++)
                {
                    authcode += auth[i] * (UInt64)Math.Pow(i+1, 15);
                }
                authcode += authnum * (UInt64)Math.Pow(10, 16);
                UInt64 regauthcode = 0;
                regauthcode = (UInt64)(Int64)Microsoft.Win32.Registry.GetValue(key, "Auth", new Int64());
                if (regauthcode == authcode)
                {
                    produce(true);
                }
            }
            authnum = 0;
            currnum = 0;
            bg.Clear(Color.White);
            timer1.Enabled = true;
            prevX = 0;
            prevY = 0;
        }

        private void FormMain_GotFocus(object sender, EventArgs e)
        {
            timer1.Enabled = true;
        }

        private void FormMain_Closed(object sender, EventArgs e)
        {
            if (!authonly) Application.Exit();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            counter--;
            generate_pass();
        }
    }
}