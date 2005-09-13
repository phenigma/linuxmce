#ifndef MenuNode_h
#define MenuNode_h

namespace DCE
{
	class MenuNode
	{
	public:
		vector<MenuNode *> m_vectMenuNode; // Child nodes
		class MenuStructure *m_pMenuStructure;
		string m_sText;
		MenuNode *m_pMenuNode_GoBack;

		MenuNode(MenuStructure *pMenuStructure,string sText) { m_pMenuNode_GoBack=NULL; m_pMenuStructure=pMenuStructure; m_sText=sText; m_iSelectedEntry=0; m_iFirstDrawnEntry=0; }
		void Render(vector<string> *str);
		string GetText() { return m_sText; }

		int m_iSelectedEntry,m_iFirstDrawnEntry;

		virtual bool CanGoRight() { return m_vectMenuNode.size()>0; };
		virtual void Up();
		virtual void Down();
		virtual void Left();
		virtual void Right();
		virtual void Enter();
		virtual void Back();
		virtual void Home();

		virtual void Rerender();
	};
}

#endif
