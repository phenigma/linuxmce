using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;
using HAData.Common;
using HAData.DataAccess;
using System.Data;
using System.Diagnostics;


namespace HADesigner
{
	/// <summary>
	/// Manage DesignObjVariation_DesignObj_Skin_Language table
	/// </summary>
	public class UIChildSkinLanguage: UI
	{
		public static int NoSetValue = -2;
		public int m_FK_SkinID = -1;
		public int m_FK_LanguageID = -1;

		//MEMBER VARIABLES
		private string graphicsDirectory;
		public string GraphicsDirectory
		{
			get {return this.graphicsDirectory;}
			set {this.graphicsDirectory = value;}
		}

		private string m_strDescription;
		private sbyte m_intPriority;
		private bool m_blnAnimate;
		private int m_intUIDesignObjType;
		private int m_intDesignObjCategoryID;
		private ArrayList m_alUIDesignObjVariations = new ArrayList();
		private UIDesignObj m_objParentUIDesignObj;
		private bool m_blnCantGoBack;


		private bool m_blnMainBackgroundDrawn = false;


		//TODO: this will change when we have multiple select capability
		private UIDesignObj m_UIDesignObjSelected = null;
		
		

		//things that can change
		private string m_strDescriptionOriginal;
		private sbyte m_intPriorityOriginal;
		private bool m_blnAnimateOriginal;
		private int m_intUIDesignObjTypeOriginal;
		private int m_intDesignObjCategoryIDOriginal;
		private bool m_blnCantGoBackOriginal;
		//the following change, but are handled in UIDesignObjVariation object
		private bool m_bitRegenerateForEachScreenOriginal;
		private bool m_bitCanBeHiddenOriginal;
		private bool m_bitHideByDefaultOriginal;
		private bool m_bitChildBehindBGOriginal;
		private bool m_bitChildBehindBG;
		private bool m_blnBGOnTopOriginal;
		private bool m_blnBGOnTop;
		private bool m_blnDontMergeBGOriginal;
		private bool m_blnDontMergeBG;
		private bool m_blnChildBeforeTextOriginal;
		private bool m_blnChildBeforeText;
		private bool m_blnIsTabStopOriginal;
		private bool m_blnIsTabStop;
		private string m_sVisibleStates;
		private string m_sVisibleStatesOriginal;
		private string m_iTiedToOriginal;
		private string m_iTiedTo;
		private string m_iTS_UpOriginal;
		private string m_iTS_Up;
		private string m_iTS_DownOriginal;
		private string m_iTS_Down;
		private string m_iTS_LeftOriginal;
		private string m_iTS_Left;
		private string m_iTS_RightOriginal;
		private string m_iTS_Right;

		
		private int m_intWidthOriginal;
		private int m_intHeightOriginal;
		private int m_intParentXOriginal;		
		private int m_intParentYOriginal;
		private int m_intParentDisplayOrderOriginal;




		//used for database updates
		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnNeedsDBParentVariationUnlink = false;
		private bool m_blnNeedsDBParentVariationLink = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete
		private bool m_blnParentVariationLinked = false;


		private bool m_blnShow = true;


		private bool m_blnSelected = false;

		//used for drawing
		private int m_intWidth = NoSetValue;
		private int m_intHeight = NoSetValue;
		private float m_fltScale;
		private bool m_bitRegenerateForEachScreen;
		private bool m_bitCanBeHidden;
		private bool m_bitDontResetSelectedState;
		private bool m_bitHideByDefault;
		private int m_intParentX;		//relative to parent UIDesignObjVariation
		private int m_intParentY;
		private int m_intParentDisplayOrder;
		private int m_intRootX;			//relative to root container where drawing takes place
		private int m_intRootY;

		private int m_intLinkID;	//this is the id in the objectvariation_oject table

		//PROPERTIES
		public string DescriptionOnly
		{
			get {return this.m_strDescription;}
			set {this.m_strDescription = value;}
		}

		public int SkinID
		{
			get {return this.m_FK_SkinID;}
			set {this.m_FK_SkinID = value;}
		}


		public int LanguageID
		{
			get {return this.m_FK_LanguageID;}
			set {this.m_FK_LanguageID = value;}
		}

		public string Description
		{
			get
			{
				if (this.m_strDescription == null) return "";
				else return ID + " " + m_strDescription + (m_bitHideByDefault ? " h" : "");
			}
			set	{m_strDescription = value;}
		}
		public bool CantGoBack
		{
			get {return m_blnCantGoBack; }
			set {m_blnCantGoBack = value; }
		}
		public sbyte Priority
		{
			get {return this.m_intPriority;}
			set {this.m_intPriority = value;}
		}
		public bool Animate
		{
			get {return this.m_blnAnimate;}
			set {this.m_blnAnimate = value;}
		}
		public override string ToString()
		{
			return this.Description;
		}
		public int UIDesignObjType
		{
			get	{return m_intUIDesignObjType;}
			set	{m_intUIDesignObjType = value;}
		}

		public int DesignObjCategory
		{
			get	{return m_intDesignObjCategoryID;}
			set	{m_intDesignObjCategoryID = value;}
		}

		public ArrayList UIDesignObjVariations
		{
			get {return m_alUIDesignObjVariations;}
			set {m_alUIDesignObjVariations = value;}
		}

		public UIDesignObj ParentUIDesignObj
		{
			get {return m_objParentUIDesignObj;}
			set	{m_objParentUIDesignObj = value;}
		}

		public int LinkID
		{
			get	{return m_intLinkID;}
			set	{m_intLinkID = value;}
		}


		public UIDesignObj SelectedDesignObj
		{
			get {return m_UIDesignObjSelected;}
		}

		
		public int Width
		{
			get	{return m_intWidth;}
			set {m_intWidth = value;}
		}
		public int Height
		{
			get	{return m_intHeight;}
			set {m_intHeight = value;}
		}
		public float Scale
		{
			get	{return m_fltScale;}
			set {m_fltScale = value;}
		}
		public bool CanBeHidden
		{
			get {return this.m_bitCanBeHidden;}
			set {this.m_bitCanBeHidden = value;}
		}
		public bool HideByDefault
		{
			get {return this.m_bitHideByDefault;}
			set {this.m_bitHideByDefault = value;}
		}
		public bool ChildBehindBG
		{
			get {return this.m_bitChildBehindBG;}
			set {this.m_bitChildBehindBG = value;}
		}
		public bool ChildBehindBGOriginal
		{
			get {return this.m_bitChildBehindBGOriginal;}
			set {this.m_bitChildBehindBGOriginal = value;}
		}
		public bool RegenerateForEachScreen
		{
			get {return this.m_bitRegenerateForEachScreen;}
			set {this.m_bitRegenerateForEachScreen = value;}
		}
		public bool DontResetSelectedState
		{
			get {return this.m_bitDontResetSelectedState;}
			set {this.m_bitDontResetSelectedState = value;}
		}
		public int ParentX
		{
			get	{return m_intParentX;}
			set {m_intParentX = value;}
		}
		public int ParentY
		{
			get	{return m_intParentY;}
			set {m_intParentY = value;}
		}
		public int ParentDisplayOrder
		{
			get	{return m_intParentDisplayOrder;}
			set	{m_intParentDisplayOrder = value;}
		}
		public int RootX
		{
			get	{return m_intRootX;}
			set {m_intRootX = value;}
		}
		public int RootY
		{
			get	{return m_intRootY;}
			set {m_intRootY = value;}
		}

		public bool CanBeHiddenOriginal
		{
			get {return this.m_bitCanBeHiddenOriginal;}
			set {this.m_bitCanBeHiddenOriginal = value;}
		}
		public bool HideByDefaultOriginal
		{
			get {return this.m_bitHideByDefaultOriginal;}
			set {this.m_bitHideByDefaultOriginal = value;}
		}

		public bool BGOnTopOriginal
		{
			get { return m_blnBGOnTopOriginal; }
			set { m_blnBGOnTopOriginal = value; }
		}
		public bool BGOnTop
		{
			get { return m_blnBGOnTop; }
			set { m_blnBGOnTop = value; }
		}
		public bool ChildBeforeText
		{
			get { return m_blnChildBeforeText; }
			set { m_blnChildBeforeText = value; }
		}
		public bool ChildBeforeTextOriginal
		{
			get { return m_blnChildBeforeTextOriginal; }
			set { m_blnChildBeforeTextOriginal = value; }
		}
		public bool DontMergeBG
		{
			get { return m_blnDontMergeBG; }
			set { m_blnDontMergeBG = value; }
		}
		public bool DontMergeBGOriginal
		{
			get { return m_blnDontMergeBGOriginal; }
			set { m_blnDontMergeBGOriginal = value; }
		}
		public bool IsTabStop
		{
			get { return m_blnIsTabStop; }
			set { m_blnIsTabStop = value; }
		}
		public bool IsTabStopOriginal
		{
			get { return m_blnIsTabStopOriginal; }
			set { m_blnIsTabStopOriginal = value; }
		}
		public string TS_Up
		{
			get { return m_iTS_Up; }
			set { m_iTS_Up = value; }
		}
		public string TS_Down
		{
			get { return m_iTS_Down; }
			set { m_iTS_Down = value; }
		}
		public string TS_Left
		{
			get { return m_iTS_Left; }
			set { m_iTS_Left = value; }
		}
		public string TS_Right
		{
			get { return m_iTS_Right; }
			set { m_iTS_Right = value; }
		}
		public string TiedTo
		{
			get { return m_iTiedTo; }
			set { m_iTiedTo = value; }
		}
		public string VisibleStates
		{
			get { return m_sVisibleStates; }
			set { m_sVisibleStates = value; }
		}
		public bool RegenerateForEachScreenOriginal
		{
			get {return this.m_bitRegenerateForEachScreenOriginal;}
			set {this.m_bitRegenerateForEachScreenOriginal = value;}
		}

		public int WidthOriginal
		{
			get	{return	m_intWidthOriginal;}
			set	{m_intWidthOriginal	= value;}
		}
		public int HeightOriginal
		{
			get	{return	m_intHeightOriginal;}
			set	{m_intHeightOriginal = value;}
		}
		public int ParentXOriginal
		{
			get	{return	m_intParentXOriginal;}
			set	{m_intParentXOriginal =	value;}
		}
		public int ParentYOriginal
		{
			get	{return m_intParentYOriginal;}
			set {m_intParentYOriginal = value;}
		}

		public int ParentDisplayOrderOriginal
		{
			get	{return m_intParentDisplayOrderOriginal;}
			set	{m_intParentDisplayOrderOriginal = value;}
		}

		


		public UIChildSkinLanguage(UIDesignObj objParentUIDesignObj, int intID, 
			int SkinID,  int LanguageID, string graphicsDir)
		{
			this.graphicsDirectory = graphicsDir;

			m_objParentUIDesignObj = objParentUIDesignObj;
			ID = intID;		

			m_FK_SkinID = SkinID;
			m_FK_LanguageID = LanguageID;

			if(objParentUIDesignObj == null)
			{
				this.m_bitCanBeHidden = false;
				this.m_bitHideByDefault = false;
				this.m_bitRegenerateForEachScreen = false;
				m_intParentX = 0;
				m_intParentY = 0;
				this.ParentVariationLinked = false;
			}
			else
			{
				this.ParentVariationLinked = true;
			}

			if(intID != -1)
			{
				LoadFromDatabase();
			}

			if(objParentUIDesignObj == null)
			{
				Build(1, true); // Start with default skin
				this.ResetLinkOriginals();
			}	
		}
        
		public override bool OriginalsChanged
		{
			get	{return true;}
		}

		public override void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
	
			DesignObjVariation_DesignObj_Skin_LanguageDataRow dr = mds.tDesignObjVariation_DesignObj_Skin_Language[ID];


			this.ParentDisplayOrder = dr.fDisplayOrder;
			this.RootX = dr.fX;
			this.RootY = dr.fY;			
			this.ParentX = dr.fX;
			this.ParentY = dr.fY;
			this.Width = dr.fWidth;
			this.Height = dr.fHeight;


			CanBeHidden = dr.fCanBeHidden;
			HideByDefault = dr.fHideByDefault;
			ChildBehindBG = dr.fDisplayChildrenBehindBackground;

			TS_Up = dr.fFK_DesignObj_UpIsNull ? "" : dr.fFK_DesignObj_Up.ToString();
			TS_Down = dr.fFK_DesignObj_DownIsNull ? "" : dr.fFK_DesignObj_Down.ToString();
			TS_Right = dr.fFK_DesignObj_RightIsNull ? "" : dr.fFK_DesignObj_Right.ToString();
			TS_Left = dr.fFK_DesignObj_RightIsNull ? "" : dr.fFK_DesignObj_Left.ToString();
			TiedTo = dr.fsFK_DesignObj_TiedTo;
			VisibleStates = dr.fVisibleStates;
			ChildBeforeText = dr.fDisplayChildrenBeforeText;
			DontMergeBG = dr.fDontMergeBackground;
			IsTabStop = dr.fIsTabStop;
			//DontResetSelectedState = dr.f;
			RegenerateForEachScreen = dr.fRegenerateForEachScreen;

/*
FK_DesignObj_InsteadOf                         int(11)        YES             (NULL)                   select,insert,update,references
VisibleStates                                  varchar(10)    YES             (NULL)                   select,insert,update,references
Ignore      
*/																														

			//set the originals so we know how and when to save
			this.ResetOriginals();

			m_blnNeedsDBInsert = false;
			m_blnNeedsDBDelete = false;
			
		}

		public override bool SaveToDatabase()
		{
			return false;
		}

		protected override void ResetOriginals()
		{
			
		}

		public void ResetLinkOriginals()
		{
			this.CanBeHiddenOriginal = this.CanBeHidden;
			this.HideByDefaultOriginal = this.HideByDefault;

			this.m_sVisibleStatesOriginal = this.m_sVisibleStates;
			this.m_iTiedToOriginal = this.m_iTiedTo;
			this.m_iTS_UpOriginal = this.m_iTS_Up;
			this.m_iTS_DownOriginal = this.m_iTS_Down;
			this.m_iTS_LeftOriginal = this.m_iTS_Left;
			this.m_iTS_RightOriginal = this.m_iTS_Right;
			this.BGOnTopOriginal = this.BGOnTop;
			this.RegenerateForEachScreenOriginal = this.RegenerateForEachScreen;
			this.ParentXOriginal = this.ParentX;
			this.ParentYOriginal = this.ParentY;
			this.WidthOriginal = this.Width;
			this.HeightOriginal = this.Height;
			this.ParentDisplayOrderOriginal = this.ParentDisplayOrder;
		}


		public void Build(int SkinID)
		{
			this.Build(SkinID, false);
		}

		public void Build(int SkinID, bool SkinChanged)
		{
			if(this.Include)	//don't include if deleted or set to be deleted or unlinked
			{
				//do the build top down stuff
				//like positioning
				//find the root x and y
				if(this.ParentUIDesignObj == null)
				{
					this.RootX = 0;
					this.RootY = 0;
				}
				else
				{
					//TODO Ender
					//this.RootX = this.ParentX + this.ParentUIDesignObj.ParentUIDesignObj.RootX;
					//this.RootY = this.ParentY + this.ParentUIDesignObj.ParentUIDesignObj.RootY;
				}
			
				//RECURSE
				foreach(UIDesignObjVariation objUIDesignObjVariation in m_alUIDesignObjVariations)
				{
					objUIDesignObjVariation.Build(SkinID, SkinChanged);
				}

				if(this.Width == UIDesignObj.NoSetValue || this.Height == UIDesignObj.NoSetValue)
				{
					int intMaxWidth = 0;
					int intMaxHeight = 0;
					//calculate the width from the children
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;

						if((this.Width == UIDesignObj.NoSetValue) && (objVariation.TotalWidth > intMaxWidth)) intMaxWidth = objVariation.TotalWidth;
						if((this.Height == UIDesignObj.NoSetValue) && (objVariation.TotalHeight > intMaxHeight)) intMaxHeight = objVariation.TotalHeight;
					}

					if(this.Width == UIDesignObj.NoSetValue) this.Width = intMaxWidth;
					if(this.Height == UIDesignObj.NoSetValue) this.Height = intMaxHeight;
				}				
			}
		}

		public bool Selected
		{
			get	{return m_blnSelected;}
			set	{m_blnSelected = value;}
		}

		public bool MainBackgroundDrawn
		{
			get	{return m_blnMainBackgroundDrawn;}
			set	{m_blnMainBackgroundDrawn = value;}
		}

		public void Draw(Graphics objGraphics, UIDesignObjVariation objVariation)
		{
			objVariation.Draw(this, objGraphics, LanguageID, SkinID);

			//if this is selected, draw a big ol' rectangle around it
			if(ParentUIDesignObj.Selected)
			{
				objGraphics.DrawRectangle(new Pen(Color.Red, 4), this.RootX, this.RootY, this.Width - 4, this.Height - 4);
			}
		}

		public bool Show
		{
			get	{return m_blnShow;}
			set {m_blnShow = value;}
		}

		public bool WarnToSave
		{
			get 
			{
				if (NeedsInsert||NeedsDelete||OriginalsChanged||NeedsParentVariationUnlink||NeedsParentVariationLink||LinkOriginalsChanged)return true;
				foreach(Object uiOV in this.UIDesignObjVariations)
				{
					if (((UIDesignObjVariation)uiOV).WarnToSave)return true;
				}
				return false;
			}
		}

		public bool NeedsParentVariationUnlink
		{
			get	{return m_blnNeedsDBParentVariationUnlink;}
			set	{m_blnNeedsDBParentVariationUnlink = value;}
		}
		public bool NeedsParentVariationLink
		{
			get	{return m_blnNeedsDBParentVariationLink;}
			set	{m_blnNeedsDBParentVariationLink = value;}
		}

		public bool ParentVariationLinked
		{
			get	{return m_blnParentVariationLinked;}
			set	{m_blnParentVariationLinked = value;}
		}

		public bool LinkOriginalsChanged
		{
			get
			{
				return BGOnTop != BGOnTopOriginal || RegenerateForEachScreen != RegenerateForEachScreenOriginal || CanBeHidden != CanBeHiddenOriginal || HideByDefault != HideByDefaultOriginal || Width != WidthOriginal || Height != HeightOriginal || ParentX != ParentXOriginal || ParentY != ParentYOriginal || 
					ParentDisplayOrder != ParentDisplayOrderOriginal || IsTabStop != IsTabStopOriginal || ChildBeforeText != ChildBeforeTextOriginal ||
					ChildBehindBG != ChildBehindBGOriginal ||
					m_blnChildBeforeTextOriginal != m_blnChildBeforeText || m_blnIsTabStopOriginal != m_blnIsTabStop || m_iTiedToOriginal != m_iTiedTo || m_sVisibleStatesOriginal != m_sVisibleStates ||
					m_iTS_UpOriginal != m_iTS_Up || m_iTS_DownOriginal != m_iTS_Down || m_iTS_LeftOriginal != m_iTS_Left || m_iTS_RightOriginal != m_iTS_Right;
			}
		}

		public void ReleaseBitmaps()
		{
			foreach (UIDesignObjVariation uiov in this.m_alUIDesignObjVariations)
			{
				uiov.ReleaseBitmaps();				
			}
		}		


		public bool SaveLinkToDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			bool blnChanged = false;
			//if(this.NeedsParentVariationLink || this.NeedsParentVariationUnlink)
			//{
			if(this.NeedsParentVariationUnlink)
			{
				//if(this.ParentVariationLinked)
				//{

				if(!this.NeedsParentVariationLink)
				{
					if(this.ParentUIDesignObj == null)
					{
						//this should not happen unless it was a deletion of a root object
						//in which case all links were removed in SaveToDatabase

					}
					else
					{
						//delete the row
						DesignObjVariation_DesignObjDataRow drLink = mds.tDesignObjVariation_DesignObj[this.LinkID];
						drLink.dr.Delete();
						mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);
					}
				}

				//}
		
				this.ParentVariationLinked = false;
				blnChanged = true;
			}
			else
			{
				if(this.NeedsParentVariationLink)
				{
					//add the row
					DesignObjVariation_DesignObjDataRow drLink = new DesignObjVariation_DesignObjDataRow(mds.tDesignObjVariation_DesignObj.NewRow());
					drLink.fFK_DesignObj_Child = this.ID;
					drLink.fFK_DesignObjVariation_Parent = this.ParentUIDesignObj.ID;

					DesignObjVariation_DesignObj_Skin_LanguageDataRow drLinkDSL = new DesignObjVariation_DesignObj_Skin_LanguageDataRow(mds.tDesignObjVariation_DesignObj_Skin_Language.NewRow());
						
					if(this.Width < -1)
						drLinkDSL.fWidthSetNull();
					else
						drLinkDSL.fWidth = this.Width;

					if(this.Height < -1)
						drLinkDSL.fHeightSetNull();
					else
						drLinkDSL.fHeight = this.Height;

					drLinkDSL.fCanBeHidden = this.CanBeHidden;
					drLinkDSL.fCanBeHidden = this.CanBeHidden;
					drLinkDSL.fIsTabStop= this.IsTabStop;
					drLinkDSL.fDisplayChildrenBeforeText = this.ChildBeforeText;
					drLinkDSL.fDisplayChildrenBehindBackground = this.ChildBehindBG;
					if( this.m_iTiedTo=="" || this.m_iTiedTo==null )
						drLinkDSL.fsFK_DesignObj_TiedToSetNull();
					else
						drLinkDSL.fsFK_DesignObj_TiedTo = this.m_iTiedTo;

					if( this.m_sVisibleStates=="" )
						drLinkDSL.fVisibleStatesSetNull();
					else
						drLinkDSL.fVisibleStates = this.m_sVisibleStates;
	
					if( this.m_iTS_Up=="" || this.m_iTS_Up==null )
						drLinkDSL.fFK_DesignObj_UpSetNull();
					else
						drLinkDSL.fFK_DesignObj_Up= Convert.ToInt32(this.m_iTS_Up);

					if( this.m_iTS_Down=="" || this.m_iTS_Down==null )
						drLinkDSL.fFK_DesignObj_DownSetNull();
					else
						drLinkDSL.fFK_DesignObj_Down= Convert.ToInt32(this.m_iTS_Down);
							
					if( this.m_iTS_Left=="" || this.m_iTS_Left==null )
						drLinkDSL.fFK_DesignObj_LeftSetNull();
					else
						drLinkDSL.fFK_DesignObj_Left= Convert.ToInt32(this.m_iTS_Left);
							
					if( this.m_iTS_Right=="" || this.m_iTS_Right==null )
						drLinkDSL.fFK_DesignObj_RightSetNull();
					else
						drLinkDSL.fFK_DesignObj_Right= Convert.ToInt32(this.m_iTS_Right);
					drLinkDSL.fDisplayChildrenBehindBackground = this.BGOnTop;
					drLinkDSL.fRegenerateForEachScreen = this.RegenerateForEachScreen;

					drLinkDSL.fX = this.ParentX;
					drLinkDSL.fY = this.ParentY;

					drLinkDSL.fDisplayOrder = this.ParentDisplayOrder;
					drLinkDSL.fFK_DesignObjVariation_DesignObj = drLink.fPK_DesignObjVariation_DesignObj;

					DesignObjModified(drLink.fFK_DesignObj_Child_DataRow);
						
					mds.tDesignObjVariation_DesignObj.Rows.Add(drLink.dr);
					mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);

					mds.tDesignObjVariation_DesignObj_Skin_Language.Rows.Add(drLinkDSL.dr);
					mds.tDesignObjVariation_DesignObj_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
						
					this.LinkID = drLinkDSL.fPK_DesignObjVariation_DesignObj_Skin_Language;
					this.ParentVariationLinked = true;
					blnChanged = true;
				}
				else
				{
					if(this.LinkOriginalsChanged)
					{
						//we need to update				
						DesignObjVariation_DesignObj_Skin_LanguageDataRow drLinkDSL = mds.tDesignObjVariation_DesignObj_Skin_Language[this.LinkID];
								
						if(this.Width < -1)
							drLinkDSL.fWidthSetNull();
						else
							drLinkDSL.fWidth = this.Width;

						if(this.Height < -1)
							drLinkDSL.fHeightSetNull();
						else
							drLinkDSL.fHeight = this.Height;


						drLinkDSL.fCanBeHidden = this.CanBeHidden;
						drLinkDSL.fHideByDefault = this.HideByDefault;
						drLinkDSL.fIsTabStop= this.IsTabStop;
						drLinkDSL.fDisplayChildrenBeforeText = this.ChildBeforeText;
						drLinkDSL.fDisplayChildrenBehindBackground = this.ChildBehindBG;

						if( this.m_sVisibleStates=="" )
							drLinkDSL.fVisibleStatesSetNull();
						else
							drLinkDSL.fVisibleStates = this.m_sVisibleStates;

						if( this.m_iTiedTo=="" || this.m_iTiedTo==null )
							drLinkDSL.fsFK_DesignObj_TiedToSetNull();
						else
							drLinkDSL.fsFK_DesignObj_TiedTo = this.m_iTiedTo;
	
						if( this.m_iTS_Up=="" || this.m_iTS_Up==null )
							drLinkDSL.fFK_DesignObj_UpSetNull();
						else
							drLinkDSL.fFK_DesignObj_Up= Convert.ToInt32(this.m_iTS_Up);

						if( this.m_iTS_Down=="" || this.m_iTS_Down==null )
							drLinkDSL.fFK_DesignObj_DownSetNull();
						else
							drLinkDSL.fFK_DesignObj_Down= Convert.ToInt32(this.m_iTS_Down);
							
						if( this.m_iTS_Left=="" || this.m_iTS_Left==null )
							drLinkDSL.fFK_DesignObj_LeftSetNull();
						else
							drLinkDSL.fFK_DesignObj_Left= Convert.ToInt32(this.m_iTS_Left);
							
						if( this.m_iTS_Right=="" || this.m_iTS_Right==null )
							drLinkDSL.fFK_DesignObj_RightSetNull();
						else
							drLinkDSL.fFK_DesignObj_Right= Convert.ToInt32(this.m_iTS_Right);
						drLinkDSL.fDisplayChildrenBehindBackground = this.BGOnTop;
						drLinkDSL.fRegenerateForEachScreen = this.RegenerateForEachScreen;

						drLinkDSL.fX = this.ParentX;
						drLinkDSL.fY = this.ParentY;
						drLinkDSL.fDisplayOrder = this.ParentDisplayOrder;

						//DesignObjModified(drLinkDSL.fFK_DesignObj_Child_DataRow);

						blnChanged = true;
					}
				}
			}


			this.NeedsParentVariationUnlink = false;
			this.NeedsParentVariationLink = false;


			//set the originals
			this.ResetLinkOriginals();


			//}

			return blnChanged;

		}

		public void DesignObjModified(DesignObjDataRow drObject)
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			string SQL = "UPDATE DesignObj SET psc_mod='" + DateTime.Now.ToString("yyMMddhhmmss") + "' WHERE PK_DesignObj=" + drObject.fPK_DesignObj.ToString();
			new Microsoft.Data.Odbc.OdbcCommand(SQL,mds.m_conn,mds.m_trans).ExecuteNonQuery();
		}

		public void DeselectAllDesignObjs(bool blnRecursive) // Also Text
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach(UIChildSkinLanguage objUIDesignObj in objVariation.DesignObjs)
				{
					objUIDesignObj.Selected = false;
					if(blnRecursive)
					{
						objUIDesignObj.DeselectAllDesignObjs(true);
					}
				}
				foreach (UIText uit in objVariation.Text)
				{
					uit.Selected = false;
				}
			}
		}

		public bool Contains(int intX, int intY)
		{
			if (this.Include) 
			{
				return ((intX >= this.RootX) && (intX <= (this.RootX + this.Width)) && (intY >= this.RootY) && (intY <= (this.RootY + this.Height)));
			}
			else return false;
		}
	}
}