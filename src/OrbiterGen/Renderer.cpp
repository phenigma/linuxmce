#ifdef ORBITER
#include <SDL_ttf.h>
#include <SDL_image.h>
#endif

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <list>
using namespace std;

#ifdef OrbiterGen
#include "DesignObj_Generator.h"
#endif

#include "Renderer.h"
#include "../Orbiter/SDL/SDLRendererOCGHelper.h"
#include "../Orbiter/RendererOCG.h"

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"
#include "Orbiter/TextStyle.h"
#include <math.h>

#ifndef ORBITER
#include "pluto_main/Define_HorizAlignment.h"
#include "pluto_main/Define_VertAlignment.h"
#endif //#ifndef ORBITER

#ifdef WINCE
	#define endl '\n'
#endif

#include "DCE/Logger.h"
using namespace DCE;

int myCounter=0;

Renderer::Renderer(string FontPath,string OutputDirectory,int Width,int Height,bool bDisableVideo)
{
#ifndef WIN32
    if( bDisableVideo )
        setenv("SDL_VIDEODRIVER", "dummy", 1); // force SDL to use its dummy video driver (removed a dependency on the X server)
#endif

    m_sFontPath=FontPath;
    m_sOutputDirectory=OutputDirectory;
    m_Width=Width;
    m_Height=Height;

	if (! (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO))
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == -1)
		{
#ifndef WINCE
			cerr << "Failed initializing SDL: " << SDL_GetError() << endl;
#else
			printf("Failed to initialize SDL %s\n", SDL_GetError());
#endif //WINCE
			throw "Failed initializing SDL";
		}
		atexit(SDL_Quit);
	}

	if (TTF_Init() == -1)
	{
#ifndef WINCE
		cout << "Failed to init SDL TTF: " << TTF_GetError() << "\nText won't be rendered" << endl;
#else
		printf("Failed to init SDL TTF: %s\nText won't be rendered\n", TTF_GetError());
#endif
		return;
	}


//      Screen = SDL_SetVideoMode(800, 600, 0, SDL_SWSURFACE);
}

Renderer::~Renderer()
{
    TTF_Quit();
	SDL_Quit();
}

#ifndef ORBITER

// This takes an incoming pRenderImage of what's been rendered so far, and will add this new object and it's
// children to it, or save them separately if set to can hide, or if rendering a selected, highlighted or alt versions
// If iRenderStandard==1, render the standard version only, if ==0, everything but the standard, if -1, do all

// When we start rendering a non-standard version (selected, highlighted, etc.), we will render all of it's children
// in the same state.  Therefore we also have an "OnlyVersion" version flag.  This means only render the given version.
// A standard version that has non-standard variations, will also want all of it's children to render only the standard version
void Renderer::RenderObject(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint Position,int iRenderStandard,bool bPreserveAspectRatio,int iOnlyVersion)
{
    //  cout << "Rendering " << pDesignObj_Generator->m_ObjectID << endl;
	if( pDesignObj_Generator->m_ObjectID.find("2195")!=string::npos )
//  //  ) //|| pDesignObj_Generator->m_ObjectID.find("2689.0.0.2790")!=string::npos )
        //if( pDesignObj_Generator->m_ObjectID== )
    {
        int k=2;
    }

    // We have versions for standard, highlighted, selected, and a variable number of alternate versions
    // We'll make a loop that starts from the number of alt versions, counting down to -2, so when it's at
    // 0, it renders the selected, -1 the highlighted, -2 the standard.  Count down because the standard
    // version should be rendered last, since that's the only one that will be composited on top of the background.
    // The others are all output as separate files.  The first time RenderObject is called, pRenderImage is null
    int StartingValue = iRenderStandard!=1 ? (int) pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() : -2;
    int EndingValue = iRenderStandard!=0 ? -2 : -1;
	if( iOnlyVersion!=-999 )
		StartingValue=EndingValue=iOnlyVersion;

    for(int iIteration=StartingValue;iIteration>=EndingValue;--iIteration)
    {
        // Normally we don't create output files for selected/highlighted/alt versions unless there is a corresponding graphic.  There's really nothing
        // to display otherwise.  However, if the user forced this version with the 'visible state' setting, then we should output that state anyway.  Perhaps
        // it only contains text that should be visible in that state
        bool bForceOutput=false;
        string sInputFile=""; // The graphics file we're using this pass
        string sSaveToFile=""; // If this is set to a value, then we will save the current image to a separate file

        // Create another pointer to the render image, because if we create a new image to be saved
        // in the above file, we want the pointer to return to the original one when this loop completes
		// This shadows the pRenderImage passed in
        RendererImage *pRenderImageOriginal=pRenderImage;
        RendererImage *pRenderImage=pRenderImageOriginal; // TROUBLE nr 1: subsequent loops overwrite pRenderImage allocated in loop a few line below this
        PlutoPoint PositionOriginal=Position; // Ditto with the position
        PlutoPoint Position=PositionOriginal;

        if( iIteration==-2 )
            sInputFile = pDesignObj_Generator->m_sOrigBackgroundFile;
        else if( iIteration==-1 )
            sInputFile = pDesignObj_Generator->m_sOrigSelectedFile;
        else if( iIteration==0 )
            sInputFile = pDesignObj_Generator->m_sOrigHighlightGraphicFilename;
        else
            sInputFile = pDesignObj_Generator->m_vectOrigAltGraphicFilename[iIteration-1];

        // If the visible state is specified, skip this screen if it's not specifically in the list
        if( pDesignObj_Generator->m_sVisibleState.length()>0 && pDesignObj_Generator->m_sVisibleState!="T" && (
                (iIteration==-2 && pDesignObj_Generator->m_sVisibleState.find("N")==string::npos) ||
                (iIteration==-1 && pDesignObj_Generator->m_sVisibleState.find("S")==string::npos) ||
                (iIteration==0 && pDesignObj_Generator->m_sVisibleState.find("H")==string::npos) ||
                (iIteration>0 && pDesignObj_Generator->m_sVisibleState.find(StringUtils::itos(iIteration))==string::npos) ) )
            continue;

        // If we're processing the background (-2) we will still render children
        if( sInputFile.length()==0 && iIteration!=-2 )
        {
            // We will render this anyway if the visible state is forced.  It may just be text, for example
            if( pDesignObj_Generator->m_sVisibleState.length()==0 ||
                    (iIteration==-2 && pDesignObj_Generator->m_sVisibleState.find("N")==string::npos) ||
                    (iIteration==-1 && pDesignObj_Generator->m_sVisibleState.find("S")==string::npos) ||
                    (iIteration==0 && pDesignObj_Generator->m_sVisibleState.find("H")==string::npos) ||
                    (iIteration>0 && pDesignObj_Generator->m_sVisibleState.find(StringUtils::itos(iIteration))==string::npos) )
                continue;
            else
                bForceOutput=true;
        }

        bool bIsMenu=false;
		// If iOnlyVersion is -999, then we've already hit an object with non-standard versions, so we're not 
		// going to write out children as separate objects anymore
        if( iOnlyVersion==-999 && (iIteration!=-2 || pDesignObj_Generator->m_bCanBeHidden || !pRenderImage || 
			pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() || pDesignObj_Generator->m_sOrigHighlightGraphicFilename.size() || pDesignObj_Generator->m_sOrigSelectedFile.size()) )
        {
            // We're going to save this out as a separate file
            sSaveToFile=pDesignObj_Generator->m_ObjectID;
            if( !pRenderImage )
            {
                // This is a new screen, start with a clean canvas
                pRenderImage = CreateBlankCanvas(PlutoSize(m_Width,m_Height)); // TROUBLE nr 1: this pointer is lost each time the for loop starts
                bIsMenu=true;
            }
            else
            {
                // This is going to be a "piece" of the existing background
                pRenderImage=Subset(pRenderImage,pDesignObj_Generator->m_rPosition + Position);
                // All the child's objects will have absolute values, so the offset position if we're outputing to a separate file is our absolute position * -1
                Position.X=pDesignObj_Generator->m_rPosition.X*-1; Position.Y=pDesignObj_Generator->m_rPosition.Y*-1;
            }

            // Append to the filename based on what graphic we're outputing
            if( iIteration==-1 )
                sSaveToFile += "_S";
            else if( iIteration==0 )
                sSaveToFile += "_H";
            else if( iIteration>0 )
                sSaveToFile += "_A" + StringUtils::itos(iIteration);
        }

        RendererImage *pRenderImage_Child=NULL; // The new image to be pasted on top
		RendererMNG *pRendererMNG=NULL;
        if( sInputFile.length()>0 )
        {
			if( StringUtils::ToUpper(FileUtils::FindExtension(sInputFile))=="MNG" )
			{
				pRendererMNG = CreateMNGFromFile(sInputFile,pDesignObj_Generator->m_rBackgroundPosition.Size());
				string::size_type pos = sInputFile.rfind('/');
//				SaveMNGToFile("C:/x/" + sInputFile.substr(pos + 1), pRendererMNG);
			}
			else
			{
				pRenderImage_Child = CreateFromFile(sInputFile,pDesignObj_Generator->m_rBackgroundPosition.Size(),bPreserveAspectRatio,bIsMenu);
//SaveImageToFile(pRenderImage_Child, "first");
			}

            if( !pRenderImage_Child && !pRendererMNG )
            {
                throw "Failed to open " + sInputFile + " abandoning creation of object " + pDesignObj_Generator->m_ObjectID;
            }

			if( pDesignObj_Generator->m_sVisibleState.find('T')!=string::npos )
			{
				int X = pDesignObj_Generator->m_rBackgroundPosition.Width;
				int Y = pDesignObj_Generator->m_rBackgroundPosition.Height;
				StringUtils::Replace(pDesignObj_Generator->m_sVisibleState,"T","");
				int LineWidth = X / 8 + 1; // The width of each line
				unsigned char *pHitTest = new unsigned char[LineWidth * Y];
				pDesignObj_Generator->m_dbHitTest.m_dwSize = LineWidth * Y;
				pDesignObj_Generator->m_dbHitTest.m_pBlock = (char *) pHitTest;
				memset(pHitTest,255,LineWidth * Y);
				
				for(int y=0;y<Y;++y)
				{
					for(int x=0;x<X;++x)
					{
						Uint32 Pixel = getpixel(pRenderImage_Child,x,y);
						unsigned char *pPixel = (unsigned char *) &Pixel;
						if ( pPixel[3]<128 )
						{
							unsigned char c = pHitTest[ y * LineWidth + x/8 ];
							c ^= 1 << (x%8);
							pHitTest[ y * LineWidth + x/8 ] = c;
						}
					}
				}
			}
        }
		else if( pRenderImageOriginal==NULL && iIteration==-2 )
		{
			// This is a top-level object (ie the background for a screen), but there is no image
			SDL_FillRect(pRenderImage->m_pSDL_Surface, NULL,SDL_MapRGBA(pRenderImage->m_pSDL_Surface->format, 0, 0, 0, 255));
			pRenderImage->NewSurface = false;
		}

		int iOnlyVersion_Children=-999;  // By default render all of our children
		if( iOnlyVersion!=-999 )
			iOnlyVersion_Children = iOnlyVersion;
		else if( iIteration!=-2 || pDesignObj_Generator->m_sOrigHighlightGraphicFilename.size() || 
			pDesignObj_Generator->m_sOrigSelectedFile.size() || pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() )
		{
			iOnlyVersion_Children = iIteration;
		}

		for(size_t iFrame=0; iFrame < (pRendererMNG ? pRendererMNG->count() : 1); iFrame++)
		{
			// If this is multi-frame, we have to repeat, so we'll need a copy of the image
			RendererImage *pRenderImageClone;
			RendererImage *pRenderImageClone_Child;
			if( pRendererMNG )
			{
				pRenderImageClone = DuplicateImage(pRenderImage);
				pRenderImageClone_Child = pRendererMNG->GetFrame(iFrame);
			}
			else
			{
				pRenderImageClone = pRenderImage;
				pRenderImageClone_Child = pRenderImage_Child;
			}

			// See if we're supposed to render our children first
			if( pDesignObj_Generator->m_bChildrenBehind )
			{
				RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,bPreserveAspectRatio,iOnlyVersion_Children);
				if(pRenderImageClone_Child )
					CompositeImage(pRenderImageClone,pRenderImageClone_Child,pDesignObj_Generator->m_rPosition.Location() + Position);
				RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
			}
			else // Render our own first
			{
				if(pRenderImageClone_Child )
				{
//SaveImageToFile(pRenderImage, "ri");
//SaveImageToFile(pRenderImageClone, "ic b4");
//SaveImageToFile(pRenderImageClone_Child, "icc b4");
					CompositeImage(pRenderImageClone,pRenderImageClone_Child,pDesignObj_Generator->m_rPosition.Location() + Position);
//SaveImageToFile(pRenderImageClone, "ic aft");
				}
				if( pDesignObj_Generator->m_bChildrenBeforeText )
				{
					RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,bPreserveAspectRatio,iOnlyVersion_Children);
					RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
				}
				else
				{
					RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
					RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,bPreserveAspectRatio,iOnlyVersion_Children);
				}
//SaveImageToFile(pRenderImageClone, "ic aft ch");
			}
//SaveImageToFile(pRenderImageClone, "compositec");
			if( pRendererMNG )
				pRendererMNG->ReplaceFrame(iFrame, pRenderImageClone);
		}
        // If this is a screen (ie top level object) then we should always save something even
        // if there was no input file.
        if( sInputFile.length() || pRenderImageOriginal==NULL || pDesignObj_Generator->m_bCanBeHidden || bForceOutput )
        {
            if( sSaveToFile.length()>0 )
            {
				if( pRendererMNG )
					SaveMNGToFile(m_sOutputDirectory + sSaveToFile,pRendererMNG);
				else
	                SaveImageToFile(pRenderImage,m_sOutputDirectory + sSaveToFile,pDesignObj_Generator->m_bUseOCG);
#ifdef OUTPUT_BMP
                sSaveToFile+=".bmp";
#else
				if( pRendererMNG )
	                sSaveToFile+=".mng";
				else if( pDesignObj_Generator->m_bUseOCG )
					sSaveToFile+=".ocg";
				else
	                sSaveToFile+=".png";
#endif
            }

            if( iIteration==-2 )
                pDesignObj_Generator->m_sBackgroundFile = sSaveToFile;
            else if( iIteration==-1 )
                pDesignObj_Generator->m_sSelectedFile = sSaveToFile;
            else if( iIteration==0 )
                pDesignObj_Generator->m_sHighlightGraphicFilename = sSaveToFile;
            else
			{
				// Pad the vector so we can do the assignment
					if( pDesignObj_Generator->m_vectAltGraphicFilename.size()<iIteration )
					for(size_t s = pDesignObj_Generator->m_vectAltGraphicFilename.size(); s<iIteration; ++s)
						pDesignObj_Generator->m_vectAltGraphicFilename.push_back("");

                pDesignObj_Generator->m_vectAltGraphicFilename[iIteration-1] = sSaveToFile;
			}
        }
        delete pRenderImage_Child;
        delete pRendererMNG;
        if( pRenderImageOriginal!=pRenderImage )
            delete pRenderImage;
    }
    //  cout << "Finished Rendering " << pDesignObj_Generator->m_ObjectID << endl;
}

void Renderer::RenderObjectsChildren(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,bool bPreserveAspectRatio,int iOnlyVersion)
{
    // We don't support layering.  This isn't normally a problem for pre-rendered graphics.  However, for objects that are rendered
    // seperately, like the selected and highlighted versions, if they have transparency, it's possible that other child objects
    // that should be rendered aren't rendered yet.  Rather than making layers and complicating things, we'll just do 2 passes
    // and render all the objects standard variations without CanHide, and then do a second pass for the others.
    DesignObj_DataList::iterator it;
    for(it=pDesignObj_Generator->m_ChildObjects.begin();it!=pDesignObj_Generator->m_ChildObjects.end();++it)
    {
        DesignObj_Generator *pObj = (DesignObj_Generator *) *it;
		if( iOnlyVersion!=-999 )
		{
	        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,0,bPreserveAspectRatio,iOnlyVersion);  // Standard only
		    continue;
		}
if( pObj->m_ObjectID.find("1399")!=string::npos )
//  //  ) //|| pObj->m_ObjectID.find("2689.0.0.2790")!=string::npos )
    //if( pObj->m_ObjectID== )
{
    int k=2;
}
        if( pObj->m_bCanBeHidden || (pObj->m_sVisibleState.length()>0 && pObj->m_sVisibleState.find('N')==string::npos) )
            continue;
        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,1,bPreserveAspectRatio);  // Standard only
        pObj->m_bRendered=true;
    }

	if( iOnlyVersion!=-999 )
		return; // We already rendered all the children

    // Now do the others
    for(it=pDesignObj_Generator->m_ChildObjects.begin();it!=pDesignObj_Generator->m_ChildObjects.end();++it)
    {
        DesignObj_Generator *pObj = (DesignObj_Generator *) *it;
if( pObj->m_ObjectID.find("1399")!=string::npos )
{
    int k=2;
}
        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,pObj->m_bRendered ? 0 : -1,bPreserveAspectRatio);
    }
}

void Renderer::RenderObjectsText(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,int iIteration)
{
    for(size_t s=0;s<pDesignObj_Generator->m_vectDesignObjText.size();++s)
    {
        DesignObjText *pDesignObjText = pDesignObj_Generator->m_vectDesignObjText[s];
        if( !pDesignObjText->m_bPreRender )
            continue;

        // Find the style to use
        TextStyle *pTextStyle = pDesignObjText->m_mapTextStyle_Find(iIteration);
        if( !pTextStyle )
            pTextStyle = pDesignObjText->m_mapTextStyle_Find(0);  // The standard one

        if( !pTextStyle )
            throw "Cannot find a style to use for text " + StringUtils::itos(pDesignObjText->m_PK_Text);

        RenderText(pRenderImage,pDesignObjText,pTextStyle,pDesignObj_Generator,pos);
    }
}

void Renderer::SaveImageToPNGFile(RendererImage * pRendererImage, FILE * File, bool Signature)
{
    // we can't just save NULL pointers...
    if (pRendererImage == NULL || pRendererImage->m_pSDL_Surface == NULL)
    {
        throw "Save to image passed in a null pointer";
    }

	png_bytepp image_rows;
    int BitsPerColor;
    SDL_Surface * Drawing = pRendererImage->m_pSDL_Surface;

    image_rows = new png_bytep[Drawing->h];
    for (int n = 0; n < Drawing->h; n++)
        image_rows[n] = (unsigned char *) Drawing->pixels + n * Drawing->pitch;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_info = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, File);

	if (!Signature)
		png_set_sig_bytes(png_ptr, 8);

    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    BitsPerColor = Drawing->format->BitsPerPixel / Drawing->format->BytesPerPixel;
    png_set_IHDR(png_ptr, png_info, Drawing->w, Drawing->h, BitsPerColor, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//  png_set_bgr(png_ptr); // on x86, 24bit makes colors appear right; not for 32bit (RGBA) or it mixes the colors
    png_write_info(png_ptr, png_info);

    png_write_image(png_ptr, image_rows);
    png_write_end(png_ptr, png_info);

    delete [] image_rows;

    png_destroy_write_struct(&png_ptr, &png_info);
}

void Renderer::SaveImageToFile(RendererImage * pRendererImage, string sSaveToFile, bool bUseOCG)
{
if( sSaveToFile.find(".1257.")!=string::npos )
{
int k=2;
}
    // we can't just save NULL pointers...
    if (pRendererImage == NULL || pRendererImage->m_pSDL_Surface == NULL)
    {
        throw "Save to image passed in a null pointer";
    }
#ifdef OUTPUT_BMP
    string FileName = sSaveToFile + ".bmp";
    if (SDL_SaveBMP(pRendererImage->m_pSDL_Surface, FileName.c_str()) == -1)
    {
        throw "Failed to write file: " + FileName + ": " + SDL_GetError();
    }
#else
	if( bUseOCG )
	{
		string FileName = sSaveToFile + ".ocg";
		cout << "Saving " FileName << endl;
		SDL_SaveOCG(pRendererImage->m_pSDL_Surface, FileName);
	}
	else
	{
		string FileName = sSaveToFile + ".png";
		cout << "Saving " FileName << endl;
		FILE * File = fopen(FileName.c_str(), "wb");
		SaveImageToPNGFile(pRendererImage, File);
		fclose(File);
	}
#endif
}

// load image from file and possibly scale/stretch it
RendererImage * Renderer::CreateFromFile(string sFilename, PlutoSize size,bool bPreserveAspectRatio,bool bCrop)
{
	FILE * File;

	File = fopen(sFilename.c_str(), "rb");
	RendererImage * Result = CreateFromFile(File, size, bPreserveAspectRatio, bCrop);
    if (Result == NULL)
    {
        throw "Can't create surface from file: " + sFilename  + ": " + SDL_GetError();
    }

	fclose(File);
	return Result;
}

RendererImage * Renderer::CreateFromRWops(SDL_RWops * rw, bool bFreeRWops, PlutoSize size, bool bPreserveAspectRatio, bool bCrop)
{
    SDL_Surface * SurfaceFromFile=NULL;
    //  try
    //  {
    SurfaceFromFile = IMG_Load_RW(rw, bFreeRWops); // rw is freed here of bFreeRWops is true (=1)
    /*}
    //  catch(...)
    {
    cerr << "Unhandled exception trying to open file " << sFilename << " with size " << size.Width << "x" << size.Height << endl;
    return NULL;
    }
    */
    if (SurfaceFromFile == NULL)
    {
        throw string("Can't create surface from FILE pointer: ") + SDL_GetError();
    }

    //  cout << "Loaded: " << sFilename << endl;
    /*
    SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format, 0, 0, 0));
    SDL_BlitSurface(SurfaceFromFile, NULL, Screen, NULL);
    SDL_Flip(Screen);
    SDL_Event SDL_event;
    SDL_PollEvent(&SDL_event);
    */
    //Sleep(5000);

    int W = size.Width == 0 ? SurfaceFromFile->w : size.Width;
    int H = size.Height == 0 ? SurfaceFromFile->h : size.Height;
    PlutoSize new_size(W, H);

    RendererImage * RIFromFile = CreateBlankCanvas(new_size);
    if (RIFromFile == NULL)
    {
        delete SurfaceFromFile;
        throw string("Failed to create new image surface");
    }

//	RIFromFile->m_sFilename = sFilename;

    SDL_Surface * ScaledSurface;
    if (W == SurfaceFromFile->w && H == SurfaceFromFile->h)
    {
        // no scaling/stretching needed
        //SDL_BlitSurface(SurfaceFromFile, NULL, RIFromFile->m_pSDL_Surface, NULL);
        //sge_transform(SurfaceFromFile, RIFromFile->m_pSDL_Surface, 0, 1, 1, 0, 0, 0, 0, SGE_TSAFE);
        ScaledSurface = sge_transform_surface(SurfaceFromFile, SDL_MapRGBA(SurfaceFromFile->format, 0, 0, 0, 0), 0, 1, 1, SGE_TSAFE);
    }
    else
    {
        // image needs to be steched/scaled
        // I could use SDL_SoftStretch(), but the SDL developers strongly advise against it for the moment
        // I use the SGE extension library instead
        float scaleX = (float) RIFromFile->m_pSDL_Surface->w / SurfaceFromFile->w;
        float scaleY = (float) RIFromFile->m_pSDL_Surface->h / SurfaceFromFile->h;

        if( bPreserveAspectRatio && bCrop )
        {
            if( scaleY>scaleX )
                scaleX=scaleY;
            else
                scaleY=scaleX;
        }
        else if( bPreserveAspectRatio )
        {
            if( scaleY>scaleX )
                scaleY=scaleX;
            else
                scaleX=scaleY;
        }
/*  starting with the mobile phone, we have 'distorted' images because we want to re-use buttons, but the aspect ratios are different
*/

        //sge_transform(SurfaceFromFile, RIFromFile->m_pSDL_Surface, 0, scaleX, scaleY, 0, 0, 0, 0, SGE_TSAFE);
        ScaledSurface = sge_transform_surface(SurfaceFromFile, SDL_MapRGBA(SurfaceFromFile->format, 0, 0, 0, 0), 0, scaleX, scaleY, SGE_TSAFE);
    }
    SDL_SetAlpha(ScaledSurface, 0, 0);
    SDL_BlitSurface(ScaledSurface, NULL, RIFromFile->m_pSDL_Surface, NULL);

    SDL_FreeSurface(ScaledSurface);
    SDL_FreeSurface(SurfaceFromFile);

    return RIFromFile;
}

RendererImage * Renderer::CreateFromFile(FILE * File, PlutoSize size, bool bPreserveAspectRatio, bool bCrop)
{
	SDL_RWops * rw = SDL_RWFromFP(File, 0);
	return CreateFromRWops(rw, true, size, bPreserveAspectRatio, bCrop);
}

void Renderer::CompositeImage(RendererImage * pRenderImage_Parent, RendererImage * pRenderImage_Child, PlutoPoint pos)
{
    // with no destination, function always fails
    if (pRenderImage_Parent == NULL || pRenderImage_Parent->m_pSDL_Surface == NULL)
        throw "Composite image passed null parent";
    // with no source, function always succeeds (I assume that it's an empty image)
    if (pRenderImage_Child == NULL || pRenderImage_Child->m_pSDL_Surface == NULL)
        throw "Composite image passed null child";

    //  cout << "Composing image: " << pRenderImage_Child->m_sFilename << endl;
    /*
    SDL_FillRect(Screen, NULL, 0);
    SDL_BlitSurface(pRenderImage_Child->m_pSDL_Surface, NULL, Screen, NULL);
    SDL_Flip(Screen);
    //Sleep(5000);
    cout << "Composing to image: " << pRenderImage_Parent->m_sFilename << endl;
    SDL_FillRect(Screen, NULL, 0);
    SDL_BlitSurface(pRenderImage_Parent->m_pSDL_Surface, NULL, Screen, NULL);
    SDL_Flip(Screen);
    */
    //Sleep(5000);

    SDL_Rect dest_rect;
    dest_rect.x = pos.X;
    dest_rect.y = pos.Y;

    bool WasSrcAlpha = (pRenderImage_Child->m_pSDL_Surface->flags & SDL_SRCALPHA) != 0;
    Uint8 WasAlpha = pRenderImage_Child->m_pSDL_Surface->format->alpha;
    if (pRenderImage_Parent->NewSurface)
    {
        SDL_SetAlpha(pRenderImage_Child->m_pSDL_Surface, 0, 0);
        pRenderImage_Parent->NewSurface = false;
    }

    if (SDL_BlitSurface(pRenderImage_Child->m_pSDL_Surface, NULL, pRenderImage_Parent->m_pSDL_Surface, &dest_rect) < 0)
        //if (my_BlitAll(pRenderImage_Child, pRenderImage_Parent, pos) == -1)
    {
        throw string("Failed composing image: ") + SDL_GetError();
    }

    SDL_SetAlpha(pRenderImage_Child->m_pSDL_Surface, WasSrcAlpha, WasAlpha);

//      cout << "Composed images: " << pRenderImage_Parent->m_sFilename << " <- " << pRenderImage_Child->m_sFilename
//          << "pos: " << pos.X << "," << pos.Y << endl;
    /*
    SDL_FillRect(Screen, NULL, 0);
    SDL_BlitSurface(pRenderImage_Parent->m_pSDL_Surface, NULL, Screen, NULL);
    SDL_Flip(Screen);
    */
    //Sleep(5000);
}

RendererImage * Renderer::DuplicateImage(RendererImage * pRendererImage)
{
	PlutoSize Size;
	PlutoPoint Pos(0, 0);

	Size.Width = pRendererImage->m_pSDL_Surface->w;
	Size.Height = pRendererImage->m_pSDL_Surface->h;
	RendererImage * Result = CreateBlankCanvas(Size);
	CompositeImage(Result, pRendererImage, Pos);

	return Result;
}

RendererMNG * Renderer::CreateMNGFromFiles(const vector<string> & FileNames, PlutoSize Size)
{
	int RealW, RealH;
	MNGHeader Header;
	int kounter = 0;

	RendererMNG * Result = new RendererMNG;

	for (int i = 0; i < FileNames.size(); i++)
	{
		RendererImage * pRendererImage = CreateFromFile(FileNames[i], Size);
		Result->AppendFrame(pRendererImage);
//		SaveImageToFile(pRendererImage, "C:/x/frame" + StringUtils::ltos(kounter++) + ".png");
	}

	Header.ticks_per_second = 1000;
	Header.frame_height = Size.Height;
	Header.frame_width = Size.Width;
	Result->SetHeader(Header);
	return Result;
}

RendererMNG * Renderer::CreateMNGFromFile(string FileName, PlutoSize Size)
{
//	vector<string> V;
//	for (int i = 0; i <= 14; i++)
//	{
//		V.insert(V.end(), string("c:/x/untitled") + (i < 10 ? "0" : "") + StringUtils::itos(i) + ".png");
//	}

//	return CreateMNGFromFiles(V, Size);

	FILE * File;
	int RealW, RealH;
	MNGHeader Header;
	int kounter = 0;

	File = fopen(FileName.c_str(), "rb");
	if (! File)
		return NULL;

	char buffer[1024];
	fread(buffer, 1, 8, File); // strip signature

	PNGChunk * chunk;
	bool BuildingPNG = false;
	PNGCatChunks * InMemoryPNG = new PNGCatChunks;

	RendererMNG * Result = new RendererMNG;
	while (ReadChunk(File, chunk = new PNGChunk))
	{
		if (chunk->type == "MHDR")
		{
			ParseMNGHeader(chunk->data, &Header);
			Result->SetHeader(Header);
			RealW = Size.Width == 0 ? Header.frame_width : Size.Width;
			RealH = Size.Height == 0 ? Header.frame_height : Size.Height;
		}

		if (chunk->type == "IHDR")
		{
			BuildingPNG = true;
		}

		if (BuildingPNG)
		{
			InMemoryPNG->AddChunk(chunk);
			if (chunk->type == "IEND")
			{
				BuildingPNG = false;
				char * pGraphicFile;
				size_t iSizeGraphicFile = InMemoryPNG->CatChunks(pGraphicFile);
				
				SDL_RWops * rw = SDL_RWFromMem(pGraphicFile, iSizeGraphicFile);
				RendererImage * pRendererImage = CreateFromRWops(rw, true, Size);

				delete [] pGraphicFile;

//				SaveImageToFile(pRendererImage, "C:/x/frameb" + StringUtils::ltos(kounter++) + ".png");
				Result->AppendFrame(pRendererImage);
				InMemoryPNG->clear();
			}
		}
		else
		{
			delete chunk;
		}
	}
	delete InMemoryPNG;

	fclose(File);

	Header = Result->GetHeader();
	Header.frame_height = RealH;
	Header.frame_width = RealW;
	Result->SetHeader(Header);
	return Result;
}

void Renderer::SaveMNGToFile(string FileName, RendererMNG * MNG)
{
	FILE * File;

	File = fopen((FileName + ".mng").c_str(), "wb");

	fwrite(MNGsignature, 1, 8, File);
	char * Header;
	MNG->GetHeader().BinaryForm(Header);
	fwrite(Header, 1, 40, File);
	delete [] Header;

	for (size_t i = 0; i < MNG->count() ; i++)
	{
		SaveImageToPNGFile(MNG->GetFrame(i), File, false);
	}

	const char * MEND = "\x00\x00\x00\x00MEND\x21\x20\xF7\xD5";
	fwrite(MEND, 1, 12, File);

	fclose(File);
}

/* WORKAROUND */
void WrapAndRenderText(void *Surface, string text, int X, int Y, int W, int H,
                       string FontPath, TextStyle *pTextStyle);

void Renderer::RenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator *pDesignObj_Generator, PlutoPoint pos)
{
    PlutoRectangle rect = pDesignObjText->m_rPosition + pos;

    WrapAndRenderText(pRenderImage->m_pSDL_Surface, pDesignObjText->m_sText, rect.X, rect.Y, rect.Width, rect.Height,
        m_sFontPath, pTextStyle);
}

Uint32 Renderer::getpixel(RendererImage * RIsurface, int x, int y)
{
    SDL_Surface * surface = RIsurface->m_pSDL_Surface;

    // all pixels outside the surface are black
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
        return SDL_MapRGB(surface->format, 0, 0, 0);

    int bpp = surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void Renderer::putpixel(RendererImage * RIsurface, int x, int y, Uint32 pixel_color)
{
    SDL_Surface * surface = RIsurface->m_pSDL_Surface;

    // don't try to put a pixel outside the surface
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
        return;

    int bpp = surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color;
        break;
    }
}

// extract a subsurface from a surface
RendererImage * Renderer::Subset(RendererImage *pRenderImage, PlutoRectangle rect)
{
    //  return pRenderImage; // this is just a hack to see if it's a bug in the downstream code

    PlutoSize size(rect.Width, rect.Height);
    RendererImage * SubSurface = CreateBlankCanvas(size);

    //  cout << "SubSurface: " << pRenderImage->m_sFilename << ": "
    //      << rect.X << "," << rect.Y << "," << rect.Width << "," << rect.Height <<  endl;

    if (SubSurface == NULL)
    {
        cerr << "Failed to create sub-surface. Can't extract subset" << endl;
    }

    // copy pixel by pixel (this can be optimized to get line by line?)
    for (int j = 0; j < rect.Height; j++)
    {
        for (int i = 0; i < rect.Width; i++)
        {
            // we may need locking on the two surfaces
            putpixel(SubSurface, i, j, getpixel(pRenderImage, i + rect.X, j + rect.Y));
        }
    }
    SubSurface->NewSurface = false;
    /*
    SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format, 0, 0, 0));
    SDL_BlitSurface(SubSurface->m_pSDL_Surface, NULL, Screen, NULL);
    SDL_Flip(Screen);
    SDL_Event SDL_event;
    SDL_PollEvent(&SDL_event);
    */
    //Sleep(5000);

    return SubSurface;
}

void DoRender(string font, string output,int width,int height,bool bAspectRatio,class DesignObj_Generator *ocDesignObj)
{
    Renderer r(font,output,width,height);
    r.RenderObject(NULL,ocDesignObj,PlutoPoint(0,0),-1,bAspectRatio);  // Render everything
}

#endif //ifndef ORBITER

RendererImage * Renderer::CreateBlankCanvas(PlutoSize size)
{
    RendererImage * Canvas = new RendererImage;
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
    rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif

    int Width = size.Width;
    int Height = size.Height;

    Canvas->m_sFilename = "(new surface)";
    Canvas->m_pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 32, rmask, gmask, bmask, amask);

    if (Canvas->m_pSDL_Surface == NULL)
    {
        throw string("Failed to create blank canvas: ") + SDL_GetError();
        delete Canvas;
        Canvas = NULL;
    }

//	SDL_FillRect(Canvas->m_pSDL_Surface, NULL,SDL_MapRGBA(Canvas->m_pSDL_Surface->format, 0, 0, 0, 255));

    Canvas->NewSurface = true;

    return Canvas;
}

// pass NULL in pRenderImage if you're interested only in the rendered text canvas size
PlutoSize Renderer::RealRenderText(RendererImage * pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, PlutoPoint pos)
{
#if ( defined( PROFILING_TEMP ) )
	clock_t clkStart = clock(  );
#endif

    //  cout << "Starting to render text" << pDesignObjText->m_PK_Text << " " << pDesignObjText->m_Text << endl;
    // hack
    if( !pTextStyle->m_pTTF_Font )
    {
// TODO -- temporary debugging info
        if (pTextStyle->m_sFont == "")
            pTextStyle->m_sFont = "arial";

        try
        {
            pTextStyle->m_pTTF_Font = TTF_OpenFont((m_sFontPath + pTextStyle->m_sFont + ".ttf").c_str(), pTextStyle->m_iPixelHeight);
        }
        catch(...)
        {
        }

        // Sometimes the camel case is converted to all upper or lower in Linux.
        if( pTextStyle->m_pTTF_Font==NULL )
        {
            try
            {
                pTextStyle->m_pTTF_Font = TTF_OpenFont((m_sFontPath + StringUtils::ToUpper(pTextStyle->m_sFont) + ".TTF").c_str(), pTextStyle->m_iPixelHeight);
            }
            catch(...)
            {
            }
        }

        if( pTextStyle->m_pTTF_Font==NULL )
        {
            try
            {
                pTextStyle->m_pTTF_Font = TTF_OpenFont((m_sFontPath + StringUtils::ToLower(pTextStyle->m_sFont) + ".ttf").c_str(), pTextStyle->m_iPixelHeight);
            }
            catch(...)
            {
            }
        }

        /*
        #ifdef WIN32
        TTF_Font * pTextStyle->m_pTTF_Font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", pDesignObjText->m_PixelHeight);
        #else
        TTF_Font * pTextStyle->m_pTTF_Font = TTF_OpenFont("/usr/share/pluto/fonts/arial.ttf", pDesignObjText->m_PixelHeight);
        #endif
        */
        if (pTextStyle->m_pTTF_Font == NULL)
        {
            TTF_Quit();
            throw "Can't open font: " + pTextStyle->m_sFont + ": " + TTF_GetError();
        }
    }
    //  cout << "opened font" << endl;

    SDL_Color SDL_color;
    /* HACKED IN - Windows and Linux get different colors here */
    SDL_color.r=pTextStyle->m_ForeColor.R();
    SDL_color.g=pTextStyle->m_ForeColor.G();
    SDL_color.b=pTextStyle->m_ForeColor.B();
    SDL_color.unused = pTextStyle->m_ForeColor.A();

    /* Underline combined with anything crashes */
    int style = TTF_STYLE_NORMAL;
    if (pTextStyle->m_bUnderline)
    {
        style = TTF_STYLE_UNDERLINE;
    }
    else
    {
        if (pTextStyle->m_bBold)
            style |= TTF_STYLE_BOLD;
        if (pTextStyle->m_bItalic)
            style |= TTF_STYLE_ITALIC;
    }
    TTF_SetFontStyle((TTF_Font *) pTextStyle->m_pTTF_Font, style);

#if ( defined( PROFILING_TEMP ) )
	clock_t clkBlending = clock(  );
#endif

	SDL_Surface * RenderedText = NULL;
	try
	{
	    RenderedText = TTF_RenderText_Blended((TTF_Font *)pTextStyle->m_pTTF_Font, pDesignObjText->m_sText.c_str(), SDL_color);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		g_pPlutoLogger->Write(LV_WARNING, "Renderer::RealRenderText : TTF_RenderText_Blended crashed!");
	}
	
    if (RenderedText == NULL)
    {
        TTF_Quit();
        //throw /*cerr <<*/ "Can't render text: " + pDesignObjText->m_sText + ": " + TTF_GetError();
 		g_pPlutoLogger->Write(LV_WARNING, "Renderer::RealRenderText: Can't render text: %s", pDesignObjText->m_sText.c_str());
			

		return PlutoSize(1, 1);
    }

    PlutoSize RenderedSize(RenderedText->w, RenderedText->h);

#if ( defined( PROFILING_TEMP ) )
	clock_t clkBliting = clock(  );
#endif

    if (pRenderImage != NULL)
    {
        SDL_Rect SDL_rect;
        SDL_rect.x = pos.X;
        SDL_rect.y = pos.Y;
        SDL_SetAlpha(RenderedText, 0, 0);
        pRenderImage->NewSurface = false;
        SDL_BlitSurface(RenderedText, NULL, pRenderImage->m_pSDL_Surface, &SDL_rect);
//      SDL_SaveBMP(pRenderImage->m_pSDL_Surface, ("blit-" + pDesignObjText->m_sText + ".bmp").c_str());
    //          cout << "called Blit" << endl;
    }

// todo - find a solution for this  TTF_CloseFont(Font);
	try
	{
	    SDL_FreeSurface(RenderedText);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		g_pPlutoLogger->Write(LV_WARNING, "Renderer::RealRenderText : SDL_FreeSurface crashed!");
	}

#if ( defined( PROFILING_TEMP ) )
	clock_t clkFinished = clock(  );

	g_pPlutoLogger->Write( 
		LV_CONTROLLER, 
		"PlutoSize Renderer::RealRenderText: %s took %d ms: \n"
			"\t- Start: %d ms\n"
			"\t- Blended: %d ms\n"
			"\t- Blited: %d ms",
		pDesignObjText->m_sText.c_str(), 
		clkFinished - clkStart,
		clkBlending - clkStart,
		clkBliting - clkBlending,
		clkFinished - clkBliting
	);

#endif


    return RenderedSize;
}

/*static*/ InMemoryMNG *Renderer::CreateInMemoryMNGFromFile(string sFileName, PlutoSize Size)
{
	FILE * File;
	int RealW, RealH;
	MNGHeader *pHeader = new MNGHeader;
	int kounter = 0;

	File = fopen(sFileName.c_str(), "rb");
	if (! File)
		return NULL;

	char buffer[1024];
	fread(buffer, 1, 8, File); // strip signature

	PNGChunk * chunk;
	bool BuildingPNG = false;

	InMemoryMNG *pInMemoryMNG = new InMemoryMNG;
	PNGCatChunks * InMemoryPNG;

	while (ReadChunk(File, chunk = new PNGChunk))
	{
		if (chunk->type == "MHDR")
		{
			ParseMNGHeader(chunk->data, pHeader);
			RealW = Size.Width == 0 ? pHeader->frame_width : Size.Width;
			RealH = Size.Height == 0 ? pHeader->frame_height : Size.Height;
		}

		if (chunk->type == "IHDR")
		{
			InMemoryPNG = new PNGCatChunks;
			BuildingPNG = true;
		}

		if (BuildingPNG)
		{
			InMemoryPNG->AddChunk(chunk);
			if (chunk->type == "IEND")
			{
				BuildingPNG = false;
				pInMemoryMNG->m_vectMNGframes.push_back(InMemoryPNG);
			}
		}
		else
		{
			delete chunk;
		}
	}

	delete chunk; //(for Radu) If ReadCheck returns false, who will delete the chunk? BUBU!

	fclose(File);

	pHeader->frame_height = RealH;
	pHeader->frame_width = RealW;
	pInMemoryMNG->m_pMNGHeader = pHeader;

	return pInMemoryMNG;
}


#ifdef OrbiterGen
#define FLAG_DISABLE_VIDEO true
#else
#define FLAG_DISABLE_VIDEO false
#endif

/** @todo: Ask radu to fix this .. global font renderer issue */
// Nasty hack -- Ask Radu why the fuck he decided to reinitialize the entire font engine for every word todo
#ifdef WIN32
	#ifndef WINCE
		Renderer r("C:/windows/fonts/", "", 800, 600, FLAG_DISABLE_VIDEO);
	#else
		Renderer r("\\", "", 800, 600, FLAG_DISABLE_VIDEO);
	#endif
#else
Renderer r("/usr/share/fonts/truetype/msttcorefonts/", "", 800, 600, FLAG_DISABLE_VIDEO);
#endif



// the last (void *) parameter is actually a (RendererImage *)
// but it's made (void *) to accomodate the WORKAROUND (just do a search for WORKAROUND)
pair<int, int> GetWordWidth(string Word, string FontPath, TextStyle *pTextStyle, void * & RI, bool NewSurface = true)
{
// Radu was creating the render for every call
    RendererImage * Canvas;

    DesignObjText DOText;
    DOText.m_sText = Word;

    PlutoSize Size = r.RealRenderText(NULL, &DOText, pTextStyle, PlutoPoint(0,0));
    if (! NewSurface)
    {
        Canvas = (RendererImage *) RI;
        delete Canvas;
    }

    Canvas = r.CreateBlankCanvas(Size);
    r.RealRenderText(Canvas, &DOText, pTextStyle, PlutoPoint(0,0));

    RI = Canvas;
    return pair<int, int>(Size.Width, Size.Height);

}

/* WORKAROUND: void * = SDL_Surface * */
int DoRenderToSurface(void * Surface, list<void *> &RI, int posX, int posY)
{
    list<void *>::iterator i;

    RendererImage * myRI;
    for (i = RI.begin(); i != RI.end(); i++)
    {
        SDL_Rect dest;
        myRI = (RendererImage *) * i;

        dest.x = posX; dest.y = posY;

        SDL_BlitSurface(myRI->m_pSDL_Surface, NULL, (SDL_Surface *) Surface, &dest);
        posX += myRI->m_pSDL_Surface->w;
    }

    return myRI->m_pSDL_Surface->h;
}

// (void *) / (RendererImage *) WORKAROUND
// returns height of rendered row
int DoRenderToScreen(list<void *> &RI, int posX, int posY)
{
    static SDL_Surface * Screen = NULL;
    if (! (SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == SDL_INIT_VIDEO))
    {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
        atexit(SDL_Quit);
    }

    if (Screen == NULL)
        Screen = SDL_SetVideoMode(800, 600, 0, SDL_SWSURFACE);

    int Result = DoRenderToSurface(Screen, RI, posX, posY);
    SDL_Flip(Screen);

    return Result;
}

/* WORKAROUND (void *) -> (RendererImage *) */
void extDeleteRendererImage(void * & RI)
{
    delete (RendererImage *) RI;
}

void DoSDL()
{
    while(1)
    {
        SDL_Event E;
        SDL_WaitEvent(&E);
        if (E.type == SDL_QUIT)
            break;
    };
}
