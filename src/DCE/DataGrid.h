#ifndef DATAGRID2_H
#define DATAGRID2_H

#include "PlutoUtils/GraphicFormat.h"
#include "Message.h"

// REQUEST DATA

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

namespace DCE
{

	class DataGridCellSerializableData
	{
	public:
		unsigned long m_GraphicLength, m_MessageLength, m_TextLength, m_ValueLength;
		int m_AltColor;
		enum eGraphicFormat m_GraphicFormat;
		bool m_bSelectable;
		int m_PK_StyleDetail;
		int m_Colspan, m_Rowspan;
		bool m_bDontFireGridEvent; // usefull for the cells with scroll arrows
	};

	class DataGridCell : public DataGridCellSerializableData
	{
	public:
		Message *m_pMessage;
		char *m_pGraphicData, *m_Text, *m_Value;

		// Unique value REQUIRED for multi-select lists.
		DataGridCell(string Text, string Value="");
		DataGridCell(int Size, char *Data);
		virtual ~DataGridCell();
		void Initialize();

		// Will display this image in the cell
		void SetImage(char *Data, int Length, enum eGraphicFormat Format);
		// Will fire off this message when touched.
		void SetMessage(Message *pMessage);
		const char *GetText();
		const char *GetValue();

		virtual void ToData(unsigned long &Size, char* &Data);
	};

	// We're currently supporting many more columns than rows, because the EPG grid with column span may need quite a bit of granularity, but
	// not much vertically.  We may opt to make this a 64 bit integer, but in the interest of speed we're trying it with 32.

#define MAKECOLROW(col, row) (((col)  << 14) | ((row) & 0x3FFF))

	typedef unsigned int ColRowType;
	typedef map<ColRowType, DataGridCell *> MemoryDataTable;

	class DataGridTableCellIndex
	{
	public:
		ColRowType m_ColRow;
		int m_Size;
	};

	class DataGridTableSerializableData
	{
	protected:
		int m_TotalColumns, m_TotalRows;
	public:
		// Starting = The first row/column relative to the full data in this chunk.
		// Count = The number of rows/columns in this snapshot of the data
		// Total = The number of rows/columns available in the full data.

		bool m_bKeepRowHeader, m_bKeepColumnHeader;
		int m_ColumnCount, m_RowCount;
		int m_StartingColumn, m_StartingRow;
		int m_CellCount;

		const int getTotalRowCount() const { return m_TotalRows; };
	};

	class DataGridTable : public DataGridTableSerializableData
	{
	public:
		// These are only used by a memory grid.
		MemoryDataTable m_MemoryDataTable;

		// These aren't serialized.  Just used internally by the plugin
		int m_iPK_Datagrid;
		string m_sOptions;
		bool m_bRePopulateEachTimeRequested; 

		DataGridTable();
		DataGridTable(int Size, char *Data);
		virtual ~DataGridTable();

		virtual int GetRows();  // The total number of rows in the whole grid
		virtual int GetCols();  // The total number of columns in the whole grid
		virtual DataGridCell *GetData(int Column, int Row, int *Size = NULL);
		//The caller is responsible for deleting the cell returned by this.
		virtual void SetData(int Column, int Row, DataGridCell *Value);
		virtual void ClearData();
		virtual void ToData(string GridID,int &Size, char* &Data);
		// Serialize just a chunk of the data.
		virtual void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
	};
}

#endif
