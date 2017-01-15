namespace SolarsystemController
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.refreshButton = new System.Windows.Forms.Button();
            this.connectButton = new System.Windows.Forms.Button();
            this.loadButton = new System.Windows.Forms.Button();
            this.writeButton = new System.Windows.Forms.Button();
            this.readButton = new System.Windows.Forms.Button();
            this.saveButton = new System.Windows.Forms.Button();
            this.serialSelectBox = new System.Windows.Forms.ComboBox();
            this.serialPortCombo = new System.Windows.Forms.ComboBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitButton = new System.Windows.Forms.ToolStripMenuItem();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.statusStrip.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel});
            this.statusStrip.Location = new System.Drawing.Point(0, 180);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(327, 22);
            this.statusStrip.TabIndex = 0;
            this.statusStrip.Text = "statusStrip1";
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(26, 17);
            this.statusLabel.Text = "Idle";
            // 
            // refreshButton
            // 
            this.refreshButton.Location = new System.Drawing.Point(227, 27);
            this.refreshButton.Name = "refreshButton";
            this.refreshButton.Size = new System.Drawing.Size(75, 23);
            this.refreshButton.TabIndex = 2;
            this.refreshButton.Text = "Refresh";
            this.refreshButton.UseVisualStyleBackColor = true;
            this.refreshButton.Click += new System.EventHandler(this.refreshButton_Click);
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(146, 27);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(75, 23);
            this.connectButton.TabIndex = 3;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // loadButton
            // 
            this.loadButton.Location = new System.Drawing.Point(27, 75);
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(116, 23);
            this.loadButton.TabIndex = 4;
            this.loadButton.Text = "Load File";
            this.loadButton.UseVisualStyleBackColor = true;
            this.loadButton.Click += new System.EventHandler(this.loadButton_Click);
            // 
            // writeButton
            // 
            this.writeButton.Location = new System.Drawing.Point(186, 115);
            this.writeButton.Name = "writeButton";
            this.writeButton.Size = new System.Drawing.Size(116, 23);
            this.writeButton.TabIndex = 5;
            this.writeButton.Text = "Write to Device";
            this.writeButton.UseVisualStyleBackColor = true;
            this.writeButton.Click += new System.EventHandler(this.writeButton_Click);
            // 
            // readButton
            // 
            this.readButton.Location = new System.Drawing.Point(186, 75);
            this.readButton.Name = "readButton";
            this.readButton.Size = new System.Drawing.Size(116, 23);
            this.readButton.TabIndex = 6;
            this.readButton.Text = "Read from Device";
            this.readButton.UseVisualStyleBackColor = true;
            this.readButton.Click += new System.EventHandler(this.readButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(27, 115);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(116, 23);
            this.saveButton.TabIndex = 7;
            this.saveButton.Text = "Save File";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // serialSelectBox
            // 
            this.serialSelectBox.FormattingEnabled = true;
            this.serialSelectBox.Location = new System.Drawing.Point(80, 26);
            this.serialSelectBox.Name = "serialSelectBox";
            this.serialSelectBox.Size = new System.Drawing.Size(121, 21);
            this.serialSelectBox.TabIndex = 1;
            // 
            // serialPortCombo
            // 
            this.serialPortCombo.FormattingEnabled = true;
            this.serialPortCombo.Location = new System.Drawing.Point(19, 28);
            this.serialPortCombo.Name = "serialPortCombo";
            this.serialPortCombo.Size = new System.Drawing.Size(121, 21);
            this.serialPortCombo.TabIndex = 8;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(327, 24);
            this.menuStrip1.TabIndex = 9;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitButton});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitButton
            // 
            this.exitButton.Name = "exitButton";
            this.exitButton.Size = new System.Drawing.Size(92, 22);
            this.exitButton.Text = "Exit";
            // 
            // serialPort
            // 
            this.serialPort.BaudRate = 115200;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(327, 202);
            this.Controls.Add(this.serialPortCombo);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.readButton);
            this.Controls.Add(this.writeButton);
            this.Controls.Add(this.loadButton);
            this.Controls.Add(this.connectButton);
            this.Controls.Add(this.refreshButton);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Solarsystem Controller";
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.Button refreshButton;
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.Button loadButton;
        private System.Windows.Forms.Button writeButton;
        private System.Windows.Forms.Button readButton;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.ComboBox serialSelectBox;
        private System.Windows.Forms.ComboBox serialPortCombo;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitButton;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
    }
}

