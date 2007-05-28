#pragma once

#include "xmlserialize.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <comutil.h>
#include <time.h>

//serialize internal types and simple collections
class serialize_example
	: public ixdev::xml::serialize
{
public:
	bool			m_bool;
	char			m_char;
	short			m_short;
	int				m_int;
	float			m_float;
	double			m_double;
	std::string		m_s;
	std::wstring	m_ws;
	_bstr_t			m_bs;
	std::list< int >				m_list_int;
	std::vector< std::string >		m_vector_string;
	std::map< int, std::wstring >	m_map_int_wstring;
	std::multimap< std::wstring, int >	m_multimap_int_wstring;
	std::set< int >					m_set_int;
	std::multiset< int >			m_multiset_int;
public:
	BEGIN_XML_MAP()
		XML_ITEM_BASE(m_bool, L"theBool")
		XML_ITEM(m_char)
		XML_ITEM(m_short)
		XML_ITEM(m_int)
		XML_ITEM(m_float)
		XML_ITEM(m_double)
		XML_ITEM(m_s)
		XML_ITEM(m_ws)
		XML_ITEM(m_bs)
		XML_ITEM(m_list_int)
		XML_ITEM(m_vector_string)
		XML_ITEM(m_map_int_wstring)
		XML_ITEM(m_multimap_int_wstring)
		XML_ITEM(m_set_int)
		XML_ITEM(m_multiset_int)
	END_XML_MAP()

	void init_default()
	{
		m_bool		= rand() % 2 ? true : false;
		m_char		= rand() % 255;
		m_short		= rand() % 255;
		m_int		= rand() % 255;
		m_float		= float(rand() % 255) / 10;
		m_double	= double(rand() % 255)/ 10.;
		m_s			= (const char*)_bstr_t(_variant_t(rand()%10));
		m_ws		= (const wchar_t*)_bstr_t(_variant_t(rand()%10));
		m_bs		= _bstr_t(_variant_t(rand()%10));
		int sz = rand()%10;
		for(int i = 0; i < sz; i++)
		{
			m_list_int.push_back(i);
			m_vector_string.push_back(std::string("string"));
			m_map_int_wstring[i] = std::wstring(L"wstring");
			m_multimap_int_wstring.insert(make_pair(std::wstring(L"wstring"),i));
			m_set_int.insert(i);
			m_multiset_int.insert(i % 3);
		}
	}

	friend bool operator==(const serialize_example& lhs, const serialize_example& rhs);
	friend bool operator!=(const serialize_example& lhs, const serialize_example& rhs);
};

inline bool operator==(const serialize_example& lhs, const serialize_example& rhs)
{
	bool res = true;
	(lhs.m_bool		== rhs.m_bool)	? res : res = false;
	(lhs.m_char		== rhs.m_char)	? res : res = false;
	(lhs.m_short	== rhs.m_short)	? res : res = false;
	(lhs.m_int		== rhs.m_int)	? res : res = false;
	(lhs.m_float	== rhs.m_float)	? res : res = false;
	(lhs.m_double	== rhs.m_double)? res : res = false;
	(lhs.m_s		== rhs.m_s)		? res : res = false;
	(lhs.m_ws		== rhs.m_ws)	? res : res = false;
	(lhs.m_bs		== rhs.m_bs)	? res : res = false;
	(lhs.m_list_int				== rhs.m_list_int)				? res : res = false;
	(lhs.m_vector_string		== rhs.m_vector_string)			? res : res = false;
	(lhs.m_map_int_wstring		== rhs.m_map_int_wstring)		? res : res = false;
	(lhs.m_multimap_int_wstring	== rhs.m_multimap_int_wstring)	? res : res = false;
	(lhs.m_set_int				== rhs.m_set_int)				? res : res = false;
	(lhs.m_multiset_int			== rhs.m_multiset_int)			? res : res = false;
	return res;
}
inline bool operator!=(const serialize_example& lhs, const serialize_example& rhs)
{
	return !(lhs==rhs);
}
//serialize collections of serializable objects
class serialize_example2
	: public ixdev::xml::serialize
{
public:
	serialize_example m_ser;
	int m_ii;
	std::pair<int, std::string> m_pair;
	std::map<int, std::string> m_map;
	std::vector<serialize_example> m_ser2;
public:
	BEGIN_XML_MAP()
		XML_ITEM(m_ser)
		XML_ITEM(m_ii)
		XML_ITEM(m_pair)
		XML_ITEM(m_map)
		XML_ITEM(m_ser2)		
	END_XML_MAP()

	void init_default()override
	{
		m_ser.init_default();
		m_ii = rand() % 255;
		m_pair = make_pair(rand()%255, std::string(_bstr_t(_variant_t(rand()%255))));
		int sz = rand()%10;
		for(int i = 0; i < sz; i++)
		{
			serialize_example z;
			z.init_default();
			m_ser2.push_back(z);
			m_map[i] = std::string("wstring");
		}
	}

	friend bool operator==(const serialize_example2& lhs, const serialize_example2& rhs);	
	friend bool operator!=(const serialize_example2& lhs, const serialize_example2& rhs);
	
};

inline bool operator==(const serialize_example2& lhs, const serialize_example2& rhs)
{
	bool res = true;
	(lhs.m_ser		== rhs.m_ser)	? res : res = false;
	(lhs.m_ii		== rhs.m_ii)	? res : res = false;
	(lhs.m_pair		== rhs.m_pair)	? res : res = false;
	(lhs.m_ser2		== rhs.m_ser2)	? res : res = false;
	(lhs.m_map		== rhs.m_map)	? res : res = false;
	return res;
}
inline bool operator!=(const serialize_example2& lhs, const serialize_example2& rhs)
{
	return !(lhs==rhs);
}
//serialize simple object, where load and save methods are splited (for easy debug)
class serialize_example3
	: public ixdev::xml::serialize
{
	std::set< int >					m_set_int;
public:	
	HRESULT serialize_obj(ixdev::xml::node& root, bool is_saving)override
	{
		if(is_saving)
			return save_internal(root);
		else
			return load_internal(root);
	}
	HRESULT save_internal(ixdev::xml::node& root)const	
	{												
		using namespace ixdev::xml;					
		HRESULT hr;									
		node theNode;
		//גûחמגû put_item
		hr = put_item(root, _bstr_t(L"m_set_int"), m_set_int);
		_ASSERT(S_OK == hr);

		return hr;
	}
	HRESULT load_internal(const ixdev::xml::node& root)
	{
		using namespace ixdev::xml;
		HRESULT hres = S_OK;
		HRESULT hr;	
		nodesmap child_list = root.get_children_map();
		nodesmap::const_iterator cit;
		//////////////////////////////////////////////////////////////////////////
		cit = child_list.lower_bound(_bstr_t(L"m_set_int"));
		if(cit != child_list.end())
		{
			hr = get_item(cit->second, m_set_int);
			_ASSERT(S_OK == hr);
			if(S_OK!=hr)
			{
				hres = S_FALSE;
			}
		}
		else
		{
			hres = S_FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		return hres;
	}
	void init_default()override
	{
		int sz = rand()%10;
		for(int i = 0; i < sz; i++)
		{
			m_set_int.insert(i);
		}
	}
};