using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace MultidumperGUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Font = SystemFonts.MessageBoxFont;
            splitContainer.SplitterDistance = splitContainer.Width / 2;

         

        }
     

        private void Form1_Resize(object sender, EventArgs e)
        {
            //splitContainer.SplitterDistance = splitContainer.Width / 2;

        }


        private void btnBrowse_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() != DialogResult.Cancel)
            {
                txtFileName.Text = openFileDialog1.FileName;

                loadFile();

            }


        }

        private void loadFile()
        {
            infoProcess.StartInfo = new ProcessStartInfo()
            {
                FileName = @"multidumper.exe",
                Arguments = $"\"{openFileDialog1.FileName}\" --json",
                UseShellExecute = false,
                RedirectStandardError = true,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            };

            infoProcess.Start();
            infoProcess.BeginOutputReadLine();
        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }
        Dictionary<int, double> progressDictionary;

        bool canceled;

        private void btnDump_Click(object sender, EventArgs e)
        {

            canceled = false;
            btnDump.Enabled = false;
            btnCancel.Enabled = true;
            mdProcess.StartInfo = new ProcessStartInfo()
            {
                FileName = @"multidumper.exe",
                Arguments = $"\"{openFileDialog1.FileName}\" {lstSubsongs.SelectedIndices[0]}",
                UseShellExecute = false,
                RedirectStandardError = true,
                RedirectStandardOutput = true,
                CreateNoWindow = true,

            };


            progressDictionary = new Dictionary<int, double>();

            string line;
            mdProcess.OutputDataReceived += new DataReceivedEventHandler((o, args) =>
            {
                line = args.Data;

                if (line == null)
                {
                    mdProcess.CancelOutputRead();
                     return;
                }

                if (line != "" && !canceled)
                {
                    var x = line.Split('|');
                    progressDictionary[Int32.Parse(x[0])] = Double.Parse(x[1]) / Double.Parse(x[2]);
                }
            });

            mdProcess.Exited += (o, args) =>
            {
                btnDump.Enabled = true;
                btnCancel.Enabled = false;
                pdWatcher.Enabled = false;

                lstChannels.Items.Clear();

                var x = 0;
                foreach (var channel in songInfo.Channels)
                {
                    var ls = new ListViewItem();
                    ls.Text = x.ToString();
                    ls.SubItems.Add(channel);
                    ls.SubItems.Add("");

                    lstChannels.Items.Add(ls);

                    x++;

                    chdVoiceName.Width = -1;

                }

                progressBar.Value = 0;

                if (!canceled) Process.Start("explorer",$"/select,\"{openFileDialog1.FileName}\"");


            };
            mdProcess.Start();
            mdProcess.PriorityClass = ProcessPriorityClass.BelowNormal;

            mdProcess.BeginOutputReadLine();
            pdWatcher.Enabled = true;




        }

        private void pdWatcher_Tick(object sender, EventArgs e)
        {
            lstChannels.BeginUpdate();
            //lstChannels.Items.Clear();

            var items = from pair in progressDictionary
                orderby pair.Key ascending
                select pair;

            foreach (var d in items)
            {
                
                lstChannels.Items[d.Key].Text = d.Key.ToString();
                lstChannels.Items[d.Key].SubItems[1].Text = songInfo.Channels[d.Key];
                lstChannels.Items[d.Key].SubItems[2].Text = d.Value.ToString("p");

                //lstChannels.Items.Add(ls);

                chdVoiceName.Width = -1;

            }

            lstChannels.EndUpdate();
            

            if (progressDictionary.Count != 0) { 

                var percentages = from pair in progressDictionary
                    select pair.Value;

                double avg = percentages.Aggregate<double, double>(0, (current, percentage) => current + percentage);
                avg = avg / percentages.Count();

                progressBar.Value = (int) Math.Round(avg*100000);
            }




        }

        private void infoProcess_Exited(object sender, EventArgs e)
        {

        }

        private MDOut songInfo;

        private void infoProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != "" || e.Data != null)
            {
                songInfo = JsonConvert.DeserializeObject<MDOut>(e.Data);
                infoProcess.CancelOutputRead();
                txtGame.Text = songInfo.Containerinfo.Game;
                txtSystem.Text = songInfo.Containerinfo.System;
                txtDumper.Text = songInfo.Containerinfo.Dumper;
                txtCopyright.Text = songInfo.Containerinfo.Copyright;

                lstChannels.Items.Clear();
                lstSubsongs.Items.Clear();
                lstChannels.BeginUpdate();
                lstSubsongs.BeginUpdate();
                var x = 0;
                foreach (var channel in songInfo.Channels)
                {
                    var ls= new  ListViewItem();
                    ls.Text = x.ToString();
                    ls.SubItems.Add(channel);
                    ls.SubItems.Add("");

                    lstChannels.Items.Add(ls);

                    x++;

                    chdVoiceName.Width = -1;

                }

                var y = 0;
                foreach (var song in songInfo.Songs)
                {
                    var ls = new ListViewItem();
                    ls.Text = y.ToString();
                    ls.SubItems.Add(song.Name);
                    ls.SubItems.Add(song.Author);

                    lstSubsongs.Items.Add(ls);

                    y++;

                    clhNumber.Width = -1;
                    clhName.Width = -1;
                    clhComposer.Width = -1;
                }
                lstChannels.EndUpdate();
                lstSubsongs.EndUpdate();

                lstSubsongs.Items[0].Selected = true;
            }
           

            


        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (!mdProcess.HasExited)
                {
                    if (MessageBox.Show("There is a dump in progress!\n\nAre you sure you want to exit?",
                            "Hey!",
                            MessageBoxButtons.YesNo,
                            MessageBoxIcon.Exclamation,
                            MessageBoxDefaultButton.Button2) == DialogResult.Yes)
                    {
                        mdProcess.Kill();
                    }
                    else
                    {
                        e.Cancel = true;
                    }
                }
            }
            catch (InvalidOperationException)
            {
                e.Cancel = false;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you want to cancel?",
                            "Hey!",
                            MessageBoxButtons.YesNo,
                            MessageBoxIcon.Question,
                            MessageBoxDefaultButton.Button2) == DialogResult.Yes)
            {
                canceled = true;
                mdProcess.Kill();
                btnDump.Enabled = true;
                btnCancel.Enabled = false;
                pdWatcher.Enabled = false;

                lstChannels.Items.Clear();

                var x = 0;
                foreach (var channel in songInfo.Channels)
                {
                    var ls = new ListViewItem();
                    ls.Text = x.ToString();
                    ls.SubItems.Add(channel);
                    ls.SubItems.Add("");

                    lstChannels.Items.Add(ls);

                    x++;

                    chdVoiceName.Width = -1;

                }

                progressBar.Value = 0;
            }
            
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {

            string[] compatible =
            {
                "ay", "gbs", "gym", "hes", "kss", "nsf",
                "nsfe", "sap", "sfm", "sgc", "spc", "vgm", "spu"
            };
            var thereAreFiles = e.Data.GetDataPresent(DataFormats.FileDrop);
            if (thereAreFiles)
            {
                var theFilesThatAreThere = (string[]) e.Data.GetData(DataFormats.FileDrop);

                
                    foreach (var s in compatible)
                    {
                        if (theFilesThatAreThere[0].ToLower().EndsWith(s))
                        {
                            e.Effect= DragDropEffects.Copy;
                        }
                    }
            }
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {         
            openFileDialog1.FileName = ((string[])e.Data.GetData(DataFormats.FileDrop))[0];
            txtFileName.Text = openFileDialog1.FileName;

            loadFile();
        }

        private void lstChannels_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

      
    }
}
