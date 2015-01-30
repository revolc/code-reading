/* Generated by Together */

#ifndef COMUTILS_H
#define COMUTILS_H




#define USE_MEM_STREAM

namespace VCFCOM{


/**
*linker requirements:
*	rpcrt4.lib  - for RPC and UUID stuff
*	shell32.lib	-	for Shell functions
*/
class APPKIT_API COMUtils {

public:    

    static void isEqualIID();

    static void isEqualCLSID();

    static void isEqualGUID();

    static void createGUID();

	static HRESULT createCOMObject( CLSID clsid, IID interfaceID,
		                             void** object );
	static HRESULT createCOMObject( const std::string& progID, IID interfaceID,
		                             void** object );

	static HRESULT BSTRtoString( const BSTR src, std::string& dest );

	static HRESULT StringtoBSTR( const std::string& src, BSTR& dest );
	
	static HRESULT UUIDtoString( const UUID id, std::string& dest );

	static HRESULT StringtoUUID( const std::string& src, UUID& destID );

	static HRESULT makeDefaultFormatETC( const CLIPFORMAT& clipboardFormat, FORMATETC* formatETC );

	static HRESULT getPidlsFromHGlobal(const HGLOBAL HGlob, std::vector<std::string>& fileNames  );	
	
	/**
	This method calls the invokingObject's IDispatch interface and attempts to invoke the
	method named in the methodName argument. This handles all of the OLE nonsense of querying
	for method id's etc. If extreme efficiency is called for, however, it should be noted that 
	this method	does not cache the method id's. It is possible in the future that this might be done
	through some sort of global hashtable, but for now, each invocation of the method causes a complete
	lookup of the method.
	@param IUnknown* invokingObject a reference to an IUnknown that supports the IDIspatch COM
	interface

	@param std::string methodName then name of the method to invoke

	@param DISPPARAMS dispatchParams the dispatch parameters that are passed as arguments to the
	method. The dispatchParams.rgvarg member should be filled with a valid VARIANTARG structure 
	which was initialize with a call to VariantInit().
	For more info see IDispatch::Invoke

	@param VARIANTARG* methodResults a reference to the results of the method. This does not need to
	have been initialized with VariantInit() as the invokeDispatchMethod() method does this 
	automatically.
	*/
	static HRESULT invokeDispatchMethod( IUnknown* invokingObject, const std::string& methodName,
		                                  DISPPARAMS& dispatchParams, VARIANTARG* methodResults );

	/**
	*Translates a framework defined action type to a windows specific COM drag action.
	*/
	static DWORD translateActionType( const VCF::DragActionType& action );

	static UINT translateFrameworkFormat( const unsigned long& dataType );

	static unsigned long translateWin32ClipboardFormat( const UINT& clipboardFormat );
};

}
#endif //COMUTILS_H