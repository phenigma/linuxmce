/*
 *  $Id: object.c,v 1.11 2005/01/26 20:39:34 lordjaxom Exp $
 */

#include "xml/object.h"
#include "render.h"
#include "font.h"

static const std::string ObjectNames[] =
	{ "image", "text", "marquee", "blink", "rectangle", "ellipse", "slope", "progress", 
	  "scrolltext", "scrollbar", "block", "list", "item" };

cxObject::cxObject(cxDisplay *Parent):
		mDisplay(Parent),
		mSkin(Parent->Skin()),
		mType((eType)__COUNT_OBJECT__),
		mPos1(0, 0),
		mPos2(-1, -1),
		mAlpha(255),
		mColors(0),
		mArc(0),
		mPath(this, false),
		mText(this, true),
		mAlign(taDefault),
		mCondition(NULL),
		mCurrent(this, false),
		mTotal(this, false),
		mFontFace("Osd"),
		mFontSize(0),
		mFontWidth(0),
		mDelay(150),
		mIndex(0),
		mObjects(NULL)
{
}

cxObject::cxObject(const cxObject &Src):
		mDisplay(Src.mDisplay),
		mSkin(Src.mSkin),
		mType(Src.mType),
		mPos1(Src.mPos1),
		mPos2(Src.mPos2),
		mAlpha(Src.mAlpha),
		mColors(Src.mColors),
		mArc(Src.mArc),
		mFg(Src.mFg),
		mBg(Src.mBg),
		mMask(Src.mMask),
		mMark(Src.mMark),
		mActive(Src.mActive),
		mKeep(Src.mKeep),
		mPath(Src.mPath),
		mText(Src.mText),
		mAlign(Src.mAlign),
		mCondition(NULL),
		mCurrent(Src.mCurrent),
		mTotal(Src.mTotal),
		mFontFace(Src.mFontFace),
		mFontSize(Src.mFontSize),
		mFontWidth(Src.mFontWidth),
		mDelay(Src.mDelay),
		mObjects(NULL)
{
	if (Src.mCondition)
		mCondition = new cxFunction(*Src.mCondition);
	if (Src.mObjects)
		mObjects = new cxObjects(*Src.mObjects);
}

cxObject::~cxObject() 
{
	delete mCondition;
	delete mObjects;
}

bool cxObject::ParseType(const std::string &Text) 
{
	for (int i = 0; i < (int)__COUNT_OBJECT__; ++i) {
		if (ObjectNames[i] == Text) {
			mType = (eType)i;
			return true;
		}
	}
	return false;
}

bool cxObject::ParseCondition(const std::string &Text) 
{
	cxFunction *result = new cxFunction(this);
	if (result->Parse(Text)) {
		delete mCondition;
		mCondition = result;
		return true;
	}
	return false;
}

bool cxObject::ParseAlignment(const std::string &Text) 
{
	if      (Text == "center") mAlign = (eTextAlignment)(taTop | taCenter);
	else if (Text == "right")  mAlign = (eTextAlignment)(taTop | taRight);
	else if (Text == "left")   mAlign = (eTextAlignment)(taTop | taLeft);
	else 
		return false;
	return true;
}

bool cxObject::ParseFontFace(const std::string &Text) 
{
	int size = 0, width = 0, pos;
	std::string face = Text;
	if ((pos = face.find(':')) != -1) {
		std::string s = face.substr(pos + 1);
		const char *p = s.c_str(); 
		char *end;
		size = strtol(p, &end, 10);
		if (*end == ',')
			width = strtol(end + 1, NULL, 10);
		
		face.erase(pos);
	}

	mFontFace  = face;
	mFontSize  = size;
	mFontWidth = width;
	return true;
}

void cxObject::SetListIndex(uint Index, int Tab)
{
	mIndex = mDisplay->Objects() + (Index * cSkinDisplayMenu::MaxTabs + Tab);
	mText.SetListIndex(Index, Tab);
	mPath.SetListIndex(Index, Tab);
	if (mCondition != NULL)
		mCondition->SetListIndex(Index, Tab);
}

const std::string &cxObject::TypeName(void) const 
{
	return ObjectNames[mType];
}

const cFont *cxObject::Font(void) const
{
	const cFont *font;

	if ((font = cText2SkinFont::Load(SkinPath() + "/fonts", mFontFace, mFontSize, mFontWidth)) 
	        != NULL)
		return font;
	
	if ((font = cText2SkinFont::Load(SkinPath() + "/" + mSkin->Name(), mFontFace, mFontSize, 
	                                 mFontWidth)) != NULL)
		return font;
	
	return cFont::GetFont(fontOsd);
}

txPoint cxObject::Pos(void) const 
{
	return txPoint(mSkin->BaseOffset().x + (mPos1.x < 0 ? Skin()->BaseSize().w + mPos1.x : mPos1.x), 
			       mSkin->BaseOffset().y + (mPos1.y < 0 ? Skin()->BaseSize().h + mPos1.y : mPos1.y));
}

txSize cxObject::Size(void) const 
{
	txPoint p1(mSkin->BaseOffset().x + (mPos1.x < 0 ? Skin()->BaseSize().w + mPos1.x : mPos1.x), 
			   mSkin->BaseOffset().y + (mPos1.y < 0 ? Skin()->BaseSize().h + mPos1.y : mPos1.y));
	txPoint p2(mSkin->BaseOffset().x + (mPos2.x < 0 ? Skin()->BaseSize().w + mPos2.x : mPos2.x), 
			   mSkin->BaseOffset().y + (mPos2.y < 0 ? Skin()->BaseSize().h + mPos2.y : mPos2.y));
	return txSize(p2.x - p1.x + 1, p2.y - p1.y + 1);
}

const tColor *cxObject::Fg(void) const 
{
	static tColor Fg;
	return cText2SkinRender::ItemColor(mFg, Fg) ? &Fg : NULL;
}

const tColor *cxObject::Bg(void) const 
{
	static tColor Bg;
	return cText2SkinRender::ItemColor(mBg, Bg) ? &Bg : NULL;
}

const tColor *cxObject::Mask(void) const 
{
	static tColor Mask;
	return cText2SkinRender::ItemColor(mMask, Mask) ? &Mask : NULL;
}

const tColor *cxObject::Mark(void) const 
{
	static tColor Mark;
	return cText2SkinRender::ItemColor(mMark, Mark) ? &Mark : NULL;
}

const tColor *cxObject::Active(void) const 
{
	static tColor Active;
	return cText2SkinRender::ItemColor(mActive, Active) ? &Active : NULL;
}

const tColor *cxObject::Keep(void) const 
{
	static tColor Keep;
	return cText2SkinRender::ItemColor(mKeep, Keep) ? &Keep : NULL;
}

cxObjects::cxObjects(void)
{
}

cxObjects::~cxObjects() 
{
	for (uint i = 0; i < size(); ++i)
		delete operator[](i);
}

