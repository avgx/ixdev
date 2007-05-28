#pragma once

#include "xmlhelper.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <comutil.h>

//////////////////////////////////////////////////////////////////////////
//	Usage example:
//		class serialize_example :
//			public ixdev::xml::serialize		//	<- Derive
//		{
//		public:
//			int m_i;
//			std::string m_s;
//			std::wstring m_ws;
//			std::list< int > m_v_i;
//			_bstr_t m_bs;
//		public:
//			BEGIN_XML_MAP()						//	<- Declare map with elements you want to serialize
//				XML_ITEM(m_i)
//				XML_ITEM(m_s)
//				XML_ITEM(m_ws)
//				XML_ITEM(m_v_i)
//				XML_ITEM(m_bs)
//			END_XML_MAP()
//		
//			void init_default()					//	<- If you want I can override firtual function to initialize before load
//			{
//				m_i = 0;
//				m_s = "";
//				m_ws = L"";
//				m_v_i.clear();
//				m_bs = "";
//			}
//		};
//////////////////////////////////////////////////////////////////////////
namespace ixdev
{

namespace xml
{
	class serialize
	{
	public:
		//кладем в xml узел name с текстом value и необязательным аттрибутом item_id
		//internal types
		HRESULT put_item(xml::node& parent,	BSTR name, BSTR value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, char value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, short value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, __int64 value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, bool value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, size_t value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, int value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, float value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, double value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, const std::string& value)const
		{
			return put_item(parent, attribute(name, value));
		}
		HRESULT put_item(xml::node& parent,	BSTR name, const std::wstring& value)const
		{
			return put_item(parent, attribute(name, value));
		}

		//ixdev::xml::serialize
		HRESULT put_item(xml::node& parent, BSTR name, const ixdev::xml::serialize& value)const
		{
			HRESULT hr;
			node theNode = parent.create_child(name);
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}
			hr = value.save_obj(theNode);
			_ASSERT(S_OK==hr);
			return hr;
		}
		//в конечном итоге все перегруженные методы вызывают этот метод (здесь формируем xml)
		HRESULT put_item(xml::node& parent, const attribute& attr)const
		{
			HRESULT hr;
			node theNode = parent.create_child(attr.name());						
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}																
			hr = theNode.add_attribute(attribute(_bstr_t(L"value"), attr.value()));
			_ASSERT(S_OK==hr);
			return hr;
		}

		//std::vector, std::list
		template<class T, template<typename T, typename = std::allocator<T> > class TContainer >
		HRESULT put_item(xml::node& parent, BSTR name, const TContainer<T>& value)const
		{
			HRESULT hr = E_FAIL;
			node theNode = parent.create_child(name);
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}
			if(value.empty())
				return S_OK;
			typename TContainer<T>::const_iterator cit;
			size_t i = 0;
			for(cit = value.begin(); cit != value.end(); ++cit, ++i)
			{
				hr = put_item(theNode, _bstr_t("item"), (*cit));
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					break;
				}
			}
			return hr;
		}

		//std::set
		template<class T, template<typename T, typename = std::less<T>, typename = std::allocator<T> > class TContainer >
		HRESULT put_item(xml::node& parent, BSTR name, const TContainer<T>& value)const
		{
			HRESULT hr = E_FAIL;
			node theNode = parent.create_child(name);
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}
			if(value.empty())
				return S_OK;
			typename TContainer<T>::const_iterator cit;
			size_t i = 0;
			for(cit = value.begin(); cit != value.end(); ++cit, ++i)
			{
				hr = put_item(theNode, _bstr_t("item"), (*cit));
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					break;
				}
			}
			return hr;
		}
		//std::pair
		template<class T, class U>
		HRESULT put_item(xml::node& parent, BSTR name, const std::pair<T,U>& value)const
		{
			HRESULT hr;
			node theNode = parent.create_child(name);
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}
			
			hr = put_item(theNode, _bstr_t("first"), value.first);
			_ASSERT(S_OK==hr);
			hr = put_item(theNode, _bstr_t("second"), value.second);
			_ASSERT(S_OK==hr);

			return hr;
		}
		//std::map, std::multimap
		template<class T, class U, template<typename T, typename U, typename = std::less<T>, typename = std::allocator< std::pair<const T,U> > > class TContainer >
		HRESULT put_item(xml::node& parent, BSTR name, const TContainer<T, U>& value)const
		{
			HRESULT hr = E_FAIL;
			node theNode = parent.create_child(name);
			_ASSERT(theNode.is_valid());									
			if(!theNode.is_valid())											
			{																
				return E_FAIL;												
			}

			if(value.empty())
				return S_OK;
			typename TContainer<T, U>::const_iterator cit;
			size_t i = 0;
			for(cit = value.begin(); cit != value.end(); ++cit, ++i)
			{
				hr = put_item(theNode, _bstr_t("item"), (*cit));
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					break;
				}
			}
			return hr;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		//internal types
		HRESULT get_item(const xml::node& item, bool& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, char& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, short& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, int& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, float& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, double& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, std::string& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, std::wstring& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		HRESULT get_item(const xml::node& item, _bstr_t& value)
		{
			HRESULT hr = E_FAIL;
			attribute attr;
			hr = get_item(item, attr);
			if(S_OK==hr)
			{
				attr.value(value);
			}
			return hr;
		}
		//получаем из узла значение текста его аттрибута (в конечном итоге)
		HRESULT get_item(const xml::node& item, attribute& attr)
		{
			attributesmap attrs = item.get_attributes_map();
			attributesmap::const_iterator cit = attrs.find(_bstr_t(L"value"));
			if(cit != attrs.end())
			{
				attr = cit->second;
				return S_OK;
			}
			return S_FALSE;
		}
		//std::vector, std::list
		template<class T, template<typename T, typename = std::allocator<T> > class TContainer >
		HRESULT get_item(const xml::node& parent, TContainer<T>& value)
		{
			HRESULT hr = S_OK;
			_ASSERT(parent.is_valid());									
			if(!parent.is_valid())											
			{																
				return E_FAIL;												
			}

			TContainer<T> value_temp;
			size_t i = 0;
			nodesmap child_list = parent.get_children_map();
			nodesmap::const_iterator cit;
			for(cit = child_list.lower_bound(_bstr_t(L"item")); cit != child_list.upper_bound(_bstr_t(L"item")); ++cit, ++i)
			{
				T temp;
				hr = get_item(cit->second, temp);
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					//error loading -> don't swap
					break;
				}
				value_temp.push_back(temp);
			}

			//swap containers (the new config may consist of zero items)
			if(S_OK==hr)
				swap(value, value_temp);
						
			return hr;
		}
		//std::set
		template<class T, template<typename T, typename = std::less<T>, typename = std::allocator<T> > class TContainer >
		HRESULT get_item(const xml::node& parent, TContainer<T>& value)
		{
			HRESULT hr = S_OK;
			_ASSERT(parent.is_valid());									
			if(!parent.is_valid())											
			{																
				return E_FAIL;												
			}

			TContainer<T> value_temp;
			size_t i = 0;
			nodesmap child_list = parent.get_children_map();
			nodesmap::const_iterator cit;
			for(cit = child_list.lower_bound(_bstr_t(L"item")); cit != child_list.upper_bound(_bstr_t(L"item")); ++cit, ++i)
			{
				T temp;
				hr = get_item(cit->second, temp);
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					//error loading -> don't swap
					break;
				}
				value_temp.insert(temp);
			}

			//swap containers (the new config may consist of zero items)
			if(S_OK==hr)
				swap(value, value_temp);

			return hr;
		}
		//std::pair
		template<class T, class U>
		HRESULT get_item(const xml::node& parent, std::pair<T,U>& value)
		{
			HRESULT hr = S_OK;
			_ASSERT(parent.is_valid());									
			if(!parent.is_valid())											
			{																
				return E_FAIL;												
			}

			std::pair<T,U> value_temp;
			nodesmap child_list = parent.get_children_map();
			nodesmap::const_iterator cit_first  = child_list.lower_bound(_bstr_t(L"first"));
			nodesmap::const_iterator cit_second = child_list.lower_bound(_bstr_t(L"second"));
			if(cit_first == child_list.end() || cit_second == child_list.end())
				return S_FALSE;
			hr = get_item(cit_first->second, value_temp.first);
			_ASSERT(S_OK==hr);
			if(S_OK != hr)
				return S_FALSE;
			hr = get_item(cit_second->second, value_temp.second);
			_ASSERT(S_OK==hr);
			if(S_OK != hr)
				return S_FALSE;

			swap(value, value_temp);
			
			return hr;
		}

		//std::map, std::multimap
		template<class T, class U, template<typename T, typename U, typename = std::less<T>, typename = std::allocator< std::pair<const T,U> > > class TContainer >
		HRESULT get_item(const xml::node& parent, TContainer<T, U>& value)
		{
			HRESULT hr = S_OK;
			_ASSERT(parent.is_valid());									
			if(!parent.is_valid())											
			{																
				return E_FAIL;												
			}

			TContainer<T,U> value_temp;
			size_t i = 0;
			nodesmap child_list = parent.get_children_map();
			nodesmap::const_iterator cit;
			for(cit = child_list.lower_bound(_bstr_t(L"item")); cit != child_list.upper_bound(_bstr_t(L"item")); ++cit, ++i)
			{
				std::pair<T,U> temp;
				hr = get_item(cit->second, temp);
				_ASSERT(S_OK==hr);
				if(S_OK!=hr)
				{
					break;
				}
				value_temp.insert(temp);
			}

			if(S_OK==hr)
				swap(value, value_temp);
			
			return hr;
		}


		//ixdev::xml::serialize
		HRESULT get_item(const xml::node& item, ixdev::xml::serialize& value)
		{
			HRESULT hr = E_FAIL;
			_ASSERT(item.is_valid());									
			if(!item.is_valid())											
			{																
				return E_FAIL;												
			}
			hr = value.load_obj(item);
			_ASSERT(S_OK==hr);			
			return hr;
		}

		//сохраняем содержимое в xml файл
		HRESULT save(BSTR filename, BSTR root_tag_name = _bstr_t(L"root"))const
		{
			HRESULT hr;									
			ixdev::xml::document doc;					
			ixdev::xml::node node1;						
			hr = doc.create_root(root_tag_name);
			_ASSERT(S_OK==hr);							
			ixdev::xml::node root(doc.get_root());		
			_ASSERT(root.is_valid());					
			_ASSERT(S_OK==hr);							
			
			hr = save_obj(root);//splitted save/load
			//hr = serialize_obj(root, true);
			_ASSERT(S_OK==hr);							
			
			hr = doc.to_file(filename);					
			_ASSERT(S_OK==hr);							
			return hr;									
		}
		//сохраняем содержимое в xml строку
		HRESULT save_xml(BSTR* xmlstring, BSTR root_tag_name = _bstr_t(L"root"))const
		{
			HRESULT hr;									
			ixdev::xml::document doc;					
			ixdev::xml::node node1;						
			hr = doc.create_root(root_tag_name);
			_ASSERT(S_OK==hr);							
			ixdev::xml::node root(doc.get_root());		
			_ASSERT(root.is_valid());					
			_ASSERT(S_OK==hr);							

			hr = save_obj(root);//splitted save/load
			//hr = serialize_obj(root, true);						
			_ASSERT(S_OK==hr);							

			hr = doc.to_string(xmlstring);					
			_ASSERT(S_OK==hr);							
			return hr;									
		}
		//загружаем содержимое из xml файла
		HRESULT load(BSTR filename)
		{
			using namespace ixdev::xml;		
			init_default();					
			HRESULT hr;						
			document doc;					
			hr = doc.from_file(filename);	
			_ASSERT(S_OK==hr);				
			if(S_OK!=hr)					
			{								
				return hr;					
			}								
			node root(doc.get_root());		
			_ASSERT(root.is_valid());		
			if(!root.is_valid())			
			{							
				return E_FAIL;				
			}								
			hr = load_obj(root);//splitted save/load
			//hr = serialize_obj(root, false);	
			_ASSERT(S_OK==hr);
			return hr;						
		}
		//загружаем содержимое из xml строки
		HRESULT load_xml(BSTR xmlstring)
		{
			using namespace ixdev::xml;		
			init_default();					
			HRESULT hr;						
			document doc;					
			hr = doc.from_string(xmlstring);	
			_ASSERT(S_OK==hr);				
			if(S_OK!=hr)					
			{								
				return hr;					
			}								
			node root(doc.get_root());		
			_ASSERT(root.is_valid());		
			if(!root.is_valid())			
			{							
				return E_FAIL;				
			}								
			hr = load_obj(root);//splitted save/load
			//hr = serialize_obj(root, false);	
			_ASSERT(S_OK==hr);
			return hr;						
		}
		
		//сохранение объекта
		HRESULT save_obj(ixdev::xml::node& root)const
		{
			return (const_cast<serialize*>(this))->serialize_obj(root, true);	//dirty, but I promise don't do anything with object state on save
		}
		//загрузка объекта
		HRESULT load_obj(const ixdev::xml::node& root)
		{
			return serialize_obj(const_cast<node&>(root), false);	//dirty, but I don't need to do anything with root node state on load
		}

		//и загрузка и сохрание (для формирования макроса)
		virtual HRESULT serialize_obj(ixdev::xml::node& root, bool is_saving)=0;
		//загрузка значений элементов объекта по умолчанию (будет вызвана при загрузке)
		virtual void init_default(){ }
	};

#define BEGIN_XML_MAP()		HRESULT serialize_obj(ixdev::xml::node& root, bool is_saving)			\
							{																		\
								using namespace ixdev::xml;											\
								HRESULT hr;															\
								node theNode;														\
																									\
								nodesmap child_list;												\
								nodesmap::const_iterator cit;										\
								HRESULT hres = S_OK;												\
								if(!is_saving)														\
								{																	\
									child_list = root.get_children_map();							\
								}

#define END_XML_MAP()			return hres;														\
							}

#define XML_ITEM(x) XML_ITEM_BASE(x, L#x)

#define XML_ITEM_BASE(x, y)	if(is_saving)										\
							{													\
								hres = put_item(root, _bstr_t(y), (x));			\
								_ASSERT(S_OK == hres);							\
								if(S_OK != hres)								\
									return hres;								\
							}													\
							else												\
							{													\
								cit = child_list.lower_bound(_bstr_t(y));		\
								if(cit != child_list.end())						\
								{												\
									hr = get_item(cit->second, (x));			\
									_ASSERT(S_OK == hr);						\
									if(S_OK!=hr)								\
									{											\
										hres = S_FALSE;							\
									}											\
								}												\
								else											\
								{												\
									hres = S_FALSE;								\
								}												\
							}
}//namespace xml
}//namespace ixdev