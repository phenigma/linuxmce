#include "PlutoMOGridEngine.h"

#include <akniconarray.h> // CAknIconArray
#include <aknlists.h> // AknListBoxLayouts
#include <barsread.h> // TResourceReader
#include <PlutoMO.mbg> // contains icon enumeration
#include <stringloader.h> // StringLoader

const TInt KNumberOfIcons(7);
const TInt KGraphicsHeight = 50;
const TInt KTextColor = 215;

void CPlutoMOGridEngine::ConstructL(TInt aGridResource, TInt aIconFileResource)
	{

	// Construct the grid from resource
	TResourceReader reader;
	CEikonEnv::Static()->CreateResourceReaderLC(reader, aGridResource);
	ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // reader

	// Set up the icons
	SetupGridIconsL (aIconFileResource);
	
	// Draw the grid
	SizeChanged();	
	}


void CPlutoMOGridEngine::SetupGridIconsL(TInt aIconFileResource)
	{
	
	HBufC* iconFileName;
	iconFileName = StringLoader::LoadLC(aIconFileResource);	
	
	// Create an array of icons, reading them from the file
	CArrayPtr<CGulIcon>* icons = new(ELeave) CAknIconArray(KNumberOfIcons);
	CleanupStack::PushL(icons);
	
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOSnap, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOGolf, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOClock, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOAces, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOHearts, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMORummy, EMbmPlutoMOCard_mask));
	icons->AppendL(iEikonEnv->CreateIconL(*iconFileName, EMbmPlutoMOPoker, EMbmPlutoMOCard_mask));
	
	CleanupStack::Pop(icons);
	CleanupStack::PopAndDestroy(iconFileName);
	
	ItemDrawer()->FormattedCellData()->SetIconArray(icons); // passing ownership of icons

	}

void CPlutoMOGridEngine::SetupGrid()
	{
		
	AknListBoxLayouts::SetupStandardGrid(*this);


	CFormattedCellListBoxItemDrawer* itemDrawer = this->ItemDrawer();
	TInt cellWidth = ColumnWidth();
	TInt cellHeight = ItemHeight();

		
	AknListBoxLayouts::SetupFormGfxCell(*this,  //the grid
										itemDrawer, // the grid's drawer
										0, // index of the graphic within item strings
										0, // left position
										0, // top position
										0, // right - unused
										0, // bottom - unused
										cellWidth, // width of graphic
										KGraphicsHeight, // height of graphic
										TPoint (0, 0), // start position
										TPoint (cellWidth , KGraphicsHeight)); // end position

	// Set up text subcells
	const CFont* KFont = LatinBold12();
	TInt baseline = cellHeight - KFont->DescentInPixels() - 1;

	AknListBoxLayouts::SetupFormTextCell(*this, // the grid
										 itemDrawer, // the grid's drawer
										 1, // index of text within item strings
										 KFont, // the font for the text
										 KTextColor, // the color of the text - N.B. although commented out in the header file, this is still used!
										 0, // left margin
										 0, // right margin - unused
										 baseline, // Baseline
										 cellWidth, // text width (would need to take margin into account if set, i.e. - lm)
										 CGraphicsContext::ECenter, // Alignment
										 TPoint (0, KGraphicsHeight), // start position
										 TPoint(cellWidth, cellHeight)); // end position
	
	}


void CPlutoMOGridEngine::SizeChanged()
	{
	CAknGrid::SizeChanged();
	SetupGrid();
	}
// End of File	
