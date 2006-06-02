//
// Author : Remus C.
//
// Changed by : ...
//

// SDL demo code copied here from wx-sdl tutorial

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_sdl.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxdialog_sdl.h"

////@begin XPM images
////@end XPM images

#ifdef WX_SDL_DEMO
#include "SDL/SDL.h"
#include <wx/dcbuffer.h>

class Demo_PanelSDL : public wxPanel
{
    DECLARE_CLASS(Demo_PanelSDL);
    DECLARE_EVENT_TABLE();

public:
    Demo_PanelSDL(wxWindow *parent);
    ~Demo_PanelSDL();

protected:
    SDL_Surface *screen;
    void onPaint(wxPaintEvent &event);
    void onEraseBackground(wxEraseEvent &event);
    void onIdle(wxIdleEvent &event);
    void createScreen(); // Creates the SDL_Surface

public:
    int v_nId;
};

inline void Demo_PanelSDL::onEraseBackground(wxEraseEvent &)
{
}

IMPLEMENT_CLASS(Demo_PanelSDL, wxPanel)

    BEGIN_EVENT_TABLE(Demo_PanelSDL, wxPanel)
    EVT_PAINT(Demo_PanelSDL::onPaint)
    EVT_ERASE_BACKGROUND(Demo_PanelSDL::onEraseBackground)
    EVT_IDLE(Demo_PanelSDL::onIdle)
    END_EVENT_TABLE()

    Demo_PanelSDL::Demo_PanelSDL(wxWindow *parent)
            : wxPanel(parent, SYMBOL_WXDIALOG_SDL_IDNAME)
            , screen(NULL)
            , v_nId(0)
{
    // ensure the size of the wxPanel
    wxSize size(640, 480);
    SetMinSize(size);
    SetMaxSize(size);
}

Demo_PanelSDL::~Demo_PanelSDL()
{
    if (screen != NULL)
    {
        SDL_FreeSurface(screen);
    }
}

void Demo_PanelSDL::onPaint(wxPaintEvent &)
{
    // can't draw if the screen doesn't exist yet
    if (screen == NULL)
    {
        return;
    }
    // lock the surface if necessary
    if (SDL_MUSTLOCK(screen))
    {
        if (SDL_LockSurface(screen) < 0)
        {
            return;
        }
    }
    // create a bitmap from our pixel data
    wxBitmap bmp(wxImage(screen->w, screen->h, static_cast<unsigned char *>(screen->pixels), true));
    // unlock the screen
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
    // paint the screen
    wxBufferedPaintDC dc(this, bmp);
}

void Demo_PanelSDL::onIdle(wxIdleEvent &)
{
    // create the SDL_Surface
    createScreen();
    // Lock surface if needed
    if (SDL_MUSTLOCK(screen))
    {
        if (SDL_LockSurface(screen) < 0)
        {
            return;
        }
    }
    // Ask SDL for the time in milliseconds
    int tick = SDL_GetTicks();
    for (int y = 0; y < 480; y++)
    {
        for (int x = 0; x < 640; x++)
        {
            //ORIGINAL: wxUint32 color = (y * y) + (x * x) + tick;
            wxUint32 color = (y * y) + (x * x) + tick;
            if (v_nId == 2)
                color = (x * x) + tick;
            else if (v_nId == 3)
                color = (y * y) + tick;
            wxUint8 *pixels = static_cast<wxUint8 *>(screen->pixels) +
                (y * screen->pitch) +
                (x * screen->format->BytesPerPixel);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            pixels[0] = color & 0xFF;
            pixels[1] = (color >> 8) & 0xFF;
            pixels[2] = (color >> 16) & 0xFF;
#else
            pixels[0] = (color >> 16) & 0xFF;
            pixels[1] = (color >> 8) & 0xFF;
            pixels[2] = color & 0xFF;
#endif
        }
    }
    // Unlock if needed
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
    // refresh the panel
    Refresh(false);
    // throttle to keep from flooding the event queue
    wxMilliSleep(33);
}

void Demo_PanelSDL::createScreen()
{
    if (screen == NULL)
    {
        int width, height;
        GetSize(&width, &height);
        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0, 0, 0, 0);
    }
}

#endif // WX_SDL_DEMO

/*!
 * wxDialog_SDL type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_SDL, wxDialog_Base );

/*!
 * wxDialog_SDL event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_SDL, wxDialog_Base )

////@begin wxDialog_SDL event table entries
    EVT_MOTION( wxDialog_SDL::OnMotion )

////@end wxDialog_SDL event table entries

    END_EVENT_TABLE();

int wxDialog_SDL::v_nRefCount = 0;

/*!
 * wxDialog_SDL constructors
 */

wxDialog_SDL::wxDialog_SDL( )
        : v_pDemo_PanelSDL(NULL)
{
    ++v_nRefCount;
}

wxDialog_SDL::wxDialog_SDL( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_pDemo_PanelSDL(NULL)
{
    ++v_nRefCount;
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_SDL creator
 */

bool wxDialog_SDL::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_SDL member initialisation
////@end wxDialog_SDL member initialisation

////@begin wxDialog_SDL creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end wxDialog_SDL creation
    return true;
}

/*!
 * Control creation for wxDialog_SDL
 */

void wxDialog_SDL::CreateControls()
{
    _LOG_NFO("v_nRefCount=%d", v_nRefCount);
////@begin wxDialog_SDL content construction
    wxDialog_SDL* itemDialog_Base1 = this;

////@end wxDialog_SDL content construction
    wxUnusedVar(itemDialog_Base1);
#ifdef WX_SDL_DEMO
    v_pDemo_PanelSDL = new Demo_PanelSDL(this);
    v_pDemo_PanelSDL->v_nId = v_nRefCount;
#endif // WX_SDL_DEMO
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_SDL::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_SDL::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_SDL bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_SDL bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_SDL::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_SDL icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_SDL icon retrieval
}

wxDialog_SDL::~wxDialog_SDL()
{
    --v_nRefCount;
}

/*!
 * wxEVT_MOTION event handler for ID_DIALOG_SDL
 */

void wxDialog_SDL::OnMotion( wxMouseEvent& event )
{
////@begin wxEVT_MOTION event handler for ID_DIALOG_SDL in wxDialog_SDL.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_MOTION event handler for ID_DIALOG_SDL in wxDialog_SDL.
}
