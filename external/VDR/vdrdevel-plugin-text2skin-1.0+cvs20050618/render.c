/*
 * $Id: render.c,v 1.29 2005/01/27 17:31:12 lordjaxom Exp $
 */

#include "render.h"
#include "loader.h"
#include "i18n.h"
#include "theme.h"
#include "bitmap.h"
#include "status.h"
#include "screen.h"
#include "display.h"
#include "scroller.h"
#include "xml/display.h"
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/menu.h>
#include <vdr/videodir.h>
#include <vdr/skinclassic.h>

cText2SkinRender *cText2SkinRender::mRender = NULL;

cText2SkinRender::cText2SkinRender(cText2SkinLoader *Loader, cxDisplay::eType Display, 
                                   const std::string &BasePath, bool OffScreen):
		mSkin(Loader->Data()),
		mDisplay(mSkin->Get(Display)),
		mI18n(Loader->I18n()),
		mTheme(Loader->Theme()),
		mScreen(NULL),
		mScroller(NULL),
		mBasePath(BasePath),
		mDirty(true),
		mFallback(NULL),
		mActive(false),
		mDoUpdate(),
		mDoUpdateMutex(),
		mStarted(),
		mUpdateIn(0),
		mNow(0),
		mBaseSize()
{
	if (mDisplay == NULL) {
		esyslog("ERROR: text2skin: display for %s missing", cxDisplay::GetType(Display).c_str());
		return;
	}

	mRender = this;
	Text2SkinStatus.SetRender(this);

	SetDescription("Text2Skin: %s display update", cxDisplay::GetType(Display).c_str());

	if (mBasePath.length() == 0)
		mBasePath = SkinPath() + "/" + mSkin->Name();

	if (mDisplay == NULL)
		return;

	mScreen = new cText2SkinScreen(OffScreen);
	if (!mScreen->IsOpen())
		return;

	mSkin->SetBase(); // refresh base coords in case the setup changed
	mBaseSize = mSkin->BaseSize();

	eOsdError res;
	tArea areas[mDisplay->NumWindows()];
	for (int i = 0; i < mDisplay->NumWindows(); ++i) {
		txPoint pos1 = Transform(mDisplay->Windows()[i].pos1);
		txPoint pos2 = Transform(mDisplay->Windows()[i].pos2);
		areas[i].x1 = mSkin->BaseOffset().x + pos1.x;
		areas[i].y1 = mSkin->BaseOffset().y + pos1.y;
		areas[i].x2 = mSkin->BaseOffset().x + pos2.x;
		areas[i].y2 = mSkin->BaseOffset().y + pos2.y;
		areas[i].bpp = mDisplay->Windows()[i].bpp;
		Dprintf("setting area: %d, %d, %d, %d, %d\n", areas[i].x1, areas[i].y1, areas[i].x2, 
		        areas[i].y2, areas[i].bpp);
	}
	res = mScreen->SetAreas(areas, mDisplay->NumWindows());

	if (res != oeOk) {
		const char *emsg = NULL;
		switch (res) {
		case oeTooManyAreas:
			emsg = "Too many OSD areas"; break;
		case oeTooManyColors:
			emsg = "Too many Colors"; break;
		case oeBppNotSupported:
			emsg = "Depth not supported"; break;
		case oeAreasOverlap:
			emsg = "Areas are overlapping"; break;
		case oeWrongAlignment:
			emsg = "Areas not correctly aligned"; break;
		case oeOutOfMemory:
			emsg = "OSD memory overflow"; break;
		case oeUnknown:
			emsg = "Unknown OSD error"; break;
		default:
			break;
		}
		esyslog("ERROR: text2skin: OSD provider can't handle skin: %s\n", emsg);
		DELETENULL(mScreen);
		mFallback = new cSkinClassic();
		return;
	}

	if (!OffScreen) {
		UpdateLock();
		Start();
		mStarted.Wait(mDoUpdateMutex);
		UpdateUnlock();
	}
}

cText2SkinRender::~cText2SkinRender() 
{
	if (mActive) {
		mActive = false;
		Flush(true);
		Cancel(3);
	}
	delete mScroller;
	delete mScreen;

	Text2SkinStatus.SetRender(NULL);
	mRender = NULL;
}

void cText2SkinRender::Action(void) 
{
	mActive = true;
	UpdateLock();
	mStarted.Broadcast();
	while (mActive) {
		if (mUpdateIn) mDoUpdate.TimedWait(mDoUpdateMutex, mUpdateIn);
		else           mDoUpdate.Wait(mDoUpdateMutex);

		if (!mActive)  break; // fall out if thread to be stopped

		mUpdateIn = 0; // has to be re-set within Update();
		mNow = time_ms();
		Update();
	}
	UpdateUnlock();
}

void cText2SkinRender::Update(void) 
{
	Dbench(update);

	for (uint i = 0; i < mDisplay->Objects(); ++i)
		DrawObject(mDisplay->GetObject(i));

	Dbench(flush);
	mScreen->Flush();
	Ddiff("flush only", flush);
	Ddiff("complete flush", update);
}

void cText2SkinRender::DrawObject(const cxObject *Object)
{
	if (Object->Condition() != NULL && !Object->Condition()->Evaluate())
		return;

	switch (Object->Type()) {
	case cxObject::image:
		DrawImage(Object->Pos(), Object->Size(), Object->Bg(), Object->Fg(), Object->Mask(),
		          Object->Alpha(), Object->Colors(), Object->Path());
		break;

	case cxObject::text:
		DrawText(Object->Pos(), Object->Size(), Object->Fg(), Object->Text(), Object->Font(), 
		         Object->Align());
		break;

	case cxObject::marquee:
		DrawMarquee(Object->Pos(), Object->Size(), Object->Fg(), Object->Text(), Object->Font(), 
		            Object->Align(), Object->Delay(), Object->Index());
		break;

	case cxObject::blink:
		DrawBlink(Object->Pos(), Object->Size(), Object->Fg(), Object->Bg(), Object->Text(), 
		          Object->Font(), Object->Align(), Object->Delay(), Object->Index());
		break;

	case cxObject::rectangle:
		DrawRectangle(Object->Pos(), Object->Size(), Object->Fg());
		break;

	case cxObject::ellipse:
		DrawEllipse(Object->Pos(), Object->Size(), Object->Fg(), Object->Arc());
		break;
	
	case cxObject::slope:
		DrawSlope(Object->Pos(), Object->Size(), Object->Fg(), Object->Arc());
		break;

	case cxObject::progress:
		DrawProgressbar(Object->Pos(), Object->Size(), Object->Current(), Object->Total(), 
		                Object->Bg(), Object->Fg(), Object->Keep(), Object->Mark(), 
		                Object->Active(), GetMarks());
		break;

	case cxObject::scrolltext:
		DrawScrolltext(Object->Pos(), Object->Size(), Object->Fg(), Object->Text(), Object->Font(), 
		               Object->Align());
		break;

	case cxObject::scrollbar:
		DrawScrollbar(Object->Pos(), Object->Size(), Object->Bg(), Object->Fg());

	case cxObject::block:
		for (uint i = 0; i < Object->Objects(); ++i)
			DrawObject(Object->GetObject(i));
		break;

	case cxObject::list: {
			Dprintf("list\n");
			const cxObject *item = Object->GetObject(0);
			if (item && item->Type() == cxObject::item) {
				txSize areasize = Object->Size();
				uint itemheight = item->Size().h;
				uint maxitems = areasize.h / itemheight;
				uint yoffset = 0;

				SetMaxItems(maxitems); //Dprintf("setmaxitems %d\n", maxitems);
				for (uint i = 0; i < maxitems; ++i, yoffset += itemheight) {
					for (uint j = 1; j < Object->Objects(); ++j) {
						const cxObject *o = Object->GetObject(j);
						int maxtabs = 1;

						if (o->Display()->Type() == cxDisplay::menu)
							maxtabs = cSkinDisplayMenu::MaxTabs;

						for (int t = -1; t < maxtabs; ++t) {
							if (!HasTabText(i, t))
								continue;

							int thistab = GetTab(t);
							int nexttab = GetTab(t + 1);

							cxObject obj(*o);
							obj.SetListIndex(i, t);
							if (obj.Condition() != NULL && !obj.Condition()->Evaluate())
								continue;

							obj.mPos1.x += Object->mPos1.x + (t >= 0 ? thistab : 0);
							obj.mPos1.y += Object->mPos1.y + yoffset;

							// get end position
							if (t >= 0 && nexttab > 0) {
								// there is a "next tab".. see if it contains text
								int n = t + 1;
								while (n < cSkinDisplayMenu::MaxTabs && !HasTabText(i, n))
									++n;
								nexttab = GetTab(n);
							}
							
							if (t >= 0 && nexttab > 0 && nexttab < obj.mPos1.x + obj.Size().w - 1)
								// there is a "next tab" with text
								obj.mPos2.x = Object->mPos1.x + o->mPos1.x + nexttab;
							else {
								// there is no "next tab", use the rightmost edge
								obj.mPos2.x += Object->mPos1.x;
								SetEditableWidth(obj.Size().w);
							}

							obj.mPos2.y += Object->mPos1.y + yoffset;

							std::string text = obj.Text();
							bool isprogress = false;
							if (text.length() > 5 
									&& text[0] == '[' && text[text.length() - 1] == ']') {
								const char *p = text.c_str() + 1;
								isprogress = true;
								for (; *p != ']'; ++p) {
									if (*p != ' ' && *p != '|') {
										isprogress = false;
										break;
									}
								}
							}

							if (isprogress) {
								//Dprintf("detected progress bar tab\n");
								if (obj.Condition() == NULL || obj.Condition()->Evaluate()) {
									int total = text.length() - 2;
									int current = 0;
									const char *p = text.c_str() + 1;
									while (*p == '|')
										(++current, ++p);

									txPoint pos = obj.Pos();
									txSize size = obj.Size();

									DrawRectangle(txPoint(pos.x, pos.y + 4), 
									              txSize(size.w, 2), obj.Fg());
									DrawRectangle(txPoint(pos.x, pos.y + 4),
									              txSize(2, size.h - 8), obj.Fg());
									DrawRectangle(txPoint(pos.x, pos.y + size.h - 6), 
									              txSize(size.w, 2), obj.Fg());
									DrawRectangle(txPoint(pos.x + size.w - 2, pos.y + 4),
									              txSize(2, size.h - 8), obj.Fg());

									pos.x += 4;
									pos.y += 8;
									size.w -= 8;
									size.h -= 16;
									DrawProgressbar(pos, size, current, total, obj.Bg(),
									                obj.Fg(), NULL, NULL, NULL, NULL);
								}
							} else
								DrawObject(&obj);
						}
					}
				}
			}
		}
		break;

	case cxObject::item:
		// ignore
		break;

	}
}

void cText2SkinRender::DrawImage(const txPoint &Pos, const txSize &Size, const tColor *Bg, 
                                 const tColor *Fg, const tColor *Mask, int Alpha, int Colors, 
                                 const std::string &Path)
{
	cText2SkinBitmap *bmp;
	//Dprintf("trying to draw image %s to %dx%d - alpha %d\n", ImagePath(Path).c_str(), Pos.x, Pos.y, Alpha);

	if ((bmp = cText2SkinBitmap::Load(ImagePath(Path), Alpha, Size.h > 1 ? Size.h : 0, 
	                                  Size.w > 1 ? Size.w : 0, Colors)) != NULL) {
		//Dprintf("success loading image\n");
		if (Bg) bmp->SetColor(0, *Bg);
		if (Fg) bmp->SetColor(1, *Fg);
		mScreen->DrawBitmap(Pos.x, Pos.y, bmp->Get(mUpdateIn, mNow), Mask);
	}
}

void cText2SkinRender::DrawText(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
                                const std::string &Text, const cFont *Font, int Align) 
{
	//Dprintf("trying to draw text %s to %d,%d size %d,%d, color %x\n", Text.c_str(), Pos.x, Pos.y, 
	//        Size.w, Size.h, Fg ? *Fg : 0);
	mScreen->DrawText(Pos.x, Pos.y, Text.c_str(), Fg ? *Fg : 0, 0, Font, Size.w, Size.h, Align);
}

void cText2SkinRender::DrawMarquee(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
                                   const std::string &Text, const cFont *Font, int Align, 
                                   uint Delay, uint Index)
{
	bool scrolling = Font->Width(Text.c_str()) > Size.w;

	tState &state = mStates[Index];
	if (state.text != Text) {
		state = tState();
		state.text = Text;
	}
	
	if (state.nexttime == 0)
		state.nexttime = mNow + 1500;
	else if (mNow >= state.nexttime) {
		uint nextin = Delay;
		if (state.direction > 0) {
			if (Font->Width(Text.c_str() + state.offset) <= Size.w) {
				--state.direction;
				nextin = 1500;
			} else
				++state.offset;
		}
		else {
			if (state.offset <= 0) {
				++state.direction;
				nextin = 1500;
			} else
				--state.offset;
		}
		state.nexttime = mNow + nextin;
	}

	if (scrolling) {
		uint updatein = state.nexttime - mNow;
		if (mUpdateIn == 0 || updatein < mUpdateIn)
			mUpdateIn = updatein;
	}
	//Dprintf("drawMarquee text = %s, state.text = %s, offset = %d, index = %d, scrolling = %d, mUpdatteIn = %d, nexttime = %d, delay = %d\n", 
	//        Text.c_str(), state.text.c_str(), state.offset, Index, scrolling, mUpdateIn, state.nexttime, Delay);
		
	mScreen->DrawText(Pos.x, Pos.y, Text.c_str() + state.offset, Fg ? *Fg : 0, clrTransparent, Font,
	                  Size.w, Size.h, Align);
}
	
void cText2SkinRender::DrawBlink(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
                                 const tColor *Bg, const std::string &Text, const cFont *Font, 
                                 int Align, uint Delay, uint Index)
{
	tState &state = mStates[Index];
	if (state.text != Text) {
		state = tState();
		state.text = Text;
	}
	Dprintf("drawBlink index = %d, state.text = %s, offset = %d\n", Index, state.text.c_str(), 
	        state.offset);

	if (state.nexttime == 0 || mNow >= state.nexttime) {
		state.nexttime = mNow + Delay;
		state.offset = 1 - state.offset;
	}

	uint updatein = state.nexttime - mNow;
	if (mUpdateIn == 0 || updatein < mUpdateIn)
		mUpdateIn = updatein;

	const tColor *col = state.offset == 0 ? Fg : Bg;
	if (col)
		mScreen->DrawText(Pos.x, Pos.y, Text.c_str(), *col, clrTransparent, Font, Size.w, Size.h, 
		                  Align);
}

void cText2SkinRender::DrawRectangle(const txPoint &Pos, const txSize &Size, const tColor *Fg) 
{
	mScreen->DrawRectangle(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0);
}

void cText2SkinRender::DrawEllipse(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
                                   int Arc) 
{
	mScreen->DrawEllipse(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0, Arc);
}

void cText2SkinRender::DrawSlope(const txPoint &Pos, const txSize &Size, const tColor *Fg, int Arc) {
	mScreen->DrawSlope(Pos.x, Pos.y, Pos.x + Size.w - 1, Pos.y + Size.h - 1, Fg ? *Fg : 0, Arc);
}

void cText2SkinRender::DrawProgressbar(const txPoint &Pos, const txSize &Size, int Current, 
                                       int Total, const tColor *Bg, const tColor *Fg, 
                                       const tColor *Selected, const tColor *Mark, 
                                       const tColor *Cur, const cMarks *Marks) 
{
	//Dprintf("trying to draw Progressbar, Current = %d, Total = %d, bg = %x, marks = %p\n", 
	//        Current, Total, Bg ? *Bg : 0, Marks);
	if (Bg)
		DrawRectangle(Pos, Size, Bg);
	if (Total == 0)
		Total = 1;
	if (Current > Total)
		Current = Total;
	if (Size.w > Size.h) {
		DrawRectangle(Pos, txSize(Size.w * Current / Total, Size.h), Fg);

		if (Marks) {
			bool start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				txPoint pt(Pos.x + m->position * Size.w / Total, Pos.y);
				if (Selected && start) {
					const cMark *m2 = Marks->Next(m);
					DrawRectangle(txPoint(pt.x, Pos.y + Size.h / 3), 
					              txSize(((m2 ? m2->position : Total) - m->position) 
					              * Size.w / Total, Size.h / 3), Selected);
				}
				DrawMark(pt, Size, start, m->position == Current, false, Mark, Cur);
				start = !start;
			}
		}
	} else {
		DrawRectangle(Pos, txSize(Size.w, Size.h * Current / Total), Fg);
		
		if (Marks) {
			bool start = true;
			for (const cMark *m = Marks->First(); m; m = Marks->Next(m)) {
				txPoint pt(Pos.x, Pos.y + m->position * Size.h / Total);
				if (Selected && start) {
					const cMark *m2 = Marks->Next(m);
					DrawRectangle(txPoint(Pos.x + Size.w / 3, pt.y), 
					              txSize(Size.w / 3, ((m2 ? m2->position : Total) - m->position) 
					              * Size.h / Total), Selected);
				}
				DrawMark(pt, Size, start, m->position == Current, true, Mark, Cur);
				start = !start;
			}
		}
	}
}

void cText2SkinRender::DrawMark(const txPoint &Pos, const txSize &Size, bool Start, bool Current, 
                                bool Horizontal, const tColor *Mark, const tColor *Cur)
{
	txPoint p1 = Pos;
	if (Horizontal) {
		if (Mark)
			DrawRectangle(p1, txSize(Size.w, 1), Mark);
		const int d = Size.w / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			const tColor *col = Current ? Cur : Mark;
			int h = Start ? i : Size.w - 1 - i;
			if (col)
				DrawRectangle(txPoint(Pos.x + h, Pos.y - d + i), txSize(1, (d - i) * 2), col);
		}
	} else {
		if (Mark)
			DrawRectangle(p1, txSize(1, Size.h), Mark);
		const int d = Size.h / (Current ? 3 : 9);
		for (int i = 0; i < d; i++) {
			const tColor *col = Current ? Cur : Mark;
			int h = Start ? i : Size.h - 1 - i;
			if (col)
				DrawRectangle(txPoint(Pos.x - d + i, Pos.y + h), txSize((d - i) * 2, 1), col);
		}
	}
}

void cText2SkinRender::DrawScrolltext(const txPoint &Pos, const txSize &Size, const tColor *Fg, 
                                      const std::string &Text, const cFont *Font, int /*Align*/)
{
	//Dprintf("trying to draw scrolltext %s\n", Text.c_str());
	if (mScroller == NULL)
		mScroller = new cText2SkinScroller(mScreen, Pos.x, Pos.y, Size.w, Size.h, Text.c_str(), 
		                                   Font, Fg ? *Fg : 0, clrTransparent);
	else
		mScroller->DrawText();
}
	
void cText2SkinRender::DrawScrollbar(const txPoint &Pos, const txSize &Size, const tColor *Bg, 
                                     const tColor *Fg) 
{
	if (mScroller != NULL) {
		int total = mScroller->Total();
		DrawRectangle(Pos, Size, Bg);
		if (total == 0)
			total = 1;
		if (Size.h > Size.w) {
			txPoint sp = Pos;
			txSize ss = Size;
			sp.y += Size.h * mScroller->Offset() / total;
			ss.h = Size.h * mScroller->Shown() / total + 1;
			DrawRectangle(sp, ss, Fg);
		} else {
			txPoint sp = Pos;
			txSize ss = Size;
			sp.x += Size.w * mScroller->Offset() / total;
			ss.w = Size.w * mScroller->Shown() / total + 1;
			DrawRectangle(sp, ss, Fg);
		}
	}
}

txPoint cText2SkinRender::Transform(const txPoint &Pos) 
{
	txSize base = mRender->mBaseSize;
	return txPoint(Pos.x < 0 ? base.w + Pos.x : Pos.x, Pos.y < 0 ? base.h + Pos.y : Pos.y);
}

bool cText2SkinRender::ItemColor(const std::string &Color, tColor &Result) 
{
	if (Color != "" && Color != "None") {
		if (Color[0] == '#')
			Result = strtoul(Color.c_str() + 1, NULL, 16);
		else
			Result = mRender->mTheme->Color(Color);
	} else
		return false;
	return true;
}

std::string cText2SkinRender::ImagePath(const std::string &Filename) 
{
	if (mRender)
		return mRender->mBasePath + "/" + Filename;
	return "";
}

cxType cText2SkinRender::GetToken(const txToken &Token) 
{
	if (mRender != NULL) {
		tTokenCache::iterator it = mRender->mTokenCache.find(Token);
		if (it != mRender->mTokenCache.end())
			return (*it).second;

		cxType res = mRender->GetTokenData(Token);
		if      (Token.Attrib.Type == aClean) {
			std::string str = res.String();
			int pos = -1;

			if (Token.Type == tMenuCurrent) {
				const char *ptr = str.c_str(); 
				char *end;
				int n = strtoul(ptr, &end, 10);
				if (n != 0)
					res = skipspace(end);
				else
					res = ptr;
				Dprintf("MenuCurrent result: |%s|\n", res.String().c_str());
			}
			else if (Token.Type == tMenuTitle) {
				if ((pos = str.find(" - ")) != -1) {
					str.erase(pos);
					while (str[str.length() - 1] == ' ')
						str.erase(str.length() - 1);
					res = str;
				}
				Dprintf("MenuTitle 'clean' result: |%s|\n", res.String().c_str());
			}
			else if (Token.Type == tReplayTitle) {
				if (Text2SkinStatus.ReplayMode() == cText2SkinStatus::replayMP3) {
					str.erase(0, 4);
					res = str;
				}
				Dprintf("ReplayTitle result: |%s|\n", res.String().c_str());
			}
		}
		else if (Token.Attrib.Type == aRest) {
			std::string str = res.String();
			int pos = -1;

			if (Token.Type == tMenuTitle) {
				if ((pos = str.find(" - ")) != -1) {
					str.erase(0, pos + 3);
					while (str[0] == ' ')
						str.erase(0, 1);
					res = str;
				}
				Dprintf("MenuTitle 'rest' result: |%s|\n", res.String().c_str());
			}
		}

		if (res.UpdateIn() > 0) {
			Dprintf("Passing token without cacheing\n");
			if (mRender->mUpdateIn == 0 || res.UpdateIn() < mRender->mUpdateIn) {
				Dprintf("updating in %d\n", res.UpdateIn());
				mRender->mUpdateIn = res.UpdateIn();
			}
		} else
			mRender->mTokenCache[Token] = res;

		return res;
	}
	return false;
}

cxType cText2SkinRender::GetTokenData(const txToken &Token) 
{
#define MB_PER_MINUTE 25.75 // this is just an estimate!
	switch (Token.Type) {
	case tFreeDiskSpace: {
			int FreeMB;
			VideoDiskSpace(&FreeMB);
			Dprintf("FreeMB: %d, attrib type is %d\n", FreeMB,Token.Attrib.Type);
			return Token.Attrib.Type == aString && Token.Attrib.Text.length() > 0
			       ? (cxType)DurationType((int)(FreeMB * 60 * FRAMESPERSEC / MB_PER_MINUTE), 
			                              Token.Attrib.Text)
			       : (cxType)FreeMB;
		}

	case tDateTime:      return TimeType(time(NULL), Token.Attrib.Text);

	case tCanScrollUp:   return mScroller != NULL && mScroller->CanScrollUp();

	case tCanScrollDown: return mScroller != NULL && mScroller->CanScrollDown();
	
	case tIsRecording:   return cRecordControls::Active();

#if VDRVERSNUM >=10318
	case tAudioTrack:    {
			cDevice *dev = cDevice::PrimaryDevice();
			const tTrackId *Track = dev->GetTrack(dev->GetCurrentAudioTrack());
			return Track
			       ? (cxType)Track->description
			       : (cxType)false;
		}

	case tAudioChannel:
		return cText2SkinDisplayTracks::ChannelName(cDevice::PrimaryDevice()->GetAudioChannel());
#endif

	default:             return Text2SkinStatus.GetTokenData(Token);
	}
}
