using System;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using HAData.Common;
using HAData.DataAccess;


using System.Reflection;


namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObj.
	/// </summary>
	public class UIDesignObj
	{
		//MEMBER VARIABLES
		private string graphicsDirectory;
		public string GraphicsDirectory
		{
			get {return this.graphicsDirectory;}
			set {this.graphicsDirectory = value;}
		}

		private int m_intID = -1;
		private string m_strDescription;
		private sbyte m_intPriority;
		private bool m_blnAnimate;
		private int m_intUIDesignObjType;
		private int m_intDesignObjCategoryID;
		private ArrayList m_alUIDesignObjVariations = new ArrayList();
		private UIDesignObjVariation m_objParentUIDesignObjVariation;
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
		private int m_intWidth = -1;
		private int m_intHeight = -1;
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
		



		
		//private bool m_blnIsVariation = false;		//if is true, then we will draw differently
		//private bool m_blnIsSelected = false;
		//private bool m_blnAutoSelectChildren = false;		//if true and this is selected, also select children
		

		//PROPERTIES
		public int ID
		{
			get	{return m_intID;}
			set {m_intID = value;}
		}
		public string DescriptionOnly
		{
			get {return this.m_strDescription;}
			set {this.m_strDescription = value;}
		}
		public string Description
		{
			get
			{
				if (this.m_strDescription == null) return "";
				else return this.m_strDescription + "(" + ID + ")" + (this.m_bitHideByDefault ? " h" : "");
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

		public UIDesignObjVariation ParentUIDesignObjVariation
		{
			get {return m_objParentUIDesignObjVariation;}
			set	{m_objParentUIDesignObjVariation = value;}
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
        
		public bool NeedsInsert
		{	
			get {return m_blnNeedsDBInsert;}
			set	{m_blnNeedsDBInsert = value;}
		}
		public bool	NeedsDelete
		{
			get	{return m_blnNeedsDBDelete;}
			set	{m_blnNeedsDBDelete = value;}
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

		public bool Deleted
		{
			get	{return m_blnDeleted;}
		}

		public bool ParentVariationLinked
		{
			get	{return m_blnParentVariationLinked;}
			set	{m_blnParentVariationLinked = value;}
		}

		
		public bool Selected
		{
			get	{return m_blnSelected;}
			set	{m_blnSelected = value;}
		}



		public bool Show
		{
			get	{return m_blnShow;}
			set {m_blnShow = value;}
		}
		public bool Include
		{
			get
			{
				return  (!this.Deleted && !this.NeedsParentVariationUnlink && (this.ParentVariationLinked || (this.ParentUIDesignObjVariation == null)) && !this.NeedsDelete);

			}
		}



		public bool OriginalsChanged
		{
			get
			{
				return m_intDesignObjCategoryIDOriginal != m_intDesignObjCategoryID || m_strDescription != m_strDescriptionOriginal || m_intPriority != m_intPriorityOriginal || this.m_blnAnimate != this.m_blnAnimateOriginal || m_intUIDesignObjType != m_intUIDesignObjTypeOriginal || m_blnCantGoBack != m_blnCantGoBackOriginal;			}
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

		public bool DescriptionChanged
		{
			get
			{
				return (this.m_strDescription != this.m_strDescriptionOriginal);
			}
		}

		public bool MainBackgroundDrawn
		{
			get	{return m_blnMainBackgroundDrawn;}
			set	{m_blnMainBackgroundDrawn = value;}
		}



		//CONSTRUCTORS
		public UIDesignObj(UIDesignObjVariation objParentUIDesignObjVariation, int intID, string graphicsDir)
		{
			this.graphicsDirectory = graphicsDir;

			m_objParentUIDesignObjVariation = objParentUIDesignObjVariation;
			m_intID = intID;		

			if(objParentUIDesignObjVariation == null)
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

			if(objParentUIDesignObjVariation == null)
			{
				Build(1, true); // Start with default skin
				this.ResetLinkOriginals();
			}
		}


		// DESTRUCTOR LIKE

		public void ReleaseBitmaps()
		{
			foreach (UIDesignObjVariation uiov in this.m_alUIDesignObjVariations)
			{
                uiov.ReleaseBitmaps();				
			}
		}



		//METHODS

		public void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
	
			DesignObjDataRow drDesignObj = mds.tDesignObj[m_intID];


			m_strDescription = drDesignObj.fDescription;
			m_intPriority = drDesignObj.fPriority;
			this.m_blnAnimate = drDesignObj.fAnimate;
			m_intUIDesignObjType = drDesignObj.fFK_DesignObjType;

			m_blnCantGoBack = drDesignObj.fCantGoBack;
			


			this.DesignObjCategory = drDesignObj.fFK_DesignObjCategory;


			
			//set the originals so we know how and when to save
			this.ResetOriginals();


			m_blnNeedsDBInsert = false;
			m_blnNeedsDBDelete = false;
			


			//load the UIDesignObjVariations
			//no sort for now
			DataRow[] drVariations = mds.tDesignObjVariation.Select(DesignObjVariationData.FK_DESIGNOBJ_FIELD + "=" + m_intID, DesignObjVariationData.PK_DESIGNOBJVARIATION_FIELD);
			foreach(DataRow dr in drVariations)
			{
				DesignObjVariationDataRow drVariation = new DesignObjVariationDataRow(dr);
				m_alUIDesignObjVariations.Add(new UIDesignObjVariation(this, drVariation.fPK_DesignObjVariation, -1));
			}


		}


		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			

			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{

					//uTODO nlink from ALLL parent variations
					this.NeedsParentVariationUnlink = true;
					DataRow[] drLinks = mds.tDesignObjVariation_DesignObj.Select(DesignObjVariation_DesignObjData.FK_DESIGNOBJ_CHILD_FIELD + "=" + this.ID, null);
					foreach(DataRow dr in drLinks)
					{
						dr.Delete();
					}
					mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);



					//delete the variations
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
						objVariation.NeedsDelete = true;
						objVariation.SaveToDatabase();
					}


					if(!this.NeedsInsert)
					{
					//delete this object
					DesignObjDataRow drDesignObj = mds.tDesignObj[m_intID];
					drDesignObj.dr.Delete();
					mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);
					}

					blnChanged = true;
					m_blnDeleted = true;
				}
				else
				{

					if(this.NeedsInsert)
					{
						//insert this object
						DesignObjDataRow drDesignObj = new DesignObjDataRow(mds.tDesignObj.NewRow());

						drDesignObj.fDescription = m_strDescription;
						drDesignObj.fCantGoBack = m_blnCantGoBack;
						drDesignObj.fPriority = m_intPriority;
						drDesignObj.fAnimate = this.m_blnAnimate;
						drDesignObj.fFK_DesignObjType = m_intUIDesignObjType;
						drDesignObj.fFK_DesignObjCategory = this.DesignObjCategory;
						mds.tDesignObj.Rows.Add(drDesignObj.dr);

						//we need the new id for the new variations below
						mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);
				
						this.ID = drDesignObj.fPK_DesignObj;

						blnChanged = true;

					}
					else
					{
						if(this.OriginalsChanged)
						{
							//update this object
							DesignObjDataRow drDesignObj = mds.tDesignObj[m_intID];

							drDesignObj.fDescription = m_strDescription;
							drDesignObj.fCantGoBack = m_blnCantGoBack;
							drDesignObj.fPriority = m_intPriority;
							drDesignObj.fAnimate = this.m_blnAnimate;
							drDesignObj.fFK_DesignObjType = m_intUIDesignObjType;
							drDesignObj.fFK_DesignObjCategory = this.DesignObjCategory;
							
							blnChanged = true;

						}
					}

					//Save link changes
					blnChanged = this.SaveLinkToDatabase() || blnChanged;

					//save all variations to database
					//this must be done after an object insertion because we need the new object id
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
						blnChanged = objVariation.SaveToDatabase() || blnChanged;
					}
				}
	
				//set the originals so we know how and when to save
				this.ResetOriginals();


				this.NeedsInsert = false;
				this.NeedsDelete = false;

			}


			//if this is the root node, then update the tables if needed
			if(this.ParentUIDesignObjVariation == null && blnChanged)
			{
				//update the tables
				/*
				mds.tDesignObjVariation_DesignObjParameter.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObjVariation_Text.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObjVariation_Zone.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObjVariation.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);
				mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);
				
				*/


				mds.Update(1,mds.m_conn,mds.m_trans);

			}



			return blnChanged;

			
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
						if(this.ParentUIDesignObjVariation == null)
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
						drLink.fFK_DesignObjVariation_Parent = this.ParentUIDesignObjVariation.ID;


						if(this.Width == -1)
							drLink.fWidthSetNull();
						else
							drLink.fWidth = this.Width;

						if(this.Height == -1)
							drLink.fHeightSetNull();
						else
							drLink.fHeight = this.Height;

						drLink.fCanBeHidden = this.CanBeHidden;
						drLink.fCanBeHidden = this.CanBeHidden;
						drLink.fIsTabStop= this.IsTabStop;
						drLink.fDisplayChildrenBeforeText = this.ChildBeforeText;
						drLink.fDisplayChildrenBehindBackground = this.ChildBehindBG;
						if( this.m_iTiedTo=="" || this.m_iTiedTo==null )
							drLink.fsFK_DesignObj_TiedToSetNull();
						else
							drLink.fsFK_DesignObj_TiedTo = this.m_iTiedTo;

						if( this.m_sVisibleStates=="" )
							drLink.fVisibleStatesSetNull();
						else
							drLink.fVisibleStates = this.m_sVisibleStates;
	
						if( this.m_iTS_Up=="" || this.m_iTS_Up==null )
							drLink.fFK_DesignObj_UpSetNull();
						else
							drLink.fFK_DesignObj_Up= Convert.ToInt32(this.m_iTS_Up);

						if( this.m_iTS_Down=="" || this.m_iTS_Down==null )
							drLink.fFK_DesignObj_DownSetNull();
						else
							drLink.fFK_DesignObj_Down= Convert.ToInt32(this.m_iTS_Down);
							
						if( this.m_iTS_Left=="" || this.m_iTS_Left==null )
							drLink.fFK_DesignObj_LeftSetNull();
						else
							drLink.fFK_DesignObj_Left= Convert.ToInt32(this.m_iTS_Left);
							
						if( this.m_iTS_Right=="" || this.m_iTS_Right==null )
							drLink.fFK_DesignObj_RightSetNull();
						else
							drLink.fFK_DesignObj_Right= Convert.ToInt32(this.m_iTS_Right);
						drLink.fDisplayChildrenBehindBackground = this.BGOnTop;
						drLink.fRegenerateForEachScreen = this.RegenerateForEachScreen;

						drLink.fX = this.ParentX;
						drLink.fY = this.ParentY;

						drLink.fDisplayOrder = this.ParentDisplayOrder;

						DesignObjModified(drLink.fFK_DesignObj_Child_DataRow);

						mds.tDesignObjVariation_DesignObj.Rows.Add(drLink.dr);
						mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);

						this.LinkID = drLink.fPK_DesignObjVariation_DesignObj;

						this.ParentVariationLinked = true;
						blnChanged = true;
					}
					else
					{
						if(this.LinkOriginalsChanged)
						{
							//we need to update				
							DesignObjVariation_DesignObjDataRow drLink = mds.tDesignObjVariation_DesignObj[this.LinkID];
								
							if(this.Width == -1)
								drLink.fWidthSetNull();
							else
								drLink.fWidth = this.Width;

							if(this.Height == -1)
								drLink.fHeightSetNull();
							else
								drLink.fHeight = this.Height;


							drLink.fCanBeHidden = this.CanBeHidden;
							drLink.fHideByDefault = this.HideByDefault;
							drLink.fIsTabStop= this.IsTabStop;
							drLink.fDisplayChildrenBeforeText = this.ChildBeforeText;
							drLink.fDisplayChildrenBehindBackground = this.ChildBehindBG;

							if( this.m_sVisibleStates=="" )
								drLink.fVisibleStatesSetNull();
							else
								drLink.fVisibleStates = this.m_sVisibleStates;

							if( this.m_iTiedTo=="" || this.m_iTiedTo==null )
								drLink.fsFK_DesignObj_TiedToSetNull();
							else
								drLink.fsFK_DesignObj_TiedTo = this.m_iTiedTo;
	
							if( this.m_iTS_Up=="" || this.m_iTS_Up==null )
								drLink.fFK_DesignObj_UpSetNull();
							else
								drLink.fFK_DesignObj_Up= Convert.ToInt32(this.m_iTS_Up);

							if( this.m_iTS_Down=="" || this.m_iTS_Down==null )
								drLink.fFK_DesignObj_DownSetNull();
							else
								drLink.fFK_DesignObj_Down= Convert.ToInt32(this.m_iTS_Down);
							
							if( this.m_iTS_Left=="" || this.m_iTS_Left==null )
								drLink.fFK_DesignObj_LeftSetNull();
							else
								drLink.fFK_DesignObj_Left= Convert.ToInt32(this.m_iTS_Left);
							
							if( this.m_iTS_Right=="" || this.m_iTS_Right==null )
								drLink.fFK_DesignObj_RightSetNull();
							else
								drLink.fFK_DesignObj_Right= Convert.ToInt32(this.m_iTS_Right);
							drLink.fDisplayChildrenBehindBackground = this.BGOnTop;
							drLink.fRegenerateForEachScreen = this.RegenerateForEachScreen;

							drLink.fX = this.ParentX;
							drLink.fY = this.ParentY;
							drLink.fDisplayOrder = this.ParentDisplayOrder;

							DesignObjModified(drLink.fFK_DesignObj_Child_DataRow);

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


		public void Draw(Graphics objGraphics, int languageID, int skinID)
		{

			//if this is the top level object, reset whether the mainBackground has been drawn
			if(this.ParentUIDesignObjVariation == null)
			{
				this.MainBackgroundDrawn = false;
			}

			if(this.Include && this.Show)	//don't include if deleted or set to be deleted or unlinked
			{

				//determine how to show.  this will depend on what variation is selected and how they want things
				//they may want to hide non selected vriations, maybe outlined or ghosted


				foreach(Object obj in this.UIDesignObjVariations)
				{
					UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
					//only draw the selected variations if this is the root
					if(this.ParentUIDesignObjVariation != null || objVariation.Selected)
					{
						objVariation.Draw(objGraphics, languageID, skinID);
					}
				}

				//if this is selected, draw a big ol' rectangle around it
				if(this.Selected)
				{
					objGraphics.DrawRectangle(new Pen(Color.Red, 4), this.RootX, this.RootY, this.Width - 4, this.Height - 4);
				}
			}
		}



		public void Reset()
		{
			//this.Width = 0;
			//this.Height = 0;
			foreach(Object obj in m_alUIDesignObjVariations)
			{
				UIDesignObjVariation objUIDesignObjVariation = (UIDesignObjVariation) obj;
				objUIDesignObjVariation.Reset();
			}
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
				if(this.ParentUIDesignObjVariation == null)
				{
					this.RootX = 0;
					this.RootY = 0;
				}
				else
				{
					this.RootX = this.ParentX + this.ParentUIDesignObjVariation.ParentUIDesignObj.RootX;
					this.RootY = this.ParentY + this.ParentUIDesignObjVariation.ParentUIDesignObj.RootY;
				}
			
				//RECURSE
				foreach(UIDesignObjVariation objUIDesignObjVariation in m_alUIDesignObjVariations)
				{
					objUIDesignObjVariation.Build(SkinID, SkinChanged);
				}

/* 10/6/2004 I wanted to comment out this stuff so that the width and height remain -1.  But Designer is such spaghetti code it breaks everything, and 
 * it's too hard to figure out why.  I'll put the logic in the OrbiterGen to ignore the specified width and height and auto-calculate. */
				//do the build bottom up stuff
				if(this.Width == -1 || this.Height == -1)
				{
					int intMaxWidth = 0;
					int intMaxHeight = 0;
					//calculate the width from the children
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;

						if((this.Width == -1) && (objVariation.TotalWidth > intMaxWidth)) intMaxWidth = objVariation.TotalWidth;
						if((this.Height == -1) && (objVariation.TotalHeight > intMaxHeight)) intMaxHeight = objVariation.TotalHeight;
					}

					if(this.Width == -1) this.Width = intMaxWidth;
					if(this.Height == -1) this.Height = intMaxHeight;
				}				
			}
		}

		public bool SelectVariationIndex(int intIndex)
		{
			UIDesignObjVariation objVariation = (UIDesignObjVariation) this.UIDesignObjVariations[intIndex];
			objVariation.Selected = true;
			return true;
		}
		public bool DeSelectVariationIndex(int intIndex)
		{
			UIDesignObjVariation objVariation = (UIDesignObjVariation) this.UIDesignObjVariations[intIndex];
			objVariation.Selected = false;
			return true;
		}

		public bool Contains(int intX, int intY)
		{
			if (this.Include) 
			{
				return ((intX >= this.RootX) && (intX <= (this.RootX + this.Width)) && (intY >= this.RootY) && (intY <= (this.RootY + this.Height)));
			}
			else return false;
		}

		public bool ClickImage(int intX, int intY)
		{
			ArrayList alFound = new ArrayList();


			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if(objVariation.Include)
				{
					if(objVariation.Show)
					{
						//go thru and see if any DesignObjs have been clicked
						foreach(Object obj2 in objVariation.DesignObjs)
						{
							UIDesignObj objUIDesignObj = (UIDesignObj) obj2;
							if(objUIDesignObj.Contains(intX, intY))
							{
								alFound.Add(objUIDesignObj);
							}
							else
							{
								objUIDesignObj.Selected = false;
							}
						}
					}
				}
			}


			//TODO this will change when we have multiple select capability
			//now go thru the found array
			//if none are selected, select the first one
			//else select the next one
			
			//int intNewSelectedIndex = -1;
			
			if(alFound.Count > 0)
			{
				this.DeselectAllText(); // TODO: integrate text into this procedure

				if(alFound.Count == 1)
				{
					//toggle the item
					UIDesignObj objUIDesignObj = (UIDesignObj) alFound[0];
					objUIDesignObj.Selected = !objUIDesignObj.Selected;

					//deselect the old one
					if(m_UIDesignObjSelected != null && m_UIDesignObjSelected != objUIDesignObj)
					{
						m_UIDesignObjSelected.Selected = false;
					}

					if(objUIDesignObj.Selected)
					{
						m_UIDesignObjSelected = objUIDesignObj;
					}
					else
					{
						m_UIDesignObjSelected = null;
					}

				}
				else
				{
					int intSelectedIndex = -1;
					int intNewSelectedIndex = -1;

					
					//find the selected item if there is one
					for(int intCount=0; intCount < alFound.Count; intCount++)
					{
						UIDesignObj objUIDesignObj = (UIDesignObj) alFound[intCount];
						if(objUIDesignObj.Selected) 
						{
							intSelectedIndex = intCount;
							break;
						}

					}
					if(intSelectedIndex == -1)
					{
						//none selected, so select the first one
						intNewSelectedIndex = 0;
					}
					else
					{
						if(intSelectedIndex == (alFound.Count - 1))
						{
							//we are at the end of the line, so loop around to 0
							intNewSelectedIndex = 0;
						}
						else
						{
							intNewSelectedIndex = intSelectedIndex + 1;
						}
					}

					
					

					if(intSelectedIndex != -1)
					{
						UIDesignObj objUIDesignObjSelectedTemp;
						objUIDesignObjSelectedTemp = (UIDesignObj) alFound[intSelectedIndex];
						objUIDesignObjSelectedTemp.Selected = false;
					}

					UIDesignObj objNewUIDesignObjSelectedTemp;
					objNewUIDesignObjSelectedTemp = (UIDesignObj) alFound[intNewSelectedIndex];
					objNewUIDesignObjSelectedTemp.Selected = true;

					if(m_UIDesignObjSelected != null)
					{
						m_UIDesignObjSelected.Selected = false;
					}
					m_UIDesignObjSelected = objNewUIDesignObjSelectedTemp;

				}
			}

			return (alFound.Count > 0);

		}


		public UIDesignObjVariation AddNewVariation(int intCriteria_DID, bool blnSelect, int SkinID)
		{
			//make sure that a variation with the give criteria id is not already there
			bool blnFound = false;
			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if (objVariation.CriteriaID == intCriteria_DID && objVariation.Include)
				{
					blnFound = true;
					break;
				}
			}
			if(!blnFound)
			{
				UIDesignObjVariation objVariation = new UIDesignObjVariation(this, -1, intCriteria_DID);
				objVariation.NeedsInsert = true;
				this.UIDesignObjVariations.Add(objVariation);
				objVariation.Selected = blnSelect;

				this.Build(SkinID, true); //we need to rebuild

				//return true;
				return objVariation;
			}
			else
			{
				//return false;
				return null;
			}
		}


		public void DeselectAllDesignObjs(bool blnRecursive) // Also Text
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach(UIDesignObj objUIDesignObj in objVariation.DesignObjs)
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
		public void DeselectAllText()
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach (UIText uit in objVariation.Text)
				{
					uit.Selected = false;
				}
			}
		}
		public void DeselectAllVariations(bool blnRecursive)
		{
			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				objVariation.Selected = false;
				if(blnRecursive)
				{
					foreach(UIDesignObj objUIDesignObj in objVariation.DesignObjs)
					{
						objUIDesignObj.DeselectAllVariations(true);
					}
				}
			}
		}


		public void ResetOriginals()
		{
			m_strDescriptionOriginal = m_strDescription;
			m_blnCantGoBackOriginal = m_blnCantGoBack;
			m_intPriorityOriginal = m_intPriority;
			this.m_blnAnimateOriginal = this.m_blnAnimate;
			m_intUIDesignObjTypeOriginal = m_intUIDesignObjType;
			m_intDesignObjCategoryIDOriginal = m_intDesignObjCategoryID;
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

		public void DesignObjModified(DesignObjDataRow drObject)
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			string SQL = "UPDATE DesignObj SET Modification_RecordInfo='" + DateTime.Now.ToString("yyMMddhhmmss") + "' WHERE PK_DesignObj=" + drObject.fPK_DesignObj.ToString();
			new Microsoft.Data.Odbc.OdbcCommand(SQL,mds.m_conn,mds.m_trans).ExecuteNonQuery();
			int PK_DesignObj=0;

			// Here are all the variations that depend on us
			foreach(DataRow dr in drObject.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_DesignObj_FK_DesignObj_Child) )
			{
				DesignObjVariation_DesignObjDataRow drV_O = new DesignObjVariation_DesignObjDataRow(dr);
				if( drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj!=PK_DesignObj )
				{
					// Minor optimization, if we hit the same object twice in a row don't reset it twice
					PK_DesignObj = drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj;
					DesignObjModified(drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj_DataRow);
				}
			}
		}
	}
}
