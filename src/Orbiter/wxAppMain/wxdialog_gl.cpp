//
// Author : Remus C.
//
// Changed by : ...
//

// OpenGL demo code copied here from wxGLCanvas cube demo

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxdialog_gl.h"
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

#include "wxdialog_gl.h"

////@begin XPM images
////@end XPM images

#ifdef WX_GL_DEMO

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#if wxUSE_GLCANVAS

#include <wx/glcanvas.h>

#ifndef __WXMSW__     // for StopWatch, see remark below
#if defined(__WXMAC__) && !defined(__DARWIN__)
#include <utime.h>
#include <unistd.h>
#else
#include <sys/time.h>
#include <sys/unistd.h>
#endif
#else
#include <sys/timeb.h>
#endif

class Demo_GLCanvas: public wxGLCanvas
{
    friend class MyFrame;
public:
    Demo_GLCanvas( wxWindow *parent, wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0, const wxString& name = _T("Demo_GLCanvas") );
    Demo_GLCanvas( wxWindow *parent, const Demo_GLCanvas *other,
                  wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0,
                  const wxString& name = _T("Demo_GLCanvas") );
    ~Demo_GLCanvas();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void Render();
    void InitGL();
    void Rotate(GLfloat deg);
    static GLfloat CalcRotateSpeed(unsigned long acceltime);
    static GLfloat CalcRotateAngle( unsigned long lasttime,
                                    unsigned long acceltime );
    void Action( long code, unsigned long lasttime,
                 unsigned long acceltime );

private:
    bool   m_init;
    GLuint m_gllist;
    long   m_rleft;
    long   m_rright;
    static unsigned long  m_secbase;
    static int            m_TimeInitialized;
    static unsigned long  m_xsynct;
    static unsigned long  m_gsynct;
    long           m_Key;
    unsigned long  m_StartTime;
    unsigned long  m_LastTime;
    unsigned long  m_LastRedraw;
    DECLARE_EVENT_TABLE();

public:
    int v_nId;
};

#endif // #if wxUSE_GLCANVAS

/*----------------------------------------------------------------------
  Utility function to get the elapsed time (in msec) since a given point

  works right with glibc-2.1 and linux, at least for me)
  -----------------------------------------------------------------------*/
unsigned long StopWatch( unsigned long *sec_base )
{
    unsigned long secs,msec;
#if defined(__WXMSW__)
    struct timeb tb;
    ftime( &tb );
    secs = tb.time;
    msec = tb.millitm;
#elif defined(__WXMAC__) && !defined(__DARWIN__)
    wxLongLong tl = wxGetLocalTimeMillis();
    secs = (unsigned long) (tl.GetValue() / 1000);
    msec = (unsigned long) (tl.GetValue() - secs*1000);
#else
    // think every unice has gettimeofday
    struct timeval tv;
    gettimeofday( &tv, (struct timezone *)NULL );
    secs = tv.tv_sec;
    msec = tv.tv_usec/1000;
#endif
    if( *sec_base == 0 )
        *sec_base = secs;
    return( (secs-*sec_base)*1000 + msec );
}

/*----------------------------------------------------------------
  Implementation of Test-GLCanvas
  -----------------------------------------------------------------*/
BEGIN_EVENT_TABLE(Demo_GLCanvas, wxGLCanvas)
    EVT_SIZE(Demo_GLCanvas::OnSize)
    EVT_PAINT(Demo_GLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(Demo_GLCanvas::OnEraseBackground)
    EVT_KEY_DOWN( Demo_GLCanvas::OnKeyDown )
    EVT_KEY_UP( Demo_GLCanvas::OnKeyUp )
    EVT_ENTER_WINDOW( Demo_GLCanvas::OnEnterWindow )
    END_EVENT_TABLE()

    unsigned long  Demo_GLCanvas::m_secbase = 0;
int            Demo_GLCanvas::m_TimeInitialized = 0;
unsigned long  Demo_GLCanvas::m_xsynct;
unsigned long  Demo_GLCanvas::m_gsynct;

Demo_GLCanvas::Demo_GLCanvas(wxWindow *parent, wxWindowID id,
                           const wxPoint& pos, const wxSize& size, long style, const wxString& name)
        : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
        , v_nId(0)
{
    m_init = false;
    m_gllist = 0;
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
}

Demo_GLCanvas::Demo_GLCanvas(wxWindow *parent, const Demo_GLCanvas *other,
                           wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                           const wxString& name )
        : wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
        , v_nId(0)
{
    m_init = false;
    m_gllist = other->m_gllist; // share display list
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
}

Demo_GLCanvas::~Demo_GLCanvas()
{
}

void Demo_GLCanvas::Render()
{
    wxPaintDC dc(this);
#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif
    SetCurrent();
    // Init OpenGL once, but after SetCurrent
    if (!m_init)
    {
        InitGL();
        m_init = true;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //ORIGINAL: glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f -1+v_nId, 1.0f, 3.0f);
    glMatrixMode(GL_MODELVIEW);
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( m_gllist == 0 )
    {
        m_gllist = glGenLists( 1 );
        glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
        /* draw six faces of a cube */
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, 1.0f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glEnd();
        glEndList();
    }
    else
    {
        glCallList(m_gllist);
    }
    glFlush();
    SwapBuffers();
}

void Demo_GLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
    SetFocus();
}

void Demo_GLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}

void Demo_GLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void Demo_GLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing.
}

void Demo_GLCanvas::InitGL()
{
    SetCurrent();
    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
    /* position viewer */
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0f, 0.0f, -2.0f);
    /* position object */
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

GLfloat Demo_GLCanvas::CalcRotateSpeed( unsigned long acceltime )
{
    GLfloat t,v;
    t = ((GLfloat)acceltime) / 1000.0f;
    if( t < 0.5f )
        v = t;
    else if( t < 1.0f )
        v = t * (2.0f - t);
    else
        v = 0.75f;
    return(v);
}

GLfloat Demo_GLCanvas::CalcRotateAngle( unsigned long lasttime,
                                       unsigned long acceltime )
{
    GLfloat t,s1,s2;
    t = ((GLfloat)(acceltime - lasttime)) / 1000.0f;
    s1 = CalcRotateSpeed( lasttime );
    s2 = CalcRotateSpeed( acceltime );
    return( t * (s1 + s2) * 135.0f );
}

void Demo_GLCanvas::Action( long code, unsigned long lasttime,
                           unsigned long acceltime )
{
    GLfloat angle = CalcRotateAngle( lasttime, acceltime );
    if (code == m_rleft)
        Rotate( angle );
    else if (code == m_rright)
        Rotate( -angle );
}

void Demo_GLCanvas::OnKeyDown( wxKeyEvent& event )
{
    long evkey = event.GetKeyCode();
    if (evkey == 0) return;
    if (!m_TimeInitialized)
    {
        m_TimeInitialized = 1;
        m_xsynct = event.GetTimestamp();
        m_gsynct = StopWatch(&m_secbase);
        m_Key = evkey;
        m_StartTime = 0;
        m_LastTime = 0;
        m_LastRedraw = 0;
    }
    unsigned long currTime = event.GetTimestamp() - m_xsynct;
    if (evkey != m_Key)
    {
        m_Key = evkey;
        m_LastRedraw = m_StartTime = m_LastTime = currTime;
    }
    if (currTime >= m_LastRedraw)      // Redraw:
    {
        Action( m_Key, m_LastTime-m_StartTime, currTime-m_StartTime );
#if defined(__WXMAC__) && !defined(__DARWIN__)
        m_LastRedraw = currTime;    // StopWatch() doesn't work on Mac...
#else
        m_LastRedraw = StopWatch(&m_secbase) - m_gsynct;
#endif
        m_LastTime = currTime;
    }
    event.Skip();
}

void Demo_GLCanvas::OnKeyUp( wxKeyEvent& event )
{
    m_Key = 0;
    m_StartTime = 0;
    m_LastTime = 0;
    m_LastRedraw = 0;
    event.Skip();
}

void Demo_GLCanvas::Rotate( GLfloat deg )
{
    SetCurrent();
    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0f, 0.0f, 1.0f);
    Refresh(false);
}

#endif // WX_GL_DEMO

/*!
 * wxDialog_GL type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxDialog_GL, wxDialog_Base );

/*!
 * wxDialog_GL event table definition
 */

BEGIN_EVENT_TABLE( wxDialog_GL, wxDialog_Base )

////@begin wxDialog_GL event table entries
////@end wxDialog_GL event table entries

    END_EVENT_TABLE();

int wxDialog_GL::v_nRefCount = 0;

/*!
 * wxDialog_GL constructors
 */

wxDialog_GL::wxDialog_GL( )
        : v_pDemo_GLCanvas(NULL)
{
    ++v_nRefCount;
}

wxDialog_GL::wxDialog_GL( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
        : v_pDemo_GLCanvas(NULL)
{
    ++v_nRefCount;
    Create(parent, id, caption, pos, size, style);
}

/*!
 * wxDialog_GL creator
 */

bool wxDialog_GL::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxDialog_GL member initialisation
////@end wxDialog_GL member initialisation

////@begin wxDialog_GL creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog_Base::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end wxDialog_GL creation
    return true;
}

/*!
 * Control creation for wxDialog_GL
 */

void wxDialog_GL::CreateControls()
{
    _LOG_NFO("v_nRefCount=%d", v_nRefCount);
////@begin wxDialog_GL content construction
    wxDialog_GL* itemDialog_Base1 = this;

////@end wxDialog_GL content construction
    wxUnusedVar(itemDialog_Base1);
#ifdef WX_GL_DEMO
    v_pDemo_GLCanvas = new Demo_GLCanvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    v_pDemo_GLCanvas->v_nId = v_nRefCount;
#endif // WX_GL_DEMO
}

/*!
 * Should we show tooltips?
 */

bool wxDialog_GL::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxDialog_GL::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxDialog_GL bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxDialog_GL bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxDialog_GL::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxDialog_GL icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxDialog_GL icon retrieval
}

wxDialog_GL::~wxDialog_GL()
{
    --v_nRefCount;
}
