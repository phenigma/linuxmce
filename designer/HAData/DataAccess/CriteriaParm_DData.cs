namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class CriteriaParm_DData : MyDataSet {
		//
		// CriteriaParm_D table constants
		//
		public const String CRITERIAPARM_D_TABLE = "CriteriaParm_D";
		public const String PK_CRITERIAPARM_FIELD = "PK_CriteriaParm";
		public const String FK_CRITERIAPARMNESTING_D_FIELD = "FK_CriteriaParmNesting_D";
		public const String FK_CRITERIAPARMLIST_FIELD = "FK_CriteriaParmList";
		public const String OPERATOR_FIELD = "Operator";
		public const String VALUE_FIELD = "Value";
		public const String FK_CANNEDEVENTS_CRITERIAPARMLIST_FIELD = "FK_CannedEvents_CriteriaParmList";
		public const String MODIFICATION_RECORDINFO_FIELD = "Modification_RecordInfo";
		public const String ISNEW_RECORDINFO_FIELD = "IsNew_RecordInfo";
		public const String ISDELETED_RECORDINFO_FIELD = "IsDeleted_RecordInfo";
		public const String FK_USERS_RECORDINFO_FIELD = "FK_Users_RecordInfo";
		// table+field constants
		public const String PK_CRITERIAPARM_TABLE_FIELD = "CriteriaParm_D.PK_CriteriaParm";
		public const String FK_CRITERIAPARMNESTING_D_TABLE_FIELD = "CriteriaParm_D.FK_CriteriaParmNesting_D";
		public const String FK_CRITERIAPARMLIST_TABLE_FIELD = "CriteriaParm_D.FK_CriteriaParmList";
		public const String OPERATOR_TABLE_FIELD = "CriteriaParm_D.Operator";
		public const String VALUE_TABLE_FIELD = "CriteriaParm_D.Value";
		public const String FK_CANNEDEVENTS_CRITERIAPARMLIST_TABLE_FIELD = "CriteriaParm_D.FK_CannedEvents_CriteriaParmList";
		public const String MODIFICATION_RECORDINFO_TABLE_FIELD = "CriteriaParm_D.Modification_RecordInfo";
		public const String ISNEW_RECORDINFO_TABLE_FIELD = "CriteriaParm_D.IsNew_RecordInfo";
		public const String ISDELETED_RECORDINFO_TABLE_FIELD = "CriteriaParm_D.IsDeleted_RecordInfo";
		public const String FK_USERS_RECORDINFO_TABLE_FIELD = "CriteriaParm_D.FK_Users_RecordInfo";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_CRITERIAPARM_PARM = "@PK_CriteriaParm";
		protected const String FK_CRITERIAPARMNESTING_D_PARM = "@FK_CriteriaParmNesting_D";
		protected const String FK_CRITERIAPARMLIST_PARM = "@FK_CriteriaParmList";
		protected const String OPERATOR_PARM = "@Operator";
		protected const String VALUE_PARM = "@Value";
		protected const String FK_CANNEDEVENTS_CRITERIAPARMLIST_PARM = "@FK_CannedEvents_CriteriaParmList";
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


		public CriteriaParm_DData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaParm_DData.CRITERIAPARM_D_TABLE);
		}

		public CriteriaParm_DData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaParm_DData.CRITERIAPARM_D_TABLE);
		}

		private CriteriaParm_DData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCriteriaParm_DTable();
		}
		public static CriteriaParm_DTable BuildCriteriaParm_DTable() {
			//
			// Create the CriteriaParm_D table
			//
			CriteriaParm_DTable Table = new CriteriaParm_DTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_CRITERIAPARM_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Column = Columns.Add(FK_CRITERIAPARMNESTING_D_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(FK_CRITERIAPARMLIST_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(OPERATOR_FIELD, typeof(System.Int32));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(VALUE_FIELD, typeof(System.String));
			Column.AllowDBNull = false;

			Columns.Add(FK_CANNEDEVENTS_CRITERIAPARMLIST_FIELD, typeof(System.Int32));
			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_CRITERIAPARM_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(OPERATOR_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(VALUE_PARM, OdbcType.VarChar, 50));
			Params.Add(new OdbcParameter(FK_CANNEDEVENTS_CRITERIAPARMLIST_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(MODIFICATION_RECORDINFO_PARM, OdbcType.DateTime,4));
			Params.Add(new OdbcParameter(ISNEW_RECORDINFO_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ISDELETED_RECORDINFO_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(FK_USERS_RECORDINFO_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_CRITERIAPARM_PARM].SourceColumn = CriteriaParm_DData.PK_CRITERIAPARM_FIELD;
			if(IsInsert) {
				Params[PK_CRITERIAPARM_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_CRITERIAPARMNESTING_D_PARM].SourceColumn = CriteriaParm_DData.FK_CRITERIAPARMNESTING_D_FIELD;
			Params[FK_CRITERIAPARMLIST_PARM].SourceColumn = CriteriaParm_DData.FK_CRITERIAPARMLIST_FIELD;
			Params[OPERATOR_PARM].SourceColumn = CriteriaParm_DData.OPERATOR_FIELD;
			Params[VALUE_PARM].SourceColumn = CriteriaParm_DData.VALUE_FIELD;
			Params[FK_CANNEDEVENTS_CRITERIAPARMLIST_PARM].SourceColumn = CriteriaParm_DData.FK_CANNEDEVENTS_CRITERIAPARMLIST_FIELD;
			Params[MODIFICATION_RECORDINFO_PARM].SourceColumn = CriteriaParm_DData.MODIFICATION_RECORDINFO_FIELD;
			Params[ISNEW_RECORDINFO_PARM].SourceColumn = CriteriaParm_DData.ISNEW_RECORDINFO_FIELD;
			Params[ISDELETED_RECORDINFO_PARM].SourceColumn = CriteriaParm_DData.ISDELETED_RECORDINFO_FIELD;
			Params[FK_USERS_RECORDINFO_PARM].SourceColumn = CriteriaParm_DData.FK_USERS_RECORDINFO_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_CriteriaParm_D", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARM_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_CriteriaParm_D", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_CriteriaParm_D", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_CriteriaParm_D", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_CRITERIAPARM_PARM, OdbcType.Int,4, PK_CRITERIAPARM_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_CriteriaParm,FK_CriteriaParmNesting_D,FK_CriteriaParmList,Operator,Value,FK_CannedEvents_CriteriaParmList FROM CriteriaParm_D", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARM_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CriteriaParm_DData LoadCriteriaParm_D(System.Int32 PK_CriteriaParm)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_CRITERIAPARM_PARM].Value = PK_CriteriaParm;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCriteriaParm_DWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParm_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_CriteriaParm, FK_CriteriaParmNesting_D, FK_CriteriaParmList, Operator, Value, FK_CannedEvents_CriteriaParmList, Modification_RecordInfo, IsNew_RecordInfo, IsDeleted_RecordInfo, FK_Users_RecordInfo FROM CriteriaParm_D WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CriteriaParm_D");
			
			dr=dsTemp.Tables["CriteriaParm_D"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCriteriaParm_D(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CriteriaParm)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParm_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_CriteriaParm_D", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARM_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_CRITERIAPARM_PARM].Value = PK_CriteriaParm;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CriteriaParm_D");
				dr = ds.Tables["CriteriaParm_D"].Rows.Find(PK_CriteriaParm);
			return dr;
		}

		public static DataRow LoadCriteriaParm_D(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CriteriaParm)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParm_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_CriteriaParm);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_CriteriaParm_D", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARM_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_CRITERIAPARM_PARM].Value = PK_CriteriaParm;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CriteriaParm_D");
				dr = ds.Tables["CriteriaParm_D"].Rows.Find(PK_CriteriaParm);
			}
			return dr;
		}

		public CriteriaParm_DData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM CriteriaParm_D", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM CriteriaParm_D", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CriteriaParm_D")==0 )
				return null;
			else
				return ds.Tables["CriteriaParm_D"].Rows;

		}

		public CriteriaParm_DData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,CRITERIAPARM_D_TABLE);
		}
		public CriteriaParm_DData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CriteriaParm_D");
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

		public bool UpdateCriteriaParm_D(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CriteriaParm_DData.CRITERIAPARM_D_TABLE);
			return true;
		}

		public static bool UpdateCriteriaParm_D(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCriteriaParm_D(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCriteriaParm_D(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[CRITERIAPARM_D_TABLE];
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

	} // public class CriteriaParm_DData
	public class CriteriaParm_DDataRow
	{
		public DataRow dr = null;
		public CriteriaParm_DDataRow(DataRow d)
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
		public System.Int32 fPK_CriteriaParm
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CriteriaParmNesting_D
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
		public CriteriaParmNesting_DDataRow fFK_CriteriaParmNesting_D_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCriteriaParmNesting_D[Convert.ToInt32(dr[1])];
			}
		}
		public System.Int32 fFK_CriteriaParmList
		{
			get
			{
				return Convert.ToInt32(dr[2]);
			}
			set
			{
				dr[2]=value;
			}
		}
		public CriteriaParmListDataRow fFK_CriteriaParmList_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCriteriaParmList[Convert.ToInt32(dr[2])];
			}
		}
		public System.Int32 fOperator
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
		public System.String fValue
		{
			get
			{
				return Convert.ToString(dr[4]);
			}
			set
			{
				dr[4]=value;
			}
		}
		public System.Int32 fFK_CannedEvents_CriteriaParmList
		{
			get
			{
				return Convert.ToInt32(dr[5]);
			}
			set
			{
				dr[5]=value;
			}
		}
		public bool fFK_CannedEvents_CriteriaParmListIsNull
		{
			get
			{
				return dr[5]==DBNull.Value;
			}
		}
		public void fFK_CannedEvents_CriteriaParmListSetNull()
		{
			dr[5]=DBNull.Value;
		}
		public System.DateTime fModification_RecordInfo
		{
			get
			{
				return Convert.ToDateTime(dr[6]);
			}
			set
			{
				dr[6]=value;
			}
		}
		public bool fModification_RecordInfoIsNull
		{
			get
			{
				return dr[6]==DBNull.Value;
			}
		}
		public void fModification_RecordInfoSetNull()
		{
			dr[6]=DBNull.Value;
		}
		public System.Boolean fIsNew_RecordInfo
		{
			get
			{
				return Convert.ToBoolean(dr[7]);
			}
			set
			{
				dr[7]=value;
			}
		}
		public bool fIsNew_RecordInfoIsNull
		{
			get
			{
				return dr[7]==DBNull.Value;
			}
		}
		public void fIsNew_RecordInfoSetNull()
		{
			dr[7]=DBNull.Value;
		}
		public System.Boolean fIsDeleted_RecordInfo
		{
			get
			{
				return Convert.ToBoolean(dr[8]);
			}
			set
			{
				dr[8]=value;
			}
		}
		public bool fIsDeleted_RecordInfoIsNull
		{
			get
			{
				return dr[8]==DBNull.Value;
			}
		}
		public void fIsDeleted_RecordInfoSetNull()
		{
			dr[8]=DBNull.Value;
		}
		public System.Int32 fFK_Users_RecordInfo
		{
			get
			{
				return Convert.ToInt32(dr[9]);
			}
			set
			{
				dr[9]=value;
			}
		}
		public bool fFK_Users_RecordInfoIsNull
		{
			get
			{
				return dr[9]==DBNull.Value;
			}
		}
		public void fFK_Users_RecordInfoSetNull()
		{
			dr[9]=DBNull.Value;
		}
		public UsersDataRow fFK_Users_RecordInfo_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tUsers[Convert.ToInt32(dr[9])];
			}
		}
	} // public class CriteriaParm_DDataRow
	public class CriteriaParm_DDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CriteriaParm_DDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public CriteriaParm_DDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CriteriaParm_DDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CriteriaParm_DDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParm_D WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaParm_DDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParm_D WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaParm_DDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParm_D WHERE " + sSQL;
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
				object[] objs = new object[10];
				for(int i=0;i<10;i++)
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
		public System.Int32 fPK_CriteriaParm
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CriteriaParmNesting_D
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public System.Int32 fFK_CriteriaParmList
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[2]);
				else
					return Convert.ToInt32(dr[2]);
			}
		}
		public System.Int32 fOperator
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[3]);
				else
					return Convert.ToInt32(dr[3]);
			}
		}
		public System.String fValue
		{
			get
			{
				if( bCache )
					return Convert.ToString(((object[]) al[iRecord])[4]);
				else
					return Convert.ToString(dr[4]);
			}
		}
		public System.Int32 fFK_CannedEvents_CriteriaParmList
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[5]);
				else
					return Convert.ToInt32(dr[5]);
			}
		}
		public bool fFK_CannedEvents_CriteriaParmListIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[5]==DBNull.Value;
				else
					return dr[5]==DBNull.Value;
			}
		}
		public System.DateTime fModification_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToDateTime(((object[]) al[iRecord])[6]);
				else
					return Convert.ToDateTime(dr[6]);
			}
		}
		public bool fModification_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[6]==DBNull.Value;
				else
					return dr[6]==DBNull.Value;
			}
		}
		public System.Boolean fIsNew_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[7]);
				else
					return Convert.ToBoolean(dr[7]);
			}
		}
		public bool fIsNew_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[7]==DBNull.Value;
				else
					return dr[7]==DBNull.Value;
			}
		}
		public System.Boolean fIsDeleted_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[8]);
				else
					return Convert.ToBoolean(dr[8]);
			}
		}
		public bool fIsDeleted_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[8]==DBNull.Value;
				else
					return dr[8]==DBNull.Value;
			}
		}
		public System.Int32 fFK_Users_RecordInfo
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[9]);
				else
					return Convert.ToInt32(dr[9]);
			}
		}
		public bool fFK_Users_RecordInfoIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[9]==DBNull.Value;
				else
					return dr[9]==DBNull.Value;
			}
		}
	} // public class CriteriaParm_DDataReader
	public class CriteriaParm_DTable : DataTable
	{
		public CriteriaParm_DTable() : base("CriteriaParm_D") {}

		public CriteriaParm_DDataRow this [System.Int32 PK_CriteriaParm]
		{
			get
			{
				CriteriaParm_DDataRow dr = new CriteriaParm_DDataRow(Rows.Find(PK_CriteriaParm));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_CriteriaParm_D", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_CriteriaParm", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_CriteriaParm"].Value = PK_CriteriaParm;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"CriteriaParm_D");
					dr = new CriteriaParm_DDataRow(Rows.Find(PK_CriteriaParm));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_CriteriaParm,FK_CriteriaParmNesting_D,FK_CriteriaParmList,Operator,Value,FK_CannedEvents_CriteriaParmList FROM CriteriaParm_D", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CriteriaParm_D")==0 )
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
			CriteriaParm_DData.UpdateCriteriaParm_D(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_CriteriaParm
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_CriteriaParmNesting_D
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cFK_CriteriaParmList
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cOperator
		{
			get
			{
				return Columns[3];
			}
		}
		public DataColumn cValue
		{
			get
			{
				return Columns[4];
			}
		}
		public DataColumn cFK_CannedEvents_CriteriaParmList
		{
			get
			{
				return Columns[5];
			}
		}
		public DataColumn cModification_RecordInfo
		{
			get
			{
				return Columns[6];
			}
		}
		public DataColumn cIsNew_RecordInfo
		{
			get
			{
				return Columns[7];
			}
		}
		public DataColumn cIsDeleted_RecordInfo
		{
			get
			{
				return Columns[8];
			}
		}
		public DataColumn cFK_Users_RecordInfo
		{
			get
			{
				return Columns[9];
			}
		}
	}
} // namespace HAData.Common.Data
