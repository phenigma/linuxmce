// demoDoc.h : interface of the CDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////
#define MAX_UNDO_LEVELS 8
#define MAX_SEL_POINTS 8000

#define	WM_USER_NEWIMAGE WM_USER+1
#define WM_USER_PROGRESS WM_USER+2

class CxImage;
class CDemoDoc : public CDocument
{
protected: // create from serialization only
	CDemoDoc();
	DECLARE_DYNCREATE(CDemoDoc)

// Attributes
public:
//	enum
//	{
//		WM_USER_NEWIMAGE = WM_USER+1
//	};

	CxImage *image;				//main image
	HANDLE	hThread,hProgress;	//elaboration thread

	void	*m_fp[4];	//function parameters
	long	m_MenuCommand;

	POINT m_Sel[MAX_SEL_POINTS]; //Selection
	long m_NumSel;
	int	m_tool;

	long m_hr[256]; //histogram
	long m_hg[256];
	long m_hb[256];
	long m_hgray[256];
	long m_hmax;

// Operations
public:
	void SubmitUndo();
	inline CxImage *GetImage() { return image; }
	inline BOOL GetStretchMode() { return stretchMode; }
	inline BOOL GetWaitingClick() { return m_WaitingClick; }
	inline void SetWaitingClick(BOOL b) { m_WaitingClick=b; }
	inline float GetZoomFactor() { return m_ZoomFactor; }
	int ComputePixel(float x, float y, float &x1, float &y1);
	CString FindExtension(const CString& name);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL DoSave(LPCTSTR pszPathName, BOOL bReplace =TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Stopwatch(int start0stop1);
	void UpdateStatusBar();
	virtual ~CDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	BOOL stretchMode;
	CxImage *imageUndo[MAX_UNDO_LEVELS];
	int m_UndoLevel;
	float m_ZoomFactor;
	BOOL m_WaitingClick;
	LARGE_INTEGER m_swFreq, m_swStart, m_swStop; //stopwatch
	float m_etime; //elapsed time
	long m_progress;

	//{{AFX_MSG(CDemoDoc)
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnStretchMode();
	afx_msg void OnUpdateStretchMode(CCmdUI* pCmdUI);
	afx_msg void OnTransformEllipse();
	afx_msg void OnWindowDuplicate();
	afx_msg void OnEditCopy();
	afx_msg void OnCximageFlip();
	afx_msg void OnCximageMirror();
	afx_msg void OnCximageNegative();
	afx_msg void OnCximageGrayscale();
	afx_msg void OnCximageRotate();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
	afx_msg void OnViewNormalviewing11();
	afx_msg void OnUpdateViewNormalviewing11(CCmdUI* pCmdUI);
	afx_msg void OnCximageSettransparency();
	afx_msg void OnUpdateCximageSettransparency(CCmdUI* pCmdUI);
	afx_msg void OnCximageRemovetransparency();
	afx_msg void OnUpdateCximageRemovetransparency(CCmdUI* pCmdUI);
	afx_msg void OnCximageResample();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageFlip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageGrayscale(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageMirror(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageNegative(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageResample(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageRotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransformEllipse(CCmdUI* pCmdUI);
	afx_msg void OnCximageDecreasebpp();
	afx_msg void OnCximageIncreasebpp();
	afx_msg void OnCximageOptions();
	afx_msg void OnCximageDither();
	afx_msg void OnUpdateCximageDither(CCmdUI* pCmdUI);
	afx_msg void OnCximageThreshold();
	afx_msg void OnUpdateCximageThreshold(CCmdUI* pCmdUI);
	afx_msg void OnCximageSplitrgb();
	afx_msg void OnUpdateCximageSplitrgb(CCmdUI* pCmdUI);
	afx_msg void OnCximageSplityuv();
	afx_msg void OnUpdateCximageSplityuv(CCmdUI* pCmdUI);
	afx_msg void OnCximageSplithsl();
	afx_msg void OnUpdateCximageSplithsl(CCmdUI* pCmdUI);
	afx_msg void OnCximagePseudocolors();
	afx_msg void OnUpdateCximagePseudocolors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageFiltersColorize(CCmdUI* pCmdUI);
	afx_msg void OnCximageFiltersColorize();
	afx_msg void OnCximageDarken();
	afx_msg void OnUpdateCximageDarken(CCmdUI* pCmdUI);
	afx_msg void OnCximageLighten();
	afx_msg void OnUpdateCximageLighten(CCmdUI* pCmdUI);
	afx_msg void OnCximageContrast();
	afx_msg void OnUpdateCximageContrast(CCmdUI* pCmdUI);
	afx_msg void OnCximageEmboss();
	afx_msg void OnUpdateCximageEmboss(CCmdUI* pCmdUI);
	afx_msg void OnCximageBlur();
	afx_msg void OnUpdateCximageBlur(CCmdUI* pCmdUI);
	afx_msg void OnCximageDilate();
	afx_msg void OnUpdateCximageDilate(CCmdUI* pCmdUI);
	afx_msg void OnCximageEdge();
	afx_msg void OnUpdateCximageEdge(CCmdUI* pCmdUI);
	afx_msg void OnCximageErode();
	afx_msg void OnUpdateCximageErode(CCmdUI* pCmdUI);
	afx_msg void OnCximageSharpen();
	afx_msg void OnUpdateCximageSharpen(CCmdUI* pCmdUI);
	afx_msg void OnCximageSoften();
	afx_msg void OnUpdateCximageSoften(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageCrop(CCmdUI* pCmdUI);
	afx_msg void OnCximageCrop();
	afx_msg void OnUpdateCximageRemovealphachannel(CCmdUI* pCmdUI);
	afx_msg void OnCximageRemovealphachannel();
	afx_msg void OnCximageOpacity();
	afx_msg void OnCximageInvetalpha();
	afx_msg void OnUpdateCximageInvetalpha(CCmdUI* pCmdUI);
	afx_msg void OnCximageAlphapalettetoggle();
	afx_msg void OnUpdateCximageAlphapalettetoggle(CCmdUI* pCmdUI);
	afx_msg void OnCximageAlphastrip();
	afx_msg void OnUpdateCximageAlphastrip(CCmdUI* pCmdUI);
	afx_msg void OnCximageRotatel();
	afx_msg void OnUpdateCximageRotatel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCximageRotater(CCmdUI* pCmdUI);
	afx_msg void OnCximageRotater();
	afx_msg void OnUpdateCximageGamma(CCmdUI* pCmdUI);
	afx_msg void OnCximageGamma();
	afx_msg void OnUpdateCximageMedian(CCmdUI* pCmdUI);
	afx_msg void OnCximageMedian();
	afx_msg void OnCximageAddnoise();
	afx_msg void OnUpdateCximageAddnoise(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToolsMove(CCmdUI* pCmdUI);
	afx_msg void OnViewToolsMove();
	afx_msg void OnViewToolsSelect();
	afx_msg void OnUpdateViewToolsSelect(CCmdUI* pCmdUI);
	afx_msg void OnViewToolsZoom();
	afx_msg void OnUpdateViewToolsZoom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPalette(CCmdUI* pCmdUI);
	afx_msg void OnViewPalette();
	afx_msg void OnCximageCombine();
	afx_msg void OnUpdateCximageCombine(CCmdUI* pCmdUI);
	afx_msg void OnCximageFft();
	afx_msg void OnUpdateCximageFft(CCmdUI* pCmdUI);
	afx_msg void OnCximageSplityiq();
	afx_msg void OnUpdateCximageSplityiq(CCmdUI* pCmdUI);
	afx_msg void OnCximageSplitxyz();
	afx_msg void OnUpdateCximageSplitxyz(CCmdUI* pCmdUI);
	afx_msg void OnCximageRepair();
	afx_msg void OnUpdateCximageRepair(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
