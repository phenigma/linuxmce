using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using HAData.Common;
using HAData.DataAccess;
using Criteria;


namespace HADesigner
{
	/// <summary>
	/// Summary description for CriteriaForm.
	/// </summary>
	public class CriteriaForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.TextBox tbDefine;
		private System.Windows.Forms.TextBox tbDescription;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btnUp;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnAddNestingParm;
		private System.Windows.Forms.Button btnNot;
		private System.Windows.Forms.Button btnDown;
		private System.Windows.Forms.Button btnOr;
		private System.Windows.Forms.Button btnAnd;
		private System.Windows.Forms.TextBox tbText;
		private System.Windows.Forms.ComboBox cbValue;
		private System.Windows.Forms.ComboBox cbOperator;
		private System.Windows.Forms.ComboBox cbCriteria;
		private System.Windows.Forms.Button Add;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;


		private CriteriaList objCriteria;



		public MyDataSet mds
		{
			get{ return HADataConfiguration.m_mdsCache; }
		}	



		public CriteriaForm(int intCriteria_DID)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		
			//todo 
			intCriteria_DID = 32;

			Criteria_DDataRow drCriteria = this.mds.tCriteria_D[intCriteria_DID];
			switch (drCriteria.fFK_CriteriaList)
			{
				case CriteriaListData.CONTROLLER_CONST:
				{
					break;
				}
				case CriteriaListData.EVENT_CONST:
				{
					objCriteria = new EventCriteria_DList(3);
					objCriteria.LoadFromDatabase(intCriteria_DID);
					break;
				}
			}
			
			UpdateCriteriaOptions();
			updateOperatorsAndValues();

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}




		private void UpdateCriteriaOptions()
		{
			cbCriteria.DataSource = objCriteria.alCriteriaParmList;
			cbCriteria.DisplayMember = "Name";
			cbCriteria.ValueMember = "ID";

		}


		private void updateOperatorsAndValues()
		{
			
			cbOperator.Items.Clear();

			cbOperator.Items.Add(Operator.String(Operator.iEquals));
			cbOperator.Items.Add(Operator.String(Operator.iNotEquals));

			//CriteriaParmList c = m_CriteriaList.GetFromID(Convert.ToInt32(cbCriteria.SelectedValue));

			CriteriaParmList c = (CriteriaParmList) cbCriteria.SelectedItem;

			if( c.m_bAllowText )
			{
				if( c.m_bAllowGLComparisson )
				{
					cbOperator.Items.Add(Operator.String(Operator.iGreaterThan));
					cbOperator.Items.Add(Operator.String(Operator.iLessThan));
				}
				// todo cbOperator.LimitToList=false;

			}
			cbOperator.SelectedIndex = 0;
			// else
			// todo cbOperator.LimitToList=true;

			cbValue.DataSource = c.m_alCriteriaValues;
			cbValue.DisplayMember = "Name";
			cbValue.ValueMember = "ID";
		}

		private void UpdateView()
		{
			DisplayLine oNextLine;

			int iCount;

			string sOut = "";
			objCriteria.BuildLines();
			iCount = -1;
			foreach (DisplayLine oLine in objCriteria.alLines)
			{
				iCount++;
				oNextLine = (DisplayLine) oLine;

				//if(oNextLine.bSelected)
				if(iCount == objCriteria.iSelectedLineNumber)
				{
					sOut += "   ->\t";

					//enable or disable buttons depending on what type of line it is.
					if (oNextLine.iType == DisplayLineType.Criteria)
					{
						btnEdit.Enabled = true;
					}
					else
					{
						btnEdit.Enabled = false;
					}

				}
				else
				{
					sOut += "\t";
				}

				sOut += oNextLine.sText + "\r\n";
			}
			tbText.Text = sOut;
			
			//tbResult.Text = objCriteria.Evaluate().ToString();
		}


		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.btnSave = new System.Windows.Forms.Button();
			this.tbDefine = new System.Windows.Forms.TextBox();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.btnUp = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnAddNestingParm = new System.Windows.Forms.Button();
			this.btnNot = new System.Windows.Forms.Button();
			this.btnDown = new System.Windows.Forms.Button();
			this.btnOr = new System.Windows.Forms.Button();
			this.btnAnd = new System.Windows.Forms.Button();
			this.tbText = new System.Windows.Forms.TextBox();
			this.cbValue = new System.Windows.Forms.ComboBox();
			this.cbOperator = new System.Windows.Forms.ComboBox();
			this.cbCriteria = new System.Windows.Forms.ComboBox();
			this.Add = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(456, 424);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(104, 24);
			this.btnSave.TabIndex = 40;
			this.btnSave.Text = "Save";
			// 
			// tbDefine
			// 
			this.tbDefine.Location = new System.Drawing.Point(304, 424);
			this.tbDefine.Name = "tbDefine";
			this.tbDefine.TabIndex = 39;
			this.tbDefine.Text = "";
			// 
			// tbDescription
			// 
			this.tbDescription.Location = new System.Drawing.Point(88, 424);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(160, 20);
			this.tbDescription.TabIndex = 38;
			this.tbDescription.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(264, 424);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(40, 16);
			this.label2.TabIndex = 37;
			this.label2.Text = "Define:";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 424);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(64, 16);
			this.label1.TabIndex = 36;
			this.label1.Text = "Description:";
			// 
			// btnUp
			// 
			this.btnUp.Location = new System.Drawing.Point(456, 336);
			this.btnUp.Name = "btnUp";
			this.btnUp.TabIndex = 35;
			this.btnUp.Text = "Up";
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(312, 336);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.TabIndex = 34;
			this.btnDelete.Text = "Delete";
			// 
			// btnAddNestingParm
			// 
			this.btnAddNestingParm.Location = new System.Drawing.Point(112, 368);
			this.btnAddNestingParm.Name = "btnAddNestingParm";
			this.btnAddNestingParm.Size = new System.Drawing.Size(136, 23);
			this.btnAddNestingParm.TabIndex = 33;
			this.btnAddNestingParm.Text = "Add Sub Parenthesis";
			// 
			// btnNot
			// 
			this.btnNot.Location = new System.Drawing.Point(24, 368);
			this.btnNot.Name = "btnNot";
			this.btnNot.TabIndex = 32;
			this.btnNot.Text = "Not";
			// 
			// btnDown
			// 
			this.btnDown.Location = new System.Drawing.Point(456, 368);
			this.btnDown.Name = "btnDown";
			this.btnDown.TabIndex = 31;
			this.btnDown.Text = "Down";
			// 
			// btnOr
			// 
			this.btnOr.Location = new System.Drawing.Point(112, 336);
			this.btnOr.Name = "btnOr";
			this.btnOr.TabIndex = 30;
			this.btnOr.Text = "Or";
			// 
			// btnAnd
			// 
			this.btnAnd.Location = new System.Drawing.Point(24, 336);
			this.btnAnd.Name = "btnAnd";
			this.btnAnd.TabIndex = 29;
			this.btnAnd.Text = "And";
			// 
			// tbText
			// 
			this.tbText.Location = new System.Drawing.Point(16, 48);
			this.tbText.Multiline = true;
			this.tbText.Name = "tbText";
			this.tbText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbText.Size = new System.Drawing.Size(592, 272);
			this.tbText.TabIndex = 28;
			this.tbText.Text = "";
			// 
			// cbValue
			// 
			this.cbValue.Location = new System.Drawing.Point(304, 16);
			this.cbValue.Name = "cbValue";
			this.cbValue.Size = new System.Drawing.Size(216, 21);
			this.cbValue.TabIndex = 27;
			// 
			// cbOperator
			// 
			this.cbOperator.Location = new System.Drawing.Point(216, 16);
			this.cbOperator.Name = "cbOperator";
			this.cbOperator.Size = new System.Drawing.Size(80, 21);
			this.cbOperator.TabIndex = 26;
			// 
			// cbCriteria
			// 
			this.cbCriteria.Location = new System.Drawing.Point(16, 16);
			this.cbCriteria.Name = "cbCriteria";
			this.cbCriteria.Size = new System.Drawing.Size(192, 21);
			this.cbCriteria.TabIndex = 25;
			this.cbCriteria.SelectedIndexChanged += new System.EventHandler(this.cbCriteria_SelectedIndexChanged);
			// 
			// Add
			// 
			this.Add.Location = new System.Drawing.Point(528, 16);
			this.Add.Name = "Add";
			this.Add.TabIndex = 24;
			this.Add.Text = "Add";
			this.Add.Click += new System.EventHandler(this.Add_Click);
			// 
			// CriteriaForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(632, 470);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.btnSave,
																		  this.tbDefine,
																		  this.tbDescription,
																		  this.label2,
																		  this.label1,
																		  this.btnUp,
																		  this.btnDelete,
																		  this.btnAddNestingParm,
																		  this.btnNot,
																		  this.btnDown,
																		  this.btnOr,
																		  this.btnAnd,
																		  this.tbText,
																		  this.cbValue,
																		  this.cbOperator,
																		  this.cbCriteria,
																		  this.Add});
			this.Name = "CriteriaForm";
			this.Text = "CriteriaForm";
			this.ResumeLayout(false);

		}
		#endregion

		private void cbCriteria_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			updateOperatorsAndValues();
		}

		private void Add_Click(object sender, System.EventArgs e)
		{
			/*
			CriteriaParmList c = (CriteriaParmList) cbCriteria.SelectedItem;
			CriteriaParm cp = new CriteriaParm(c,cbOperator.SelectedIndex+1,cbValue.SelectedValue.ToString());
			cp.m_ParentNestingParameter = m_CriteriaList.SelectedNestingParameter;
			m_CriteriaList.SelectedNestingParameter.alCriteriaParm.Add(cp);
			UpdateView();

			//goto the new line number.  bummer that we have to call UpdateView again :(  This could be better
			selectLineNumber(cp.m_DisplayLine.iLineNumber);
			UpdateView();
			*/
		}
	}
}
