namespace HAData.DataAccess {
	using System;
	using System.Data;
	using MySql;
	using MySql.Data;
	using MySql.Data.MySqlClient;
	using System.Collections;

	using HAData.Common;

	public class SkinData : MyDataSet {
		//
		// Skin table constants
		//
		public const String SKIN_TABLE = "Skin";
		public const String PK_SKIN_FIELD = "PK_Skin";
		public const String DESCRIPTION_FIELD = "Description";
		public const String DATASUBDIRECTORY_FIELD = "DataSubdirectory";
		public const String FK_STYLE_FIELD = "FK_Style";
		public const String FK_SKIN_TEXTPLACEMENT_FIELD = "FK_Skin_TextPlacement";
		public const String DRAWTEXTBEFORECHILDREN_FIELD = "DrawTextBeforeChildren";
		public const String FK_STABILITYSTATUS_FIELD = "FK_StabilityStatus";
		// table+field constants
		public const String PK_SKIN_TABLE_FIELD = "Skin.PK_Skin";
		public const String DESCRIPTION_TABLE_FIELD = "Skin.Description";
		public const String DATASUBDIRECTORY_TABLE_FIELD = "Skin.DataSubdirectory";
		public const String FK_STYLE_TABLE_FIELD = "Skin.FK_Style";
		public const String FK_SKIN_TEXTPLACEMENT_TABLE_FIELD = "Skin.FK_Skin_TextPlacement";
		public const String DRAWTEXTBEFORECHILDREN_TABLE_FIELD = "Skin.DrawTextBeforeChildren";
		public const String FK_STABILITYSTATUS_TABLE_FIELD = "Skin.FK_StabilityStatus";
		// DataSetCommand object
		protected MySqlDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_SKIN_PARM = "@PK_Skin";
		protected const String DESCRIPTION_PARM = "@Description";
		protected const String DATASUBDIRECTORY_PARM = "@DataSubdirectory";
		protected const String FK_STYLE_PARM = "@FK_Style";
		protected const String FK_SKIN_TEXTPLACEMENT_PARM = "@FK_Skin_TextPlacement";
		protected const String DRAWTEXTBEFORECHILDREN_PARM = "@DrawTextBeforeChildren";
		protected const String FK_STABILITYSTATUS_PARM = "@FK_StabilityStatus";
		protected const String USERID_PARM = "@UserID";

		protected MySqlCommand m_LoadCommand;
		protected MySqlCommand m_InsertCommand;
		protected MySqlCommand m_UpdateCommand;
		protected MySqlCommand m_DeleteCommand;
		protected MySqlConnection m_Connection;
		protected MySqlTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public SkinData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetMySqlConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", SkinData.SKIN_TABLE);
		}

		public SkinData(MySqlConnection conn,MySqlTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new MySqlDataAdapter();

			m_DSCommand.TableMappings.Add("Table", SkinData.SKIN_TABLE);
		}

		private SkinData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildSkinTable();
		}
		public static SkinTable BuildSkinTable() {
			//
			// Create the Skin table
			//
			SkinTable Table = new SkinTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_SKIN_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(DESCRIPTION_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(DATASUBDIRECTORY_FIELD, typeof(System.String));
			Columns.Add(FK_STYLE_FIELD, typeof(System.Int32));
			Columns.Add(FK_SKIN_TEXTPLACEMENT_FIELD, typeof(System.Int32));
			Column = Columns.Add(DRAWTEXTBEFORECHILDREN_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_STABILITYSTATUS_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 1;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(MySqlParameterCollection Params, bool IsInsert) {
			Params.Add(new MySqlParameter(PK_SKIN_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DESCRIPTION_PARM, MySqlDbType.VarChar, 30));
			Params.Add(new MySqlParameter(DATASUBDIRECTORY_PARM, MySqlDbType.VarChar, 30));
			Params.Add(new MySqlParameter(FK_STYLE_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(FK_SKIN_TEXTPLACEMENT_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(DRAWTEXTBEFORECHILDREN_PARM, MySqlDbType.Bit,1));
			Params.Add(new MySqlParameter(FK_STABILITYSTATUS_PARM, MySqlDbType.Int32,4));
			Params.Add(new MySqlParameter(USERID_PARM, MySqlDbType.Int32));

			// map the parameters to the data table

			Params[PK_SKIN_PARM].SourceColumn = SkinData.PK_SKIN_FIELD;
			if(IsInsert) {
				Params[PK_SKIN_PARM].Direction = ParameterDirection.Output;
			}

			Params[DESCRIPTION_PARM].SourceColumn = SkinData.DESCRIPTION_FIELD;
			Params[DATASUBDIRECTORY_PARM].SourceColumn = SkinData.DATASUBDIRECTORY_FIELD;
			Params[FK_STYLE_PARM].SourceColumn = SkinData.FK_STYLE_FIELD;
			Params[FK_SKIN_TEXTPLACEMENT_PARM].SourceColumn = SkinData.FK_SKIN_TEXTPLACEMENT_FIELD;
			Params[DRAWTEXTBEFORECHILDREN_PARM].SourceColumn = SkinData.DRAWTEXTBEFORECHILDREN_FIELD;
			Params[FK_STABILITYSTATUS_PARM].SourceColumn = SkinData.FK_STABILITYSTATUS_FIELD;
		}

		protected static void CreateCommands(MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new MySqlCommand("sp_Select_Skin", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_SKIN_PARM, MySqlDbType.Int32,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new MySqlCommand("sp_Insert_Skin", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				MySqlParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new MySqlCommand("sp_Update_Skin", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				MySqlParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new MySqlCommand("sp_Delete_Skin", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_SKIN_PARM, MySqlDbType.Int32,4, PK_SKIN_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, MySqlDbType.Int32);
			}
		}

		protected static void CreateCommands(MySqlDataAdapter odbcda,MySqlConnection Conn, MySqlTransaction Trans, ref MySqlCommand LoadCommand, ref MySqlCommand InsertCommand, ref MySqlCommand UpdateCommand, ref MySqlCommand DeleteCommand) {
				LoadCommand = new MySqlCommand("SELECT PK_Skin,Description,DataSubdirectory,FK_Style,FK_Skin_TextPlacement,DrawTextBeforeChildren,FK_StabilityStatus FROM Skin", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new MySqlParameter(PK_SKIN_PARM, MySqlDbType.Int32,4));

			odbcda.SelectCommand = LoadCommand;
			MySqlCommandBuilder odbcCB = new MySqlCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public SkinData LoadSkin(System.Int32 PK_Skin)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_SKIN_PARM].Value = PK_Skin;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadSkinWithWhere(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Skin"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			string sSQL = "SELECT PK_Skin, Description, DataSubdirectory, FK_Style, FK_Skin_TextPlacement, DrawTextBeforeChildren, FK_StabilityStatus FROM Skin WHERE " + WhereClause;
			
			MySqlCommand LoadCommand = new MySqlCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"Skin");
			
			dr=dsTemp.Tables["Skin"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheSkin(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Skin)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Skin"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();

			LoadCommand = new MySqlCommand("sp_Select_Skin", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new MySqlParameter(PK_SKIN_PARM, MySqlDbType.Int32,4));
			LoadCommand.Parameters[PK_SKIN_PARM].Value = PK_Skin;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"Skin");
				dr = ds.Tables["Skin"].Rows.Find(PK_Skin);
			return dr;
		}

		public static DataRow LoadSkin(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans, System.Int32 PK_Skin)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["Skin"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_Skin);
			}

			if( dr==null )
			{
				MySqlDataAdapter sqlda = new MySqlDataAdapter();
				MySqlCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetMySqlConnection();

				LoadCommand = new MySqlCommand("sp_Select_Skin", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new MySqlParameter(PK_SKIN_PARM, MySqlDbType.Int32,4));
				LoadCommand.Parameters[PK_SKIN_PARM].Value = PK_Skin;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"Skin");
				dr = ds.Tables["Skin"].Rows.Find(PK_Skin);
			}
			return dr;
		}

		public SkinData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand("SELECT * FROM Skin", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT * FROM Skin", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"Skin")==0 )
				return null;
			else
				return ds.Tables["Skin"].Rows;

		}

		public SkinData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,SKIN_TABLE);
		}
		public SkinData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new MySqlCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"Skin");
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, MySqlConnection conn, MySqlTransaction trans,string sTableName) {
			if( conn==null )
				conn = HADataConfiguration.GetMySqlConnection();
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand(sSQL, conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if(sqlda.Fill(ds,sTableName)==0 )
				return null;
			else
				return ds.Tables[sTableName].Rows;

		}

		public bool UpdateSkin(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, SkinData.SKIN_TABLE);
			return true;
		}

		public static bool UpdateSkin(ref MyDataSet ds, int CurUserID)
		{
			MySqlConnection OdbcConn = HADataConfiguration.GetMySqlConnection();
			return UpdateSkin(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateSkin(ref MyDataSet ds, int CurUserID,MySqlConnection OdbcConn,MySqlTransaction Trans)
		{
			DataTable dt = ds.Tables[SKIN_TABLE];
			if( dt == null )
				return false;

			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = null;
			MySqlCommand InsertCommand = null;
			MySqlCommand UpdateCommand = null;
			MySqlCommand DeleteCommand = null;
			CreateCommands(sqlda,OdbcConn, Trans, ref LoadCommand, ref InsertCommand, ref UpdateCommand, ref DeleteCommand);
			sqlda.RowUpdated += new MySqlRowUpdatedEventHandler(MyRowUpdated);

			sqlda.Update(dt);
			return true;
		}

		static void MyRowUpdated(Object sender, MySqlRowUpdatedEventArgs e)
		{
			if( e.StatementType==StatementType.Insert )
			{
				MySqlCommand ocmd = new MySqlCommand("SELECT @@IDENTITY", e.Command.Connection);
				int value = Int32.Parse(ocmd.ExecuteScalar().ToString());
				e.Row[0]=value;
				e.Row.AcceptChanges();
			}
		}

	} // public class SkinData
	public class SkinDataRow
	{
		public DataRow dr = null;
		public SkinDataRow(DataRow d)
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
		public System.Int32 fPK_Skin
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDescription
		{
			get
			{
				return Convert.ToString(dr[1]);
			}
			set
			{
				dr[1]=value;
			}
		}
		public System.String fDataSubdirectory
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
		public bool fDataSubdirectoryIsNull
		{
			get
			{
				return dr[2]==DBNull.Value;
			}
		}
		public void fDataSubdirectorySetNull()
		{
			dr[2]=DBNull.Value;
		}
		public System.Int32 fFK_Style
		{
			get
			{
				return Convert.ToInt32(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
		public bool fFK_StyleIsNull
		{
			get
			{
				return dr[3]==DBNull.Value;
			}
		}
		public void fFK_StyleSetNull()
		{
			dr[3]=DBNull.Value;
		}
		public StyleDataRow fFK_Style_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tStyle[Convert.ToInt32(dr[3])];
			}
		}
		public System.Int32 fFK_Skin_TextPlacement
		{
			get
			{
				return Convert.ToInt32(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public bool fFK_Skin_TextPlacementIsNull
		{
			get
			{
				return dr[4]==DBNull.Value;
			}
		}
		public void fFK_Skin_TextPlacementSetNull()
		{
			dr[4]=DBNull.Value;
		}
		public SkinDataRow fFK_Skin_TextPlacement_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tSkin[Convert.ToInt32(dr[4])];
			}
		}
		public System.Boolean fDrawTextBeforeChildren
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
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				return Convert.ToInt32(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
	} // public class SkinDataRow
	public class SkinDataReader
	{
		public MySqlDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public SkinDataReader(MySqlDataReader d)
		{
			dr=d;
		}
		public SkinDataReader(MySqlCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public SkinDataReader(MySqlCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public SkinDataReader(string sSQL)
		{
			MySqlConnection conn = HADataConfiguration.GetMySqlConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Skin WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public SkinDataReader(string sSQL,MySqlConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Skin WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public SkinDataReader(string sSQL,MySqlConnection conn,MySqlTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetMySqlConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM Skin WHERE " + sSQL;
			}

			MySqlCommand cmd = new MySqlCommand(sSQL,conn,trans);
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
				object[] objs = new object[7];
				for(int i=0;i<7;i++)
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
		public System.Int32 fPK_Skin
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.String fDescription
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[1]);
				else
					return Convert.ToString(dr[1]);
			}
		}
		public System.String fDataSubdirectory
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[2]);
				else
					return Convert.ToString(dr[2]);
			}
		}
		public bool fDataSubdirectoryIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[2]==DBNull.Value;
				else
					return dr[2]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Style
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public bool fFK_StyleIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[3]==DBNull.Value;
				else
					return dr[3]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Skin_TextPlacement
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[4]);
				else
					return Convert.ToInt32(dr[4]);
			}
		}
		public bool fFK_Skin_TextPlacementIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[4]==DBNull.Value;
				else
					return dr[4]==DBNull.Value;
			}
		}
		public System.Boolean fDrawTextBeforeChildren
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[5]);
				else
					return Convert.ToBoolean(dr[5]);
			}
		}
		public System.Int32 fFK_StabilityStatus
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[6]);
				else
					return Convert.ToInt32(dr[6]);
			}
		}
	} // public class SkinDataReader
	public class SkinTable : DataTable
	{
		public SkinTable() : base("Skin") {}

		public SkinDataRow this [System.Int32 PK_Skin]
		{
			get
			{
				SkinDataRow dr = new SkinDataRow(Rows.Find(PK_Skin));
				return dr;
			}
		}
		public DataRowCollection LoadAll(MySqlConnection conn, MySqlTransaction trans)
		{
			MySqlDataAdapter sqlda = new MySqlDataAdapter();
			MySqlCommand LoadCommand = new MySqlCommand("SELECT PK_Skin,Description,DataSubdirectory,FK_Style,FK_Skin_TextPlacement,DrawTextBeforeChildren,FK_StabilityStatus FROM Skin", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"Skin")==0 )
				return null;
			else
				return Rows;
		}
		public void Update(int PK_Users)
		{
			Update(PK_Users,((MyDataSet) DataSet).m_conn,((MyDataSet) DataSet).m_trans);
		}
		public void Update(int PK_Users,MySqlConnection conn, MySqlTransaction trans)
		{
			if( conn==null )
				return;
			MyDataSet ds = (MyDataSet) this.DataSet;
			SkinData.UpdateSkin(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_Skin
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cDescription
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cDataSubdirectory
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cFK_Style
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cFK_Skin_TextPlacement
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cDrawTextBeforeChildren
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cFK_StabilityStatus
		{
			get
			{
				return Columns[6];
			}
		}
	}
} // namespace HAData.Common.Data
