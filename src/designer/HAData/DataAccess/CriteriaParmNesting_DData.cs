namespace HAData.DataAccess {
	using System;
	using System.Data;
	using Microsoft.Data.Odbc;
	using System.Collections;

	using HAData.Common;

	public class CriteriaParmNesting_DData : MyDataSet {
		//
		// CriteriaParmNesting_D table constants
		//
		public const String CRITERIAPARMNESTING_D_TABLE = "CriteriaParmNesting_D";
		public const String PK_CRITERIAPARMNESTING_D_FIELD = "PK_CriteriaParmNesting_D";
		public const String FK_CRITERIAPARMNESTING_D_PARENT_FIELD = "FK_CriteriaParmNesting_D_Parent";
		public const String ISAND_FIELD = "IsAnd";
		public const String ISNOT_FIELD = "IsNot";
		// table+field constants
		public const String PK_CRITERIAPARMNESTING_D_TABLE_FIELD = "CriteriaParmNesting_D.PK_CriteriaParmNesting_D";
		public const String FK_CRITERIAPARMNESTING_D_PARENT_TABLE_FIELD = "CriteriaParmNesting_D.FK_CriteriaParmNesting_D_Parent";
		public const String ISAND_TABLE_FIELD = "CriteriaParmNesting_D.IsAnd";
		public const String ISNOT_TABLE_FIELD = "CriteriaParmNesting_D.IsNot";
		// DataSetCommand object
		protected OdbcDataAdapter m_DSCommand;

		// Stored procedure parameters
		protected const String PK_CRITERIAPARMNESTING_D_PARM = "@PK_CriteriaParmNesting_D";
		protected const String FK_CRITERIAPARMNESTING_D_PARENT_PARM = "@FK_CriteriaParmNesting_D_Parent";
		protected const String ISAND_PARM = "@IsAnd";
		protected const String ISNOT_PARM = "@IsNot";
		protected const String USERID_PARM = "@UserID";

		protected OdbcCommand m_LoadCommand;
		protected OdbcCommand m_InsertCommand;
		protected OdbcCommand m_UpdateCommand;
		protected OdbcCommand m_DeleteCommand;
		protected OdbcConnection m_Connection;
		protected OdbcTransaction m_Transaction;
		public DataTable Table { get { return Tables[0]; } }


		public CriteriaParmNesting_DData() {  // marker:1
			//
			// Create the tables in the dataset
			//
			Tables.Add(BuildDataTables());
			m_Connection = HADataConfiguration.GetOdbcConnection();
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaParmNesting_DData.CRITERIAPARMNESTING_D_TABLE);
		}

		public CriteriaParmNesting_DData(OdbcConnection conn,OdbcTransaction trans) {

			m_Connection = conn;
			m_Transaction = trans;
			CreateCommands(m_Connection, m_Transaction, ref m_LoadCommand, ref m_InsertCommand, ref m_UpdateCommand, ref m_DeleteCommand);
			// Create our DataSetCommand
			m_DSCommand = new OdbcDataAdapter();

			m_DSCommand.TableMappings.Add("Table", CriteriaParmNesting_DData.CRITERIAPARMNESTING_D_TABLE);
		}

		private CriteriaParmNesting_DData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context) {
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
			return (DataTable) BuildCriteriaParmNesting_DTable();
		}
		public static CriteriaParmNesting_DTable BuildCriteriaParmNesting_DTable() {
			//
			// Create the CriteriaParmNesting_D table
			//
			CriteriaParmNesting_DTable Table = new CriteriaParmNesting_DTable();
			DataColumnCollection Columns = Table.Columns;
			DataColumn[] PKColumns = new DataColumn[1];

			DataColumn Column = Columns.Add(PK_CRITERIAPARMNESTING_D_FIELD, typeof(System.Int32));
			Column.AutoIncrement = true;
			Column.AutoIncrementSeed = -1;
			Column.AutoIncrementStep = -1;
			PKColumns[0] = Column;

			Columns.Add(FK_CRITERIAPARMNESTING_D_PARENT_FIELD, typeof(System.Int32));
			Column = Columns.Add(ISAND_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Column = Columns.Add(ISNOT_FIELD, typeof(System.Boolean));
			Column.AllowDBNull = false;
			Column.DefaultValue = 0;

			Table.PrimaryKey = PKColumns;

			return Table;
		}
		protected static void CreateParameters(OdbcParameterCollection Params, bool IsInsert) {
			Params.Add(new OdbcParameter(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(FK_CRITERIAPARMNESTING_D_PARENT_PARM, OdbcType.Int,4));
			Params.Add(new OdbcParameter(ISAND_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(ISNOT_PARM, OdbcType.Bit,1));
			Params.Add(new OdbcParameter(USERID_PARM, OdbcType.Int));

			// map the parameters to the data table

			Params[PK_CRITERIAPARMNESTING_D_PARM].SourceColumn = CriteriaParmNesting_DData.PK_CRITERIAPARMNESTING_D_FIELD;
			if(IsInsert) {
				Params[PK_CRITERIAPARMNESTING_D_PARM].Direction = ParameterDirection.Output;
			}

			Params[FK_CRITERIAPARMNESTING_D_PARENT_PARM].SourceColumn = CriteriaParmNesting_DData.FK_CRITERIAPARMNESTING_D_PARENT_FIELD;
			Params[ISAND_PARM].SourceColumn = CriteriaParmNesting_DData.ISAND_FIELD;
			Params[ISNOT_PARM].SourceColumn = CriteriaParmNesting_DData.ISNOT_FIELD;
		}

		protected static void CreateCommands(OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
			if(LoadCommand == null) {
				// Create the command since it's null
				LoadCommand = new OdbcCommand("sp_Select_CriteriaParmNesting_D", Conn);
				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));
			}

			if(InsertCommand == null) {
				// Create the command since it's null
				InsertCommand = new OdbcCommand("sp_Insert_CriteriaParmNesting_D", Conn);
				InsertCommand.CommandType = CommandType.StoredProcedure;
				InsertCommand.Transaction = Trans;

				OdbcParameterCollection Params = InsertCommand.Parameters;

				CreateParameters(Params, true);

			}

			if(UpdateCommand == null) {
				// Create the command since it's null
				UpdateCommand = new OdbcCommand("sp_Update_CriteriaParmNesting_D", Conn);
				UpdateCommand.CommandType = CommandType.StoredProcedure;
				UpdateCommand.Transaction = Trans;

				OdbcParameterCollection Params = UpdateCommand.Parameters;

				CreateParameters(Params, false);

			}
			if (DeleteCommand == null)
			{
				DeleteCommand = new OdbcCommand("sp_Delete_CriteriaParmNesting_D", Conn);
				DeleteCommand.CommandType = CommandType.StoredProcedure;
				DeleteCommand.Transaction = Trans;

				DeleteCommand.Parameters.Add(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4, PK_CRITERIAPARMNESTING_D_FIELD);
				DeleteCommand.Parameters.Add(USERID_PARM, OdbcType.Int);
			}
		}

		protected static void CreateCommands(OdbcDataAdapter odbcda,OdbcConnection Conn, OdbcTransaction Trans, ref OdbcCommand LoadCommand, ref OdbcCommand InsertCommand, ref OdbcCommand UpdateCommand, ref OdbcCommand DeleteCommand) {
				LoadCommand = new OdbcCommand("SELECT PK_CriteriaParmNesting_D,FK_CriteriaParmNesting_D_Parent,IsAnd,IsNot FROM CriteriaParmNesting_D", Conn);
				LoadCommand.Transaction = Trans;

				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));

			odbcda.SelectCommand = LoadCommand;
			OdbcCommandBuilder odbcCB = new OdbcCommandBuilder(odbcda);
			odbcCB.RefreshSchema();
			DeleteCommand = odbcCB.GetDeleteCommand();
			InsertCommand = odbcCB.GetInsertCommand();
			UpdateCommand = odbcCB.GetUpdateCommand();
		}

		public CriteriaParmNesting_DData LoadCriteriaParmNesting_D(System.Int32 PK_CriteriaParmNesting_D)
		{
			m_DSCommand.SelectCommand = m_LoadCommand;
			m_DSCommand.SelectCommand.Parameters[PK_CRITERIAPARMNESTING_D_PARM].Value = PK_CriteriaParmNesting_D;

			m_DSCommand.Fill(this);
			return this;
		}

		public static DataRowCollection LoadCriteriaParmNesting_DWithWhere(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, string WhereClause) // marker:2
		{
			DataRowCollection dr;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParmNesting_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}
			
			DataSet dsTemp = new MyDataSet();
			dsTemp.Tables.Add(BuildDataTables());
			
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			string sSQL = "SELECT PK_CriteriaParmNesting_D, FK_CriteriaParmNesting_D_Parent, IsAnd, IsNot FROM CriteriaParmNesting_D WHERE " + WhereClause;
			
			OdbcCommand LoadCommand = new OdbcCommand(sSQL,conn);
			
			if( trans!=null )
				LoadCommand.Transaction = trans;
			
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(dsTemp,"CriteriaParmNesting_D");
			
			dr=dsTemp.Tables["CriteriaParmNesting_D"].Rows;
			
			if( dr!=null )
				ds.Merge(dsTemp);
			
			return dr;
		}

		public static DataRow LoadNoCacheCriteriaParmNesting_D(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CriteriaParmNesting_D)
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParmNesting_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
			}

			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand;
			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();

			LoadCommand = new OdbcCommand("sp_Select_CriteriaParmNesting_D", conn);

			LoadCommand.CommandType = CommandType.StoredProcedure;
			LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));
			LoadCommand.Parameters[PK_CRITERIAPARMNESTING_D_PARM].Value = PK_CriteriaParmNesting_D;
			if( trans!=null )
				LoadCommand.Transaction = trans;
			sqlda.SelectCommand = LoadCommand;
			sqlda.Fill(ds,"CriteriaParmNesting_D");
				dr = ds.Tables["CriteriaParmNesting_D"].Rows.Find(PK_CriteriaParmNesting_D);
			return dr;
		}

		public static DataRow LoadCriteriaParmNesting_D(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans, System.Int32 PK_CriteriaParmNesting_D)  // marker:3
		{
			DataRow dr = null;
			if( ds==null )
			{
				ds = new MyDataSet();
				ds.Tables.Add(BuildDataTables());
			}
			else
			{
				DataTable dt = ds.Tables["CriteriaParmNesting_D"];
				if( dt==null )
					ds.Tables.Add(BuildDataTables());
				else
				dr = dt.Rows.Find(PK_CriteriaParmNesting_D);
			}

			if( dr==null )
			{
				OdbcDataAdapter sqlda = new OdbcDataAdapter();
				OdbcCommand LoadCommand;
				if( conn==null )
					conn = HADataConfiguration.GetOdbcConnection();

				LoadCommand = new OdbcCommand("sp_Select_CriteriaParmNesting_D", conn);

				LoadCommand.CommandType = CommandType.StoredProcedure;
				LoadCommand.Parameters.Add(new OdbcParameter(PK_CRITERIAPARMNESTING_D_PARM, OdbcType.Int,4));
				LoadCommand.Parameters[PK_CRITERIAPARMNESTING_D_PARM].Value = PK_CriteriaParmNesting_D;
				if( trans!=null )
					LoadCommand.Transaction = trans;
				sqlda.SelectCommand = LoadCommand;
				sqlda.Fill(ds,"CriteriaParmNesting_D");
				dr = ds.Tables["CriteriaParmNesting_D"].Rows.Find(PK_CriteriaParmNesting_D);
			}
			return dr;
		}

		public CriteriaParmNesting_DData LoadAll() {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand("SELECT * FROM CriteriaParmNesting_D", m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this);
			return this;

		}

		public static DataRowCollection LoadAll(ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {

			if( conn==null )
				conn = HADataConfiguration.GetOdbcConnection();
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT * FROM CriteriaParmNesting_D", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(ds,"CriteriaParmNesting_D")==0 )
				return null;
			else
				return ds.Tables["CriteriaParmNesting_D"].Rows;

		}

		public CriteriaParmNesting_DData ExecuteQuery(String sSQL) {
			return ExecuteQuery(sSQL,CRITERIAPARMNESTING_D_TABLE);
		}
		public CriteriaParmNesting_DData ExecuteQuery(String sSQL,String sTableName) {

			// Create the command since it's null
			m_DSCommand.SelectCommand = new OdbcCommand(sSQL, m_Connection);
			m_DSCommand.SelectCommand.CommandType = CommandType.Text;
			m_DSCommand.SelectCommand.Transaction = m_Transaction;

			m_DSCommand.Fill(this,sTableName);

			return this;
		}

		public static DataRowCollection ExecuteQuery(String sSQL,ref MyDataSet ds, OdbcConnection conn, OdbcTransaction trans) {
			return ExecuteQuery(sSQL,ref ds,conn,trans,"CriteriaParmNesting_D");
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

		public bool UpdateCriteriaParmNesting_D(int CurUserID) {
			m_DSCommand.UpdateCommand = m_UpdateCommand;
			m_DSCommand.UpdateCommand.Parameters[USERID_PARM].Value = CurUserID;

			m_DSCommand.InsertCommand = m_InsertCommand;
			m_DSCommand.InsertCommand.Parameters[USERID_PARM].Value = CurUserID;

			if( m_DeleteCommand != null )
			{
				m_DSCommand.DeleteCommand = m_DeleteCommand;
				m_DSCommand.DeleteCommand.Parameters[USERID_PARM].Value = CurUserID;
			}

			m_DSCommand.Update(this, CriteriaParmNesting_DData.CRITERIAPARMNESTING_D_TABLE);
			return true;
		}

		public static bool UpdateCriteriaParmNesting_D(ref MyDataSet ds, int CurUserID)
		{
			OdbcConnection OdbcConn = HADataConfiguration.GetOdbcConnection();
			return UpdateCriteriaParmNesting_D(ref ds,CurUserID,OdbcConn,null);
		}

		public static bool UpdateCriteriaParmNesting_D(ref MyDataSet ds, int CurUserID,OdbcConnection OdbcConn,OdbcTransaction Trans)
		{
			DataTable dt = ds.Tables[CRITERIAPARMNESTING_D_TABLE];
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

	} // public class CriteriaParmNesting_DData
	public class CriteriaParmNesting_DDataRow
	{
		public DataRow dr = null;
		public CriteriaParmNesting_DDataRow(DataRow d)
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
		public System.Int32 fPK_CriteriaParmNesting_D
		{
			get
			{
				return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CriteriaParmNesting_D_Parent
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
		public bool fFK_CriteriaParmNesting_D_ParentIsNull
		{
			get
			{
				return dr[1]==DBNull.Value;
			}
		}
		public void fFK_CriteriaParmNesting_D_ParentSetNull()
		{
			dr[1]=DBNull.Value;
		}
		public CriteriaParmNesting_DDataRow fFK_CriteriaParmNesting_D_Parent_DataRow
		{
			get
			{
				MyDataSet mds = (MyDataSet)dr.Table.DataSet;
				return mds.tCriteriaParmNesting_D[Convert.ToInt32(dr[1])];
			}
		}
		public System.Boolean fIsAnd
		{
			get
			{
				return Convert.ToBoolean(dr[2]);
			}
			set
			{
				dr[2]=value;
			}
		}
		public System.Boolean fIsNot
		{
			get
			{
				return Convert.ToBoolean(dr[3]);
			}
			set
			{
				dr[3]=value;
			}
		}
	} // public class CriteriaParmNesting_DDataRow
	public class CriteriaParmNesting_DDataReader
	{
		public OdbcDataReader dr;
		bool bCache=false;
		int iRecord=-1,iNumRecords=-1;
		ArrayList al = null;

		public CriteriaParmNesting_DDataReader(OdbcDataReader d)
		{
			dr=d;
		}
		public CriteriaParmNesting_DDataReader(OdbcCommand cmd)
		{
			dr = cmd.ExecuteReader();
		}

		public CriteriaParmNesting_DDataReader(OdbcCommand cmd,bool Cache)
		{
			dr = cmd.ExecuteReader();
			bCache=Cache;
			if( bCache )
				CacheAndClose();
		}

		public CriteriaParmNesting_DDataReader(string sSQL)
		{
			OdbcConnection conn = HADataConfiguration.GetOdbcConnection();

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParmNesting_D WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaParmNesting_DDataReader(string sSQL,OdbcConnection conn)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParmNesting_D WHERE " + sSQL;
			}

			OdbcCommand cmd = new OdbcCommand(sSQL,conn,null);
			dr = cmd.ExecuteReader();
		}

		public CriteriaParmNesting_DDataReader(string sSQL,OdbcConnection conn,OdbcTransaction trans,bool Cache)
		{
			if( conn==null )
			{
				conn = HADataConfiguration.GetOdbcConnection();
			}

			if( !sSQL.ToUpper().StartsWith("SELECT") )
			{
				sSQL = "SELECT * FROM CriteriaParmNesting_D WHERE " + sSQL;
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
				object[] objs = new object[4];
				for(int i=0;i<4;i++)
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
		public System.Int32 fPK_CriteriaParmNesting_D
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[0]);
				else
					return Convert.ToInt32(dr[0]);
			}
		}
		public System.Int32 fFK_CriteriaParmNesting_D_Parent
		{
			get
			{
				if( bCache )
					return Convert.ToInt32(((object[]) al[iRecord])[1]);
				else
					return Convert.ToInt32(dr[1]);
			}
		}
		public bool fFK_CriteriaParmNesting_D_ParentIsNull
		{
			get
			{
				if( bCache )
					return ((object[]) al[iRecord])[1]==DBNull.Value;
				else
					return dr[1]==DBNull.Value;
			}
		}
		public System.Boolean fIsAnd
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[2]);
				else
					return Convert.ToBoolean(dr[2]);
			}
		}
		public System.Boolean fIsNot
		{
			get
			{
				if( bCache )
					return Convert.ToBoolean(((object[]) al[iRecord])[3]);
				else
					return Convert.ToBoolean(dr[3]);
			}
		}
	} // public class CriteriaParmNesting_DDataReader
	public class CriteriaParmNesting_DTable : DataTable
	{
		public CriteriaParmNesting_DTable() : base("CriteriaParmNesting_D") {}

		public CriteriaParmNesting_DDataRow this [System.Int32 PK_CriteriaParmNesting_D]
		{
			get
			{
				CriteriaParmNesting_DDataRow dr = new CriteriaParmNesting_DDataRow(Rows.Find(PK_CriteriaParmNesting_D));
				if( !dr.bIsValid  && false /* can't do this with ODBC */  )
				{
					MyDataSet mds = (MyDataSet) DataSet;
					if( mds.m_conn==null )
						return dr;
					OdbcDataAdapter sqlda = new OdbcDataAdapter();
					OdbcCommand LoadCommand = new OdbcCommand("sp_Select_CriteriaParmNesting_D", mds.m_conn,mds.m_trans);
					LoadCommand.CommandType = CommandType.StoredProcedure;
					LoadCommand.Parameters.Add(new OdbcParameter("@PK_CriteriaParmNesting_D", OdbcType.Int,4));
					LoadCommand.Parameters["@PK_CriteriaParmNesting_D"].Value = PK_CriteriaParmNesting_D;
					sqlda.SelectCommand = LoadCommand;
					sqlda.Fill(mds,"CriteriaParmNesting_D");
					dr = new CriteriaParmNesting_DDataRow(Rows.Find(PK_CriteriaParmNesting_D));
				}
				return dr;
			}
		}
		public DataRowCollection LoadAll(OdbcConnection conn, OdbcTransaction trans)
		{
			OdbcDataAdapter sqlda = new OdbcDataAdapter();
			OdbcCommand LoadCommand = new OdbcCommand("SELECT PK_CriteriaParmNesting_D,FK_CriteriaParmNesting_D_Parent,IsAnd,IsNot FROM CriteriaParmNesting_D", conn);
			LoadCommand.CommandType = CommandType.Text;
			if( trans!=null )
				LoadCommand.Transaction = trans;

			sqlda.SelectCommand = LoadCommand;
			if( sqlda.Fill(this.DataSet,"CriteriaParmNesting_D")==0 )
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
			CriteriaParmNesting_DData.UpdateCriteriaParmNesting_D(ref ds,PK_Users,conn,trans);
		}
		public DataColumn cPK_CriteriaParmNesting_D
		{
			get
			{
				return Columns[0];
			}
		}
		public DataColumn cFK_CriteriaParmNesting_D_Parent
		{
			get
			{
				return Columns[1];
			}
		}
		public DataColumn cIsAnd
		{
			get
			{
				return Columns[2];
			}
		}
		public DataColumn cIsNot
		{
			get
			{
				return Columns[3];
			}
		}
	}
} // namespace HAData.Common.Data
