#ifndef schemeconfig_h__included
#define schemeconfig_h__included

#include "SchemeCompiler.h"

class CustomStyleCollection
{
	_NO_COPY(CustomStyleCollection)
	public:
		CustomStyleCollection();
		~CustomStyleCollection();

		virtual void AddStyle(StyleDetails* pStyle);

		CustomStyleCollection* GetNext();
		void SetNext(CustomStyleCollection* pNext);
		void SetName(LPCTSTR name);
		void SetDescription(LPCTSTR description);

		LPCTSTR GetName();
		LPCTSTR GetDescription();

		StyleDetails* GetStyle(int key);

		STYLES_LIST	m_Styles;

	protected:
		ctcString m_name;
		ctcString m_description;

		CustomStyleCollection* m_pNext;
};

class CustomStyleHolder : public CustomStyleCollection
{
	_NO_COPY(CustomStyleHolder)
	public:
		CustomStyleHolder();
		virtual void AddStyle(StyleDetails* pStyle);

		void BeginGroup(LPCTSTR name, LPCTSTR description = NULL);
		void EndGroup();
	protected:
        CustomStyleCollection* m_pCurrent;
};

class SchemeConfig : public CustomKeywordHolder, public CustomStyleHolder
{
	_NO_COPY(SchemeConfig)
	public:
		SchemeConfig(){}

		CString m_Name;
		CString m_Title;
		int m_foldflags;

		CustomStyleCollection m_customs;
};

typedef list<SchemeConfig*>	LIST_SCHEMECONFIGS;
typedef LIST_SCHEMECONFIGS::iterator SCF_IT;

class SchemeConfigParser : public SchemeParser
{
	_NO_COPY(SchemeConfigParser)
	public:
		SchemeConfigParser();
		~SchemeConfigParser();

		void LoadConfig(LPCTSTR path, LPCTSTR compiledpath);

		LIST_SCHEMECONFIGS& GetSchemes();

	protected:
		void Sort();
		LIST_SCHEMECONFIGS	m_Schemes;
		SchemeConfig*		m_pCurrent;

	// SchemeParser
	protected:
		virtual void onLexer(LPCTSTR name, int styleBits);
		virtual void onLanguage(LPCTSTR name, LPCTSTR title, int foldflags);
		virtual void onLanguageEnd();
		virtual void onStyleGroup(XMLAttributes& att);
		virtual void onStyle(StyleDetails* pStyle, StyleDetails* pCustom);
		virtual void onStyleGroupEnd();
		virtual void onKeywords(int key, LPCTSTR keywords);
		virtual void onFile(LPCTSTR filename);

		
};


#endif