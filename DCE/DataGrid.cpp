/* 
	DataGrid
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/minilzo.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "DataGrid.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"


// Message parameter list.

#define DCELL_TEXT 1
#define DCELL_GRAPHIC_DATA 2
#define DCELL_GRAPHIC_FORMAT 3
#define DCELL_ONTOUCH_MESSAGE 4
#define DCELL_VALUE	5
#define DCELL_ALT_COLOR 6
#define DCELL_STYLE 7
#define DCELL_SELECTABLE 8

#define HEAP_ALLOC(var,size) \
        lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);

using namespace DCE;

void DataGridCell::Initialize()
{
	m_bSelectable = true;
	m_bDontFireGridEvent = false;
	m_PK_StyleDetail = 0;
	m_AltColor=0;
	m_Colspan=1;
	m_Rowspan=1;
	m_pMessage=NULL;	
	m_pGraphicData=NULL;
	m_Text = NULL;
	m_Value = NULL;
	m_GraphicLength = m_MessageLength = m_TextLength = m_ValueLength = 0;
	m_GraphicFormat = GR_UNKNOWN;
}

DataGridCell::~DataGridCell()
{
	if (m_Text)
		free(m_Text);
	if (m_Value)
		free(m_Value);

//	delete m_pMessage;
	delete m_pGraphicData;
}

DataGridCell::DataGridCell(string Text, string Value)
{
	Initialize();

#ifdef WINCE
	m_Text = const_cast<char *>((Text + Text).c_str());
#else
	m_Text = strdup(Text.c_str());
#endif

	m_TextLength = (int)Text.length();

#ifdef WINCE
	m_Value = const_cast<char *>((Value + Value).c_str());
#else
	m_Value = strdup(Value.c_str());
#endif
	
	m_ValueLength = (int)Value.length();
}

DataGridCell::DataGridCell(int Size, char *Data)
{
	Initialize();

	// The first chunk of the data is the serializable part

	memcpy((DataGridCellSerializableData *)this, Data, sizeof(DataGridCellSerializableData));
	Data+=sizeof(DataGridCellSerializableData);
	if (m_TextLength)
	{
		m_Text = (char *)malloc(m_TextLength+1);
		memcpy(m_Text, Data, m_TextLength);
		m_Text[m_TextLength]=0;
		Data+=m_TextLength;
	}
	if (m_ValueLength)
	{
		m_Value = (char *)malloc(m_ValueLength+1);
		memcpy(m_Value, Data, m_ValueLength);
		m_Value[m_ValueLength]=0;
		Data+=m_ValueLength;
	}
	if (m_GraphicLength)
	{
		m_pGraphicData = new char[m_GraphicLength];
		memcpy(m_pGraphicData, Data, m_GraphicLength);
		Data+=m_GraphicLength;
	}
	if (m_MessageLength)
	{
		m_pMessage = new Message(m_MessageLength, Data);
		Data+=m_MessageLength;
	}
}

void DataGridCell::SetImage(char *Data, int Length, enum eGraphicFormat Format)
{
	m_pGraphicData = Data;
	m_GraphicLength = Length;
	m_GraphicFormat = Format;
}

const char *DataGridCell::GetText()
{
	return (m_Text) ? (const char *)m_Text : "";
}

const char *DataGridCell::GetValue()
{
	return (m_Value) ? (const char *)m_Value : "";
}

void DataGridCell::SetMessage(Message* pMessage)
{	
	m_pMessage = pMessage;
}

void DataGridCell::ToData(unsigned long &Size, char* &Data)
{
	char *MessageData;
	m_MessageLength = 0;
	if (m_pMessage)
	{
		m_pMessage->ToData(m_MessageLength, MessageData);
	}
	Size = sizeof(DataGridCellSerializableData)+m_TextLength+m_ValueLength+m_GraphicLength+m_MessageLength;
	Data = new char[Size];
	char *Datap = Data;
	memcpy(Datap, (DataGridCellSerializableData *)this, sizeof(DataGridCellSerializableData));
	Datap+=sizeof(DataGridCellSerializableData);
	if (m_TextLength)
	{
		memcpy(Datap, m_Text, m_TextLength);
		Datap+=m_TextLength;
	}
	if (m_ValueLength)
	{
		memcpy(Datap, m_Value, m_ValueLength);
		Datap+=m_ValueLength;
	}
	if (m_GraphicLength)
	{
		memcpy(Datap, m_pGraphicData, m_GraphicLength);
		Datap+=m_GraphicLength;
	}
	if (m_MessageLength)
	{
		memcpy(Datap, MessageData, m_MessageLength);
		Datap+=m_MessageLength;
	}
}


DataGridTable::DataGridTable()
{
	Message::Message();
	ClearData();
}

DataGridTable::DataGridTable(int Size, char *Data) 
{
#ifndef DISABLE_LZO_DATAGRID
	int UncompressedLength = *((int *)Data);
	char *UncompressedData = new char[UncompressedLength];
	lzo_uint new_len;

#ifdef UNDER_CE
	__try
	{
		lzo1x_decompress((lzo_byte *)Data+4,Size - 4,(lzo_byte *)UncompressedData,&new_len,NULL);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//g_pPlutoLogger->Write(LV_CRITICAL,"lzo decompress data failed %p %d %p",
		//	Data,Size,UncompressedData);
	}
#else
	lzo1x_decompress((lzo_byte *)Data+4,Size - 4,(lzo_byte *)UncompressedData,&new_len,NULL);
#endif

#else
	int UncompressedLength = Size;
	char *UncompressedData = Data;
#endif

	char *Datap = UncompressedData;
	memcpy((DataGridTableSerializableData *)this, Datap, sizeof(DataGridTableSerializableData));
	Datap+=sizeof(DataGridTableSerializableData);
	int IndexSize = sizeof(DataGridTableCellIndex) * m_CellCount;
	DataGridTableCellIndex *ntIndex = new DataGridTableCellIndex[m_CellCount];
	memcpy(ntIndex, Datap, IndexSize);
	Datap+=IndexSize;
    for(int i=0;i<m_CellCount;++i)
	{
		int CellSize = ntIndex[i].m_Size;
		DataGridCell *pNewCell = new DataGridCell(CellSize, Datap);
		Datap+=CellSize;
		m_MemoryDataTable[ntIndex[i].m_ColRow]=pNewCell;
	}

	delete[] ntIndex;
#ifndef DISABLE_LZO_DATAGRID
	delete[] UncompressedData;
#endif
}

DataGridTable::~DataGridTable()
{
	ClearData();
}

int DataGridTable::GetRows()
{
	return m_TotalRows;
}

int DataGridTable::GetCols()
{
	return m_TotalColumns;
}

DataGridCell *DataGridTable::GetData(int Column, int Row, int *Size)
{
	MemoryDataTable::iterator iDR = m_MemoryDataTable.find(MAKECOLROW(Column, Row));
	if (iDR!=m_MemoryDataTable.end())
	{
		char *pData;
		unsigned long nSize;

		(*iDR).second->ToData(nSize, pData);
		DataGridCell *pNewCell = new DataGridCell(nSize, pData);
		
		if (Size)
			*Size = nSize;

		delete[] pData;
		return pNewCell;
	}
	return NULL;
}

void DataGridTable::SetData(int Column, int Row, DataGridCell *Value)
{
	if (Column < 0 || Row <0)
		return;

	if (Row+1 > m_TotalRows)
		m_TotalRows = Row+1;

	if (Column >= m_ColumnCount+1+m_StartingColumn)
		m_ColumnCount = Column+1-m_StartingColumn;

	if (Column+1 > m_TotalColumns)
		m_TotalColumns = Column+1;

	m_MemoryDataTable[MAKECOLROW(Column, Row)]=Value;
}

void DataGridTable::ClearData()
{
	for(MemoryDataTable::iterator iDR = m_MemoryDataTable.begin();iDR != m_MemoryDataTable.end(); ++iDR)
	{
        delete (*iDR).second;
	}
	m_ColumnCount = 0;
	m_TotalColumns = 0;
	m_StartingColumn = 0;
	m_TotalRows = 0;
	m_RowCount = 0;
	m_StartingRow = 0;
	m_MemoryDataTable.clear();
}   

void DataGridTable::ToData(string GridID,int &Size, char* &Data)
{
	// Default: Serialize the whole table.
	ToData(GridID, Size, Data, 0, 0, GetCols(), GetRows());
}

void DataGridTable::ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount)
{
	m_CellCount = 0;
	int MaxCell = ColCount * RowCount;
	DataGridTableCellIndex *ntIndex = new DataGridTableCellIndex[MaxCell];
	DataGridCell **ntCells = new DataGridCell*[MaxCell];
	int ntTotalCellSize = 0, CurrentCellSize;

	// Set the subset-related parameters. 
	m_StartingRow = RowStart;
	m_StartingColumn = ColStart;
	int GridRowsLeft = GetRows() - RowStart;
	m_RowCount = RowCount < GridRowsLeft ? RowCount : GridRowsLeft;
	m_ColumnCount = ColCount;

	int ActualFirstColumn = m_bKeepColumnHeader ? 1 : 0;
	register int i;

	for(i=0;i<m_RowCount;++i)
	{
		for(register int j=0;j<m_ColumnCount;++j)
		{
			DataGridCell *Cell = GetData((j==0 && m_bKeepColumnHeader) ? 0 : j+m_StartingColumn, (i==0 && m_bKeepRowHeader) ? 0 : i+m_StartingRow, &CurrentCellSize);
			if (!Cell && j==ActualFirstColumn)
			{
				// Hmm, no data on the first cell.  Maybe this is the middle of a columnspan?  If so
				// find the column and make the adjustment.
				for(int cssearch = m_StartingColumn+ActualFirstColumn-1; cssearch>=ActualFirstColumn; cssearch--)
				{
					Cell = GetData(cssearch, (i==0 && m_bKeepRowHeader) ? 0 : i+m_StartingRow, &CurrentCellSize);
                    if (Cell)
					{
						if (Cell->m_Colspan >= (1 + (m_StartingColumn-cssearch)-ActualFirstColumn))
						{
							Cell->m_Colspan -= (m_StartingColumn-cssearch+1);
							// This will work, break out of the loop and use it.
							break;
						}
						else
						{
							// There's a cell, but its column span doesn't reach. Discard and cancel.
							delete Cell;
							Cell = NULL;
							break;
						}
					}
				}

			}
			if (Cell)
			{
				if (Cell->m_Colspan + j > m_ColumnCount)
				{
					Cell->m_Colspan = m_ColumnCount - j;
				}
				ntIndex[m_CellCount].m_ColRow = MAKECOLROW((j==0 && m_bKeepColumnHeader) ? 0 : j+m_StartingColumn, (i==0 && m_bKeepRowHeader) ? 0 : i+m_StartingRow);
				ntIndex[m_CellCount].m_Size = CurrentCellSize;
				ntCells[m_CellCount++] = Cell;
				ntTotalCellSize += CurrentCellSize;
			}
		}
	}

	int IndexSize = sizeof(DataGridTableCellIndex) * m_CellCount;

	int UncompressedSize = 	(sizeof(DataGridTableSerializableData)) + // The serializable data
							(IndexSize) + // The index
							(ntTotalCellSize);  // The cell data itself

	char *UncompressedData = new char[UncompressedSize];
	char *Datap = UncompressedData;


	memcpy(Datap, (DataGridTableSerializableData *)this, sizeof(DataGridTableSerializableData));
	Datap+=sizeof(DataGridTableSerializableData);
	memcpy(Datap, ntIndex, IndexSize);
	Datap+=IndexSize;
    for(i=0;i<m_CellCount;++i)
	{
		unsigned long CellSize;
		char *CellData;
		ntCells[i]->ToData(CellSize, CellData);

		if (CellSize != ntIndex[i].m_Size)
		{
			printf("ERROR: Cell is of unexpected size!");
			exit(0);
		}
		memcpy(Datap, CellData, CellSize);
		Datap+=CellSize;
		delete[] CellData;
	}

	delete[] ntIndex;
	delete[] ntCells;
#ifndef DISABLE_LZO_DATAGRID
	Data = new char[UncompressedSize + UncompressedSize / 64 + 16 + 3 + 4];
    lzo_uint out_len;
	*((int *)Data)=UncompressedSize;
    lzo1x_1_compress((lzo_byte *)UncompressedData,UncompressedSize,(lzo_byte *)Data+4,&out_len,wrkmem);
	Size = (int)out_len+4;
	// Shouldn't this have been here?
	delete[] UncompressedData;

#else
	Data = UncompressedData;
	Size = UncompressedSize;
#endif

}
