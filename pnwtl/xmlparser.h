/**
 * @file xmlparser.h
 * @brief Define an XML parser framework using expat (http://expat.sourceforge.net/).
 * @author Simon Steele
 * @note Copyright (c) 2002 Simon Steele <s.steele@pnotepad.org>
 *
 * Programmers Notepad 2 : The license file (license.[txt|html]) describes 
 * the conditions under which this source may be modified / distributed.
 *
 * Unicode Status: Unicode Ready (tested).
 */

#ifndef xmlparser_h__included
#define xmlparser_h__included

#ifdef _UNICODE
	#define XML_UNICODE
	#pragma comment(lib,"lib/libexpatw.lib") 
	#pragma message("Automatically linking with libexpatw.lib")
#else
	#pragma comment(lib,"lib/libexpat.lib") 
	#pragma message("Automatically linking with libexpat.lib")
#endif

#include "expat.h"

static const TCHAR* tszXMLParserDefaultException = _T("Exception while parsing XML.");

/**
 * @class XMLAttributes
 * @brief Simple wrapper class for a char** based set of attributes.
 */
class XMLAttributes
{
	public:
		XMLAttributes(LPCTSTR * atts);

		LPCTSTR getName(int index);
		
		LPCTSTR getValue(int index);
		LPCTSTR getValue(LPCTSTR name);
		LPCTSTR operator [] (int index);

		int getCount();

	protected:
		LPCTSTR * m_atts;
		int m_count;
};

/**
 * @class XMLParseState
 * @brief Abstract base class for implementing XML parser callbacks.
 *
 * You should either inherit from this class or use one of the callback
 * implementations like XMLParserCallback.
 */
class XMLParseState
{
	public:
		virtual void startElement(LPCTSTR name, XMLAttributes& atts) = 0;
		virtual void endElement(LPCTSTR name) = 0;
		virtual void characterData(LPCTSTR data, int len) = 0;
};

template <typename T>
class XMLParserCallback : public XMLParseState
{
	public:
		typedef void (T::*SF)(void *userData, LPCTSTR name, XMLAttributes& atts);
		typedef void (T::*EF)(void *userData, LPCTSTR name);
		typedef void (T::*CD)(void *userData, LPCTSTR data, int len);

		XMLParserCallback(T& t, SF elementStartFn, EF elementEndFn) :
			m_t(&t), m_sf(elementStartFn), m_ef(elementEndFn), m_cd(NULL) {}

		XMLParserCallback(T& t, SF elementStartFn, EF elementEndFn, CD characterDataFn) :
			m_t(&t), m_sf(elementStartFn), m_ef(elementEndFn), m_cd(characterDataFn) {}

		void SetCharacterDataCallback(CD characterDataFn)
		{
			m_cd = characterDataFn;
		}

		void SetUserData(void* data)
		{
			m_userData = data;
		}

		virtual void startElement(LPCTSTR name, XMLAttributes& atts)
		{
			(m_t->*m_sf)(m_userData, name, atts);
		}

		virtual void endElement(LPCTSTR name)
		{
			(m_t->*m_ef)(m_userData, name);
		}

		virtual void characterData(LPCTSTR data, int len)
		{
			if(m_cd)
			{
				(m_t->*m_cd)(m_userData, data, len);
			}
		}

	protected:
		T*	m_t;
		SF	m_sf;
		EF	m_ef;
		CD	m_cd;

		void* m_userData;
};

/**
 * @class XMLParser
 * @author Simon Steele - s.steele@pnotepad.org
 * @brief Wrapper for an expat XML parser.
 *
 * This class wraps the operations involved in using
 * expat. It also allows for the use of callbacks and
 * multiple inheritance through the XMLParseState
 * class. A parse state must be specified before load
 * file is called.
 */
class XMLParser
{
	public:
		XMLParser();
		~XMLParser();
		bool LoadFile(LPCTSTR filename);
		void SetParseState(XMLParseState* pState);
		void Reset();

		XML_Parser	GetParser();
		LPCTSTR		GetFileName();

	protected:
		XML_Parser		m_parser;
		XMLParseState*	m_pState;
		TCHAR*			m_szFilename;
};

/**
 * @class XMLParserException
 * @author Simon Steele - s.steele@pnotepad.org
 * @brief Exception object to be thrown by an XML parser
 * 
 * When constructed, this class stores the line and column
 * on which an error occured. Optionally, an error code can
 * also be stored.
 */
class XMLParserException
{
	public:
		XMLParserException(XMLParser* pParser, LPCTSTR msg = NULL)
		{
			m_errcode = 0;
			m_filename = NULL;
			set(pParser, msg);
		}

		XMLParserException(XMLParser* pParser, int ErrorCode = 0, LPCTSTR msg = NULL)
		{
			m_errcode = ErrorCode;
			m_filename = NULL;
			set(pParser, msg);
		}

		XMLParserException(const XMLParserException& copy)
		{
			throw "Can't copy me, use a reference!";
		}
		
		virtual ~XMLParserException()
		{
			if(m_filename)
			{
				delete [] m_filename;
				m_filename = NULL;
			}
		}

		LPCTSTR GetFileName() { return m_filename; }
		LPCTSTR	GetMessage() { return m_msg; }
		int		GetLine() { return m_line; }
		int		GetColumn()	{ return m_column; }
		int		GetErrorCode() { return m_errcode; }

	protected:
		void set(XMLParser* pParser, LPCTSTR msg)
		{
			m_line = XML_GetCurrentLineNumber(pParser->GetParser());
			m_column = XML_GetCurrentColumnNumber(pParser->GetParser());

			LPCTSTR f = pParser->GetFileName();
			m_filename = new TCHAR[_tcslen(f)+1];
			_tcscpy(m_filename, f);

			if(msg != NULL)
				m_msg = msg;
			else
				m_msg = tszXMLParserDefaultException;
		}

	protected:
		LPCTSTR			m_msg;
		int				m_line;
		int				m_column;
		int				m_errcode;
		TCHAR*			m_filename;
};

/* These are definitions for the global scope functions used to
   call operations on an XMLParseState instance. */
void XMLParserStartElement(void *userData, const char *name, const char **atts);
void XMLParserEndElement(void *userData, const char *name);
void XMLParserCharacterData(void *userData, const char *s, int len);

#endif //xmlparser_h__included