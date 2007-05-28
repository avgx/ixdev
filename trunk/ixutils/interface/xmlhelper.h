#ifndef __XML_HELPER__H__
#define __XML_HELPER__H__

#pragma once

#include <vector>
#include <map>
#include <string>

#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>
#include <msxml2.h>
#pragma comment(lib, "comsuppw.lib")

namespace ixdev
{

namespace xml
{

#define IXDEV_XML_VERSION L"1.2"

class attribute;
class node;
class document;

typedef std::vector<attribute> attributes;
typedef std::map<_bstr_t, attribute> attributesmap;
typedef std::vector<node> nodes;
typedef std::multimap<_bstr_t, node> nodesmap;

class attribute
{
	_bstr_t _name;
	_bstr_t _value;
public:
	attribute()
	{
	}

	//имя
	_bstr_t name()const
	{
		return _name;
	}
	//значение
	_bstr_t value()const
	{
		return _value;
	}

	//набор ...
	template<typename T>
	attribute(BSTR theName, T theValue)
		: _name(theName), _value(_variant_t(theValue))
	{
	}
#pragma warning(disable : 4267)
//как-то плохо преобразуется size_t в вариант
	attribute(BSTR theName, size_t theValue)
		: _name(theName), _value(_variant_t(theValue))
	{
	}
#pragma warning(default : 4267)
	attribute(BSTR theName, BSTR theValue)
		: _name(theName), _value(theValue)
	{
	}	
	attribute(BSTR theName, bool theValue)
		: _name(theName), _value(_variant_t(int(theValue)))
	{
	}
	attribute(BSTR theName, const std::string& theValue)
		: _name(theName), _value(_bstr_t(theValue.c_str()))
	{
	}
	attribute(BSTR theName, const std::wstring& theValue)
		: _name(theName), _value(_bstr_t(theValue.c_str()))
	{
	}

	//набор перегруженных функций, для однообразного получения значения
	template<typename T>
	void value(T& v)const
	{
		v = _variant_t(_value);
	}
	void value(bool& v)const
	{
		v = int(_variant_t(_value))?true:false;
	}
	void value(std::string& v)const
	{
		v = _value;
	}
	void value(std::wstring& v)const
	{
		v = _value;
	}
	void value(_bstr_t& v)const
	{
		v = _value;
	}
};

class node
{
	CComPtr<IXMLDOMNode> _node;
public:
	//конструктор по умолчанию - пустая, не готовая к использованию ветка
	node()
	{
	}
	//конструктор для присвоения этому узлу элемента msxml
	//(для чтения его свойств и дочерних узлов) (или установки корня дерева)
	node(IXMLDOMNode* theNode)
	{
		_ASSERT(theNode);
		if(theNode)
			_node = theNode;
	}

	//конструктор копирования (для использования в контейнерах stl)
	node(const node& r)
	{
		_node = r._node;
	}
	node& operator=(const node& r)
	{
		_node = r._node;
		return (*this);
	}
public:
	bool is_valid()const
	{
		return (NULL != _node);
	}
	void clear()
	{
		_node.Release();
	}
	//создаем новый узел в документе дочерний для этого узла
	node create_child(BSTR name)
	{	
		HRESULT hr;
		CComPtr<IXMLDOMDocument> pDoc;
		hr = _node->get_ownerDocument(&pDoc);
		_ASSERT(S_OK==hr);
		_ASSERT(pDoc);
		if(S_OK!=hr)
		{
			return node(0);
		}

		CComPtr<IXMLDOMElement> child;
		hr = pDoc->createElement(_bstr_t(name), &child);
		hr = _node->appendChild(child, NULL);
		_ASSERT(S_OK==hr);
		CComQIPtr<IXMLDOMNode> pChildNode(child);
		_ASSERT(pChildNode);
		return node(pChildNode);
	}
	
	
	//получаем все аттрибуты этого узла
	attributes get_attributes()const
	{
		std::vector<attribute> result;
		CComPtr<IXMLDOMNamedNodeMap> attrList;
		HRESULT hr = E_FAIL;
		hr = _node->get_attributes( &attrList );
		_ASSERT(S_OK==hr);
		_ASSERT(attrList);
		if(NULL == attrList)
		{
			return result;
		}

		long size = 0;
		hr = attrList->get_length(&size);
		_ASSERT(S_OK==hr);
		for (long i = 0; i < size; i++) 
		{
			CComPtr<IXMLDOMNode> attr;
			hr = attrList->get_item(i, &attr);
			_ASSERT(S_OK==hr);
			_ASSERT(attr);
			if(S_OK!=hr)
				continue;
			DOMNodeType dt;
			hr = attr->get_nodeType(&dt);
			_ASSERT(S_OK==hr);
			if(S_OK!=hr)
				continue;
			_ASSERT(NODE_ATTRIBUTE==dt);
			if(NODE_ATTRIBUTE!=dt)
				continue;
			_bstr_t name;
			_variant_t val;
			hr = attr->get_nodeName(name.GetAddress());
			_ASSERT(S_OK==hr);
			if(S_OK!=hr)
				continue;
			hr = attr->get_nodeTypedValue(&val);
			_ASSERT(S_OK==hr);
			if(S_OK!=hr)
				continue;
			result.push_back(attribute(name, _bstr_t(val)));
		}
		return result;
	}

	attributesmap get_attributes_map()const
	{
		attributesmap result;
		std::vector<attribute> item_list = get_attributes();
		for(size_t j = 0; j < item_list.size(); j++)
		{
			result[item_list[j].name()] = item_list[j];
		}
		return result;
	}
	//получаем все дочерние узлы этого узла
	nodes get_children()const
	{
		std::vector<node> result;
		HRESULT hr = E_FAIL;
		_ASSERT(_node);
		if(NULL == _node)
		{
			return result;
		}

		CComPtr<IXMLDOMNodeList> childList;			
		long size;

		hr = _node->get_childNodes(&childList);
		_ASSERT(S_OK==hr);
		_ASSERT(childList);
		if(NULL == childList)
		{
			return result;
		}

		hr = childList->get_length(&size);
		_ASSERT(S_OK==hr);
		for (long i = 0; i < size; i++) 
		{
			CComPtr<IXMLDOMNode> child;
			hr = childList->get_item(i, &child);
			_ASSERT(S_OK==hr);
			_ASSERT(child);
			if(child)
			{
				//debug only!!
				//_bstr_t nodeName;
				//child->get_nodeName(nodeName.GetAddress());
				result.push_back( node(child) );
			}
			child.Release();
		}

		return result;
	}
	nodesmap get_children_map()const
	{
		nodesmap result;
		nodes item_list = get_children();
		for(size_t j = 0; j < item_list.size(); j++)
		{
			result.insert( nodesmap::value_type(item_list[j].get_name(), item_list[j]) );
		}
		return result;
	}
	//получаем текст узла
	_bstr_t get_text()const
	{
		_bstr_t result;
		_ASSERT(_node);
		if(!_node)
			return result;

		_variant_t v;
		HRESULT hr = E_FAIL;
		hr = _node->get_nodeTypedValue(&v);
		_ASSERT(hr);
		if(S_OK==hr)
			result = _bstr_t(v);
		return result;
	}
	//получаем имя узла (тэг)
	_bstr_t get_name()const
	{
		_bstr_t result;
		_ASSERT(_node);
		_bstr_t s;
		HRESULT hr = E_FAIL;
		if(NULL !=_node)
		{
			hr = _node->get_nodeName(result.GetAddress());
		}
		_ASSERT(S_OK==hr);
		return result;
	}
public:
	//устанавливаем текст узла
	HRESULT set_text(BSTR s)
	{
		HRESULT hr = E_FAIL;
		if(!s)
			return E_POINTER;
		_ASSERT(_node);
		if(!_node)
			return hr;
		
		hr = _node->put_text(_bstr_t(s));
		return hr;
	}
	//добавляем аттрибут к этому узлу
	HRESULT add_attribute(const attribute& attr)
	{
		HRESULT hr = E_FAIL;
		_ASSERT(_node);
		if(!_node)
			return hr;
		CComQIPtr<IXMLDOMElement> pElement(_node);
		_ASSERT(pElement);
		if(pElement)
			hr = pElement->setAttribute(_bstr_t(attr.name()), _variant_t(attr.value()));
		return hr;
	}
};

class document
{
	CComPtr<IXMLDOMDocument2> _doc;
	CComPtr<IXMLDOMNode> _root;

	document(const document&);
	document& operator=(const document&);
public:
	document()
	{
		::CoInitialize(NULL);
		clear();
	}
	~document()
	{
		_doc.Release();
		_root.Release();
		::CoUninitialize();
	}
	void clear()
	{
		HRESULT hr = Create();
		_ASSERT(S_OK==hr);
	}

	node get_root()const
	{
		return node(_root);
	}
	HRESULT create_root(BSTR name)
	{
		HRESULT hr = E_FAIL;
		hr = Create();
		if(FAILED(hr))
		{
			return hr;
		}

		hr = AddProcessingInstruction();
		_ASSERT(S_OK==hr);

		CComPtr<IXMLDOMElement> pRootElement;
		hr = _doc->createElement(_bstr_t(name), &pRootElement);
		_ASSERT(S_OK==hr);
		if(!pRootElement)
			return hr;
		hr = _doc->putref_documentElement(pRootElement);
		_ASSERT(S_OK==hr);
		if(!pRootElement)
			return hr;
		hr = pRootElement->QueryInterface(&_root);
		_ASSERT(S_OK==hr);
		_ASSERT(_root);
		return hr;
	}

	HRESULT from_string(BSTR s)
	{
		HRESULT hr = E_FAIL;
		hr = Create();
		if(FAILED(hr))
			return hr;

		VARIANT_BOOL res;
		hr = _doc->loadXML(_bstr_t(s), &res);

		if (SUCCEEDED(hr) && res != VARIANT_FALSE) 
		{
			hr = AddProcessingInstruction();
			_ASSERT(S_OK==hr);

			hr = SetRootElementFromDoc();
		}

		return hr;
	}

	HRESULT to_string(BSTR* s)const
	{
		_ASSERT(_doc);
		HRESULT hr = E_FAIL;
		if(_doc)
		{
			hr = _doc->get_xml(s);
		}
		return hr;
	}

	HRESULT from_file(BSTR filename)
	{
		HRESULT hr = E_FAIL;
		hr = Create();
		if(FAILED(hr))
			return hr;

		_variant_t v(filename);
		VARIANT_BOOL res;
		hr = _doc->load(v, &res);

		if (SUCCEEDED(hr) && res != VARIANT_FALSE) 
		{
			hr = SetRootElementFromDoc();
		}

		return hr;
	}

	HRESULT to_file(BSTR filename)const
	{
		_ASSERT(_doc);
		HRESULT hr = E_FAIL;
		if(_doc)
		{
			hr = _doc->save(_variant_t(_bstr_t(filename)));
		}
		return hr;
	}
private:
	HRESULT Create()
	{
		HRESULT hr = E_FAIL;
		_doc.Release();
		_root.Release();
		hr = _doc.CoCreateInstance(__uuidof(DOMDocument));
		_ASSERT(SUCCEEDED(hr));
		_ASSERT(_doc);
		return hr;
	}

	HRESULT SetRootElementFromDoc()
	{
		HRESULT hr = E_FAIL;
		_ASSERT(_doc);
		CComPtr<IXMLDOMElement> pRootElement;
		hr = _doc->get_documentElement(&pRootElement);
		_ASSERT(S_OK==hr);
		_ASSERT(pRootElement);
		if(pRootElement)
		{
			hr = pRootElement->QueryInterface(&_root);
			_ASSERT(S_OK==hr);
			_ASSERT(_root);
		}
		return hr;
	}
	HRESULT AddProcessingInstruction()
	{
		HRESULT hr = E_FAIL;
		_bstr_t data;
		_bstr_t target;
		bool need_insert_new_pi = true;

		//создаем новую ProcessingInstruction
		CComPtr<IXMLDOMProcessingInstruction> pi;
		target = _T("xml");
		data = _T(" version='1.0' encoding='UTF-8'");
		hr = _doc->createProcessingInstruction(target, data, &pi);
		_ASSERT(S_OK==hr);
		_ASSERT(pi);

		//ищем старую
		CComPtr<IXMLDOMNode> instr;
		hr = _doc->get_firstChild( &instr );
		if(S_OK==hr)
		{
			_ASSERT(NULL != instr);
			DOMNodeType tp;
			hr = instr->get_nodeType( &tp );
			_ASSERT(S_OK==hr);

			if ( tp == NODE_PROCESSING_INSTRUCTION )
			{
				//Обнаружили остатки ProcessingInstruction
				CComQIPtr<IXMLDOMProcessingInstruction> pi_old(instr);
				_ASSERT(NULL != pi_old);

				if ( NULL != pi_old )
				{
					CComPtr<IXMLDOMNode> oldInstr;
					hr = _doc->replaceChild( pi, pi_old, &oldInstr );
					need_insert_new_pi = false;
				}	
			}
		}

		if(need_insert_new_pi)
		{
			//надо втыкнуть первой
			hr = _doc->appendChild(pi, NULL);

			CComPtr<IXMLDOMComment> comment;
			_bstr_t comment_text = _bstr_t(L"created with ixdev.xml version='") + _bstr_t(IXDEV_XML_VERSION)+ _bstr_t(L"'");
			hr = _doc->createComment(comment_text, &comment);
			_ASSERT(S_OK==hr);
			if(comment)
			{
				_doc->appendChild(comment, NULL);
			}
		}		
				
		return hr;
	}

	
};

}//namespace xml
//////////////////////////////////////////////////////////////////////////

}//namespace ixdev

#endif//__XML_HELPER__H__