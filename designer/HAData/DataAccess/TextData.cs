namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class TextData : MyDataSet {
		//
		// Text table constants
		//
		public const String TEXT_TABLE = "Text";
		public const String PK_TEXT_FIELD = "PK_Text";
		public const String FK_TEXTCATEGORY_FIELD = "FK_TextCategory";
		public const String DESCRIPTION_FIELD = "Description";
		public const String DEFINE_FIELD = "Define";
		public const String MODIFICATION_RECORDINFO_FIELD = "Modification_RecordInfo";
		public const String ISNEW_RECORDINFO_FIELD = "IsNew_RecordInfo";
		public const String ISDELETED_RECORDINFO_FIELD = "IsDeleted_RecordInfo";
		public const String FK_USERS_RECORDINFO_FIELD = "FK_Users_RecordInfo";
		// table+field constants
		public const String PK_TEXT_TABLE_FIELD = "Text.PK_Text";
		public const String FK_TEXTCATEGORY_TABLE_FIELD = "Text.FK_TextCategory";
		public const String DESCRIPTION_TABLE_FIELD = "Text.Description";
		public const String DEFINE_TABLE_FIELD = "Text.Define";
		public const String MODIFICATION_RECORDINFO_TABLE_FIELD = "Text.Modification_RecordInfo";
		public const String ISNEW_RECORDINFO_TABLE_FIELD = "Text.IsNew_RecordInfo";
		public const String ISDELETED_RECORDINFO_TABLE_FIELD = "Text.IsDeleted_RecordInfo";
		public const String FK_USERS_RECORDINFO_TABLE_FIELD = "Text.FK_Users_RecordInfo";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_TEXT_PARM = "@PK_Text";
		protected const String FK_TEXTCATEGORY_PARM = "@FK_TextCategory";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String DEFINE_PARM = "@Define";
		protected const String MODIFICATION_RECORDINFO_PARM = "@Modification_RecordInfo";
		protected const String ISNEW_RECORDINFO_PARM = "@IsNew_RecordInfo";
		protected const String ISDELETED_RECORDINFO_PARM = "@IsDeleted_RecordInfo";
		protected const String FK_USERS_RECORDINFO_PARM = "@FK_Users_RecordInfo";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public TextData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", TextData.TEXT_TABLE);
		}

		public TextData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", TextData.TEXT_TABLE);
		}

		private TextData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);

			//
			// Build the schema
			//
			Tables.Add(BuildDataTables());
			//
			// Populate the dataset
			//
			base.SetObjectData(info, context);
		}

		public static DataTable BuildDataTables() {
			return (DataTable) BuildTextTable();
		}
		public static TextTable BuildTextTable() {
			//
			// Create the Text table
			//
			TextTable Table = new TextTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_TEXT_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_TEXTCATEGORY_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Column = Columns.Add(DEFINE_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_TEXTCATEGORY_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(DESCRIPTION_PARM, OdbcType.VarChar, 40));
			Params.Add(new OdbcParameter(DEFINE_PARM, OdbcType.VarChar, 40));
			Params.Add(new OdbcParameter(MODIFICATION_RECORDINFO_PARM, OdbcType.DateTime,4));
			Params.Add(new OdbcParameter(ISNEW_RECORDINFO_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ISDELETED_RECORDINFO_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(FK_USERS_RECORDINFO_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_TEXT_PARM].SourceColumn = TextData.PK_TEXT_FIELD;
			if(IsInsert) {
				Params[PK_TEXT_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_TEXTCATEGORY_PARM].SourceColumn = TextData.FK_TEXTCATEGORY_FIELD;
			Params[DESCRIPTION_PARM].SourceColumn = TextData.DESCRIPTION_FIELD;
			Params[DEFINE_PARM].SourceColumn = TextData.DEFINE_FIELD;
			Params[MODIFICATION_RECORDINFO_PARM].SourceColumn = TextData.MODIFICATION_RECORDINFO_FIELD;
			Params[ISNEW_RECORDINFO_PARM].SourceColumn = TextData.ISNEW_RECORDINFO_FIELD;
			Params[ISDELETED_RECORDINFO_PARM].SourceColumn = TextData.ISDELETED_RECORDINFO_FIELD;
			Params[FK_USERS_RECORDINFO_PARM].SourceColumn = TextData.FK_USERS_RECORDINFO_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_Text", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_Text", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_Text", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_Text", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_TEXT_PARM, OdbcType.Int,4, PK_TEXT_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_Text,FK_TextCategory,Description,Define FROM Text", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public TextData LoadText(System.Int32 PK_Text)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_TEXT_PARM].Value = PK_Text;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadTextWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Text"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_Text, FK_TextCategory, Description, Define, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo FROM Text WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Text");
			
			dr=dsTemp.Tables["Text"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheText(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_Text, int FK_Language, out DataRow drChild)
		{
			drChild = null;
			bool bNoDataset=false;
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
				bNoDataset=true;
			}
			else
			{
				DataTable dt = ds.Tables["Text"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_Text", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_TEXT_PARM].Value = PK_Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Text");
				dr = ds.Tables["Text"].Rows.Find(PK_Text);
			if( dr!=null && !bNoDataset && FK_Language>=0 ) // Can't do this without a dataset
			{
				DataTable LSTable = null;
				try 
				{
					LSTable = ds.Tables["Text_LS"];
				}
				catch(Exception) {}
				if( LSTable == null )
				{
					LSTable = Text_LSData.BuildDataTables();
					ds.Tables.Add(LSTable);
				}
					DataRelation rel = ds.Relations["Text_Text_LS"];
					if( rel==null )
						ds.Relations.Add("Text_Text_LS",ds.Tables["Text"].Columns["PK_Text"],ds.Tables["Text_LS"].Columns["FK_Text"]);
				drChild = Text_LSData.LoadText_LS(ref ds, conn, trans, PK_Text, FK_Language);
			}
			return dr;
		}

		public static DataRow LoadText(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_Text, int FK_Language, out DataRow drChild)
		{
			drChild = null;
			bool bNoDataset=false;
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
				bNoDataset=true;
			}
			else
			{
				DataTable dt = ds.Tables["Text"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Text);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_Text", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_TEXT_PARM].Value = PK_Text;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Text");
				dr = ds.Tables["Text"].Rows.Find(PK_Text);
			}
			if( dr!=null && !bNoDataset && FK_Language>=0 ) // Can't do this without a dataset
			{
				DataTable LSTable = null;
				try 
				{
					LSTable = ds.Tables["Text_LS"];
				}
				catch(Exception) {}
				if( LSTable == null )
				{
					LSTable = Text_LSData.BuildDataTables();
					ds.Tables.Add(LSTable);
				}
					DataRelation rel = ds.Relations["Text_Text_LS"];
					if( rel==null )
						ds.Relations.Add("Text_Text_LS",ds.Tables["Text"].Columns["PK_Text"],ds.Tables["Text_LS"].Columns["FK_Text"]);
				drChild = Text_LSData.LoadText_LS(ref ds, conn, trans, PK_Text, FK_Language);
			}
			return dr;
		}

		public static DataRow LoadText(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_Text)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Text"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Text);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_Text", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_TEXT_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_TEXT_PARM].Value = PK_Text;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Text");
				dr = ds.Tables["Text"].Rows.Find(PK_Text);
			}
			return dr;
		}

		public TextData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM Text", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM Text", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Text")==0 )
				return null;
			else
				return ds.Tables["Text"].Rows;

		}

		public TextData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,TEXT_TABLE);
		}
		public TextData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Text");
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans,string sTableName) {
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand(sSQL, conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if(sqlda.Fill(ds,sTableName)==0 )
				return null;
			else
				return ds.Tables[sTableName].Rows;

		}

		public bool UpdateText(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, TextData.TEXT_TABLE);
			return true;
		}

		public static bool UpdateText(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateText(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateText(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[TEXT_TABLE];
			if( dt == null )
				return false;

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = null;
			OdbcCommand InsertCommand = null;
			OdbcCommand UpdateCommand = null;
			OdbcCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);
			sqlda.RowUpdated += new OdbcRowUpdatedEventHandler(MyRowUpdated);

			sqlda.Update(dt);
			return true;
		}

		static void MyRowUpdated(Object sender, OdbcRowUpdatedEventArgs e)
		{
			if( e.StatementType==StatementType.Insert )
			{
				OdbcCommand ocmd = new OdbcCommand("SELECT @@IDENTITY", e.Command.Connection);
				int value = Int32.Parse(ocmd.ExecuteScalar().ToString());
				e.Row[0]=value;
				e.Row.AcceptChanges();
			}
		}

	} // public class TextData
	public class TextDataRow
	{
		public DataRow dr = null;
		public TextDataRow(DataRow d)
		{
			dr=d;
		}

		public bool bIsValid
		{
			get
			{
				return dr!=null;
			}
		}
		public System.Int32 fPK_Text
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_TextCategory
		{
			get
			{
				return Convert.ToInt32(dr[1]);
			}
			set
			{
				dr[1]=value;
			}
		}
		public TextCategoryDataRow fFK_TextCategory_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tTextCategory[Convert.ToInt32(dr[1])];
			}
		}
		public System.String fDescription
		{
			get
			{
				return Convert.ToString(dr[2]);
			}
			set
			{
				dr[2]=value;
			}
		}
		public System.String fDefine
		{
			get
			{
				return Convert.ToString(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
		public System.DateTime fModification_RecordInfo
		{
			get
			{
				return Convert.ToDateTime(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public bool fModification_RecordInfoIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fModification_RecordInfoSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public System.Boolean fIsNew_RecordInfo
		{
			get
			{
				return Convert.ToBoolean(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public bool fIsNew_RecordInfoIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fIsNew_RecordInfoSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.Boolean fIsDeleted_RecordInfo
		{
			get
			{
				return Convert.ToBoolean(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public bool fIsDeleted_RecordInfoIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fIsDeleted_RecordInfoSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.Int32 fFK_Users_RecordInfo
		{
			get
			{
				return Convert.ToInt32(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public bool fFK_Users_RecordInfoIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fFK_Users_RecordInfoSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public UsersDataRow fFK_Users_RecordInfo_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tUsers[Convert.ToInt32(dr[7])];
			}
		}
		public string fDescription_LS(int PK_Language)
		{
			return fDescription_LS(PK_Language,true);
		}
		public string fDescription_LS(int PK_Language,bool ReturnEnglish)
		{
			object o = null;
			MyDataSet mds = (MyDataSet) dr.Table.DataSet;
			DataRow drLS = mds.tText_LS[Convert.ToInt32(dr[0]),PK_Language].dr;
			if( drLS!=null )
				o=drLS[2];
			if( o==null && PK_Language!=1 && ReturnEnglish )
			{
				drLS = mds.tText_LS[Convert.ToInt32(dr[0]),1].dr;
				if( drLS!=null )
					 o=drLS[2];
			}
			return (o==null ? null : Convert.ToString(o));
		}
		public void fDescription_LS(int PK_Language,string NewValue)
		{
			MyDataSet mds = (MyDataSet) dr.Table.DataSet;
			Text_LSDataRow drTLS = mds.tText_LS[Convert.ToInt32(dr[0]),PK_Language];
			if( drTLS.bIsValid )
				drTLS.fDescription = NewValue;
			else
			{
				DataRow drNew = mds.tText_LS.NewRow();
				drNew[0]=dr[0];
				drNew[1]=PK_Language;
				drNew[2]=NewValue;
				mds.tText_LS.Rows.Add(drNew);
			}
		}
	} // public class TextDataRow
	public class TextDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public TextDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public TextDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public TextDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public TextDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public TextDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public TextDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Text WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,trans);
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public void GotoTop() { iRecord=-1; }
		public int NumRecords { get { return iNumRecords; } }
		void CacheAndClose()
		{
			al = new ArrayList();
			iNumRecords=0;
			while( dr.Read() )
			{
				iNumRecords++;
				object[] objs = new object[8];
				for(int i=0;i<8;i++)
					objs[i]=dr[i];
				al.Add(objs);
			}
			dr.Close();
		}
		public bool Read()
		{
			if( !bCache )
				return dr.Read();

			return ++iRecord<iNumRecords;;
		}

		public void Close()
		{
			if( !bCache )
				dr.Close();
			else
			{
				bCache=false;
				al = null;
			}
		}
		public System.Int32 fPK_Text
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_TextCategory
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public System.String fDefine
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[3]);
				else
					return Convert.ToString(dr[3]);
			}
		}
		public System.DateTime fModification_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToDateTime(((object[]) al[iRecord])[4]);
				else
					return Convert.ToDateTime(dr[4]);
			}
		}
		public bool fModification_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.Boolean fIsNew_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[5]);
				else
					return Convert.ToBoolean(dr[5]);
			}
		}
		public bool fIsNew_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.Boolean fIsDeleted_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[6]);
				else
					return Convert.ToBoolean(dr[6]);
			}
		}
		public bool fIsDeleted_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Users_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[7]);
				else
					return Convert.ToInt32(dr[7]);
			}
		}
		public bool fFK_Users_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
	} // public class TextDataReader
	public class TextTable : DataTable
	{
		public TextTable() : base("Text") {}

		public TextDataRow this [System.Int32 PK_Text]
		{
			get
			{
				TextDataRow dr = new TextDataRow(Rows.Find(PK_Text));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_Text", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_Text", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_Text"].Value = PK_Text;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"Text");
					dr = new TextDataRow(Rows.Find(PK_Text));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_Text,FK_TextCategory,Description,Define FROM Text", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Text")==0 )
				return null;
			else
				return Rows;
		}
		public void Update(int PK_Users)
		{
			Update(PK_Users,((MyDataSet) DataSet).m_conn,((MyDataSet) DataSet).m_trans);
		}
		public void Update(int PK_Users,OdbcConnection conn, OdbcTransaction trans)
		{
			if( conn==null )
				return;
			MyDataSet ds = (MyDataSet) this.DataSet;
			TextData.UpdateText(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Text
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_TextCategory
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cDescription
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cDefine
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cModification_RecordInfo
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cIsNew_RecordInfo
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cIsDeleted_RecordInfo
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cFK_Users_RecordInfo
		{
			get
			{
				return Columns[7];
			}
		}
	}
} // namespace HAData.Common.Data
