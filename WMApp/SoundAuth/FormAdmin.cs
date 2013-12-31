using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SoundAuth
{
    public partial class FormAdmin : Form
    {
        string key = "HKEY_CURRENT_USER\\Auth\\2359072\\2347235";
        public FormAdmin()
        {
            InitializeComponent();
            textBox2.Text = Convert.ToString((Int32)Microsoft.Win32.Registry.GetValue(key, "Counter", 0));
            textBox1.Text = Convert.ToString((Int64)Microsoft.Win32.Registry.GetValue(key, "Message", (Int64)0));
        }

        private void menuItem1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Microsoft.Win32.Registry.SetValue(key, "Counter", Convert.ToInt32(textBox2.Text), Microsoft.Win32.RegistryValueKind.DWord);
            FormMain.counter = Convert.ToInt32(textBox2.Text);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Microsoft.Win32.Registry.SetValue(key, "Message", Convert.ToInt64(textBox1.Text), Microsoft.Win32.RegistryValueKind.QWord);
            FormMain.message = Convert.ToInt64(textBox1.Text);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            FormMain authm = new FormMain(true);
            authm.ShowDialog();
        }
    }
}