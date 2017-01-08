namespace MultidumperGUI
{
    partial class Form1
    {
        /// <summary>
        /// Variable del diseñador necesaria.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén usando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben desechar; false en caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido de este método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.txtFileName = new System.Windows.Forms.TextBox();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lstSubsongs = new System.Windows.Forms.ListView();
            this.clhNumber = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.clhName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.clhComposer = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.txtCopyright = new System.Windows.Forms.TextBox();
            this.txtDumper = new System.Windows.Forms.TextBox();
            this.txtSystem = new System.Windows.Forms.TextBox();
            this.txtGame = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lblName = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lstChannels = new System.Windows.Forms.ListView();
            this.chdNumber = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chdVoiceName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chdProgress = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btnDump = new System.Windows.Forms.Button();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.mdProcess = new System.Diagnostics.Process();
            this.pdWatcher = new System.Windows.Forms.Timer(this.components);
            this.infoProcess = new System.Diagnostics.Process();
            this.btnCancel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "File name: ";
            // 
            // txtFileName
            // 
            this.txtFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFileName.BackColor = System.Drawing.SystemColors.Window;
            this.txtFileName.Location = new System.Drawing.Point(70, 12);
            this.txtFileName.Name = "txtFileName";
            this.txtFileName.ReadOnly = true;
            this.txtFileName.Size = new System.Drawing.Size(575, 21);
            this.txtFileName.TabIndex = 1;
            // 
            // btnBrowse
            // 
            this.btnBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnBrowse.Location = new System.Drawing.Point(651, 12);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(53, 21);
            this.btnBrowse.TabIndex = 2;
            this.btnBrowse.Text = "Browse";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "Supported formats|*.ay;*.gbs;*.gym;*.hes;*.kss;*.nsf;*.nsfe;*.sap;*.sfm;*.sgc;*.s" +
    "pc;*.vgm";
            this.openFileDialog1.SupportMultiDottedExtensions = true;
            // 
            // splitContainer
            // 
            this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer.Location = new System.Drawing.Point(13, 39);
            this.splitContainer.Name = "splitContainer";
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer.Size = new System.Drawing.Size(691, 423);
            this.splitContainer.SplitterDistance = 349;
            this.splitContainer.SplitterWidth = 5;
            this.splitContainer.TabIndex = 3;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lstSubsongs);
            this.groupBox1.Controls.Add(this.txtCopyright);
            this.groupBox1.Controls.Add(this.txtDumper);
            this.groupBox1.Controls.Add(this.txtSystem);
            this.groupBox1.Controls.Add(this.txtGame);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.lblName);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(349, 423);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Info and subsongs";
            this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
            // 
            // lstSubsongs
            // 
            this.lstSubsongs.Activation = System.Windows.Forms.ItemActivation.OneClick;
            this.lstSubsongs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lstSubsongs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clhNumber,
            this.clhName,
            this.clhComposer});
            this.lstSubsongs.FullRowSelect = true;
            this.lstSubsongs.HideSelection = false;
            this.lstSubsongs.Location = new System.Drawing.Point(11, 186);
            this.lstSubsongs.MultiSelect = false;
            this.lstSubsongs.Name = "lstSubsongs";
            this.lstSubsongs.Size = new System.Drawing.Size(332, 231);
            this.lstSubsongs.TabIndex = 2;
            this.lstSubsongs.UseCompatibleStateImageBehavior = false;
            this.lstSubsongs.View = System.Windows.Forms.View.Details;
            // 
            // clhNumber
            // 
            this.clhNumber.Text = "ID";
            this.clhNumber.Width = 30;
            // 
            // clhName
            // 
            this.clhName.Text = "Title";
            this.clhName.Width = 125;
            // 
            // clhComposer
            // 
            this.clhComposer.Text = "Author";
            this.clhComposer.Width = 125;
            // 
            // txtCopyright
            // 
            this.txtCopyright.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtCopyright.Location = new System.Drawing.Point(10, 146);
            this.txtCopyright.Name = "txtCopyright";
            this.txtCopyright.ReadOnly = true;
            this.txtCopyright.Size = new System.Drawing.Size(333, 21);
            this.txtCopyright.TabIndex = 1;
            // 
            // txtDumper
            // 
            this.txtDumper.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtDumper.Location = new System.Drawing.Point(10, 109);
            this.txtDumper.Name = "txtDumper";
            this.txtDumper.ReadOnly = true;
            this.txtDumper.Size = new System.Drawing.Size(333, 21);
            this.txtDumper.TabIndex = 1;
            // 
            // txtSystem
            // 
            this.txtSystem.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSystem.Location = new System.Drawing.Point(10, 71);
            this.txtSystem.Name = "txtSystem";
            this.txtSystem.ReadOnly = true;
            this.txtSystem.Size = new System.Drawing.Size(333, 21);
            this.txtSystem.TabIndex = 1;
            // 
            // txtGame
            // 
            this.txtGame.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtGame.Location = new System.Drawing.Point(10, 34);
            this.txtGame.Name = "txtGame";
            this.txtGame.ReadOnly = true;
            this.txtGame.Size = new System.Drawing.Size(333, 21);
            this.txtGame.TabIndex = 1;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(8, 170);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "Subsongs";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 130);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(54, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "Copyright";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 93);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(44, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Dumper";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 56);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(42, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "System";
            // 
            // lblName
            // 
            this.lblName.AutoSize = true;
            this.lblName.Location = new System.Drawing.Point(7, 19);
            this.lblName.Name = "lblName";
            this.lblName.Size = new System.Drawing.Size(34, 13);
            this.lblName.TabIndex = 0;
            this.lblName.Text = "Game";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lstChannels);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(337, 423);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Channels";
            // 
            // lstChannels
            // 
            this.lstChannels.Activation = System.Windows.Forms.ItemActivation.OneClick;
            this.lstChannels.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chdNumber,
            this.chdVoiceName,
            this.chdProgress});
            this.lstChannels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstChannels.FullRowSelect = true;
            this.lstChannels.Location = new System.Drawing.Point(3, 17);
            this.lstChannels.MultiSelect = false;
            this.lstChannels.Name = "lstChannels";
            this.lstChannels.Size = new System.Drawing.Size(331, 403);
            this.lstChannels.TabIndex = 0;
            this.lstChannels.UseCompatibleStateImageBehavior = false;
            this.lstChannels.View = System.Windows.Forms.View.Details;
            // 
            // chdNumber
            // 
            this.chdNumber.Text = "ID";
            this.chdNumber.Width = 30;
            // 
            // chdVoiceName
            // 
            this.chdVoiceName.Text = "Name";
            this.chdVoiceName.Width = 125;
            // 
            // chdProgress
            // 
            this.chdProgress.Text = "Progress";
            this.chdProgress.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // btnDump
            // 
            this.btnDump.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnDump.Location = new System.Drawing.Point(594, 471);
            this.btnDump.Name = "btnDump";
            this.btnDump.Size = new System.Drawing.Size(51, 21);
            this.btnDump.TabIndex = 4;
            this.btnDump.Text = "Dump";
            this.btnDump.UseVisualStyleBackColor = true;
            this.btnDump.Click += new System.EventHandler(this.btnDump_Click);
            // 
            // progressBar
            // 
            this.progressBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.progressBar.Location = new System.Drawing.Point(13, 471);
            this.progressBar.Maximum = 100000;
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(575, 21);
            this.progressBar.TabIndex = 5;
            // 
            // mdProcess
            // 
            this.mdProcess.EnableRaisingEvents = true;
            this.mdProcess.StartInfo.Domain = "";
            this.mdProcess.StartInfo.LoadUserProfile = false;
            this.mdProcess.StartInfo.Password = null;
            this.mdProcess.StartInfo.StandardErrorEncoding = null;
            this.mdProcess.StartInfo.StandardOutputEncoding = null;
            this.mdProcess.StartInfo.UserName = "";
            this.mdProcess.SynchronizingObject = this;
            // 
            // pdWatcher
            // 
            this.pdWatcher.Interval = 125;
            this.pdWatcher.Tick += new System.EventHandler(this.pdWatcher_Tick);
            // 
            // infoProcess
            // 
            this.infoProcess.EnableRaisingEvents = true;
            this.infoProcess.StartInfo.Domain = "";
            this.infoProcess.StartInfo.LoadUserProfile = false;
            this.infoProcess.StartInfo.Password = null;
            this.infoProcess.StartInfo.StandardErrorEncoding = null;
            this.infoProcess.StartInfo.StandardOutputEncoding = null;
            this.infoProcess.StartInfo.UserName = "";
            this.infoProcess.SynchronizingObject = this;
            this.infoProcess.OutputDataReceived += new System.Diagnostics.DataReceivedEventHandler(this.infoProcess_OutputDataReceived);
            this.infoProcess.Exited += new System.EventHandler(this.infoProcess_Exited);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Enabled = false;
            this.btnCancel.Location = new System.Drawing.Point(651, 471);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(51, 21);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // Form1
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(714, 504);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnDump);
            this.Controls.Add(this.splitContainer);
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.txtFileName);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Multidumper by Christopher Snowhill (kode54), GUI by KDDLB";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.Form1_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.Form1_DragEnter);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
            this.splitContainer.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtFileName;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnDump;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.TextBox txtCopyright;
        private System.Windows.Forms.TextBox txtDumper;
        private System.Windows.Forms.TextBox txtSystem;
        private System.Windows.Forms.TextBox txtGame;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lblName;
        private System.Diagnostics.Process mdProcess;
        private System.Windows.Forms.Timer pdWatcher;
        private System.Diagnostics.Process infoProcess;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ListView lstChannels;
        private System.Windows.Forms.ColumnHeader chdNumber;
        private System.Windows.Forms.ColumnHeader chdVoiceName;
        private System.Windows.Forms.ColumnHeader chdProgress;
        private System.Windows.Forms.ListView lstSubsongs;
        private System.Windows.Forms.ColumnHeader clhNumber;
        private System.Windows.Forms.ColumnHeader clhName;
        private System.Windows.Forms.ColumnHeader clhComposer;
        private System.Windows.Forms.Button btnCancel;
    }
}

