/**
 * @file    CBHOIm.cpp
 * @author  Zhan WANG <wangzhan@rd.netease.com>
 */
// CBHOImp.cpp : Implementation of CCBHOImp

#include "stdafx.h"
#include "CBHOImp.h"


// CCBHOImp

STDMETHODIMP CCBHOImp::SetSite(IUnknown*pUnkSite)
{
    if(pUnkSite!=NULL)
    {
        HRESULT hr;
        CComPtr<IServiceProvider> sp;

        hr = pUnkSite->QueryInterface(&sp);
        if(SUCCEEDED(hr) && sp)
        {
            //����ָ��IWebBrowser2��ָ��
            hr = sp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&m_spWebBrowser);

            if(SUCCEEDED(hr)&&m_spWebBrowser!=0)
            {
                //ע��DWebBrowserEvents2�¼���
                hr = DispEventAdvise(m_spWebBrowser);
                if(SUCCEEDED(hr))
                {
                    m_fAdvised=TRUE;
                }
            }
        }
        m_spUnkSite = pUnkSite;
    }
    else
    {
        //ȡ��ע���¼���
        if(m_fAdvised)
        {
            DispEventUnadvise(m_spWebBrowser);
            m_fAdvised=FALSE;
        }
        //�ڴ��ͷŻ����ָ���������Դ��
        m_spWebBrowser.Release();
        //m_spTarget.Release();
    }
    //���û���ʵ�֡�
    return IObjectWithSiteImpl<CCBHOImp>::SetSite(pUnkSite);
}

void STDMETHODCALLTYPE CCBHOImp::OnDocumentComplete(IDispatch*pDisp,VARIANT*pvarURL)  
{  
    HRESULT hr = S_OK;   
    // ��ѯ IWebBrowser2 �ӿڡ�   
    CComQIPtr<IWebBrowser2> spTempWebBrowser = pDisp;   
    // ���¼��Ƿ��붥������������?   
    if (spTempWebBrowser && m_spWebBrowser && m_spWebBrowser.IsEqualObject(spTempWebBrowser))   
    {   
        // ��������л�ȡ��ǰ�ĵ����󡭡�   
        CComPtr<IDispatch>  spDispDoc;   
        hr = m_spWebBrowser->get_Document(&spDispDoc);   
        if (SUCCEEDED(hr))   
        {   
            // ��������ѯ HTML �ĵ���   
            CComQIPtr<IHTMLDocument2> spHTMLDoc = spDispDoc;   
            if (spHTMLDoc != NULL) {   
                // ���ɾ����Щͼ��   
                RemoveImages(spHTMLDoc);   
            }   
        }   
    }   
}  

void CCBHOImp::RemoveImages(IHTMLDocument2* pDocument)   
{   
    CComPtr<IHTMLElementCollection> spImages;   
    // �� DOM �л�ȡͼ�񼯡�   
    HRESULT hr = pDocument->get_images(&spImages);   
    if (hr == S_OK && spImages != NULL) {   
        // ��ȡ�����е�ͼ������   
        long cImages = 0;   
        hr = spImages->get_length(&cImages);   
        if (hr == S_OK && cImages > 0)   
        {   
            for (int i = 0; i < cImages; i++)   
            {   
                CComVariant svarItemIndex(i);   
                CComVariant svarEmpty;   
                CComPtr<IDispatch> spdispImage;   
                // �������Ӽ����л�ȡͼ��   
                hr = spImages->item(svarItemIndex, svarEmpty, &spdispImage);   
                if (hr == S_OK && spdispImage != NULL)   
                {   
                    // ���ȣ���ѯͨ�� HTML Ԫ�ؽӿڡ���   
                    CComQIPtr<IHTMLElement> spElement = spdispImage;   
                    if (spElement)   
                    {   
                        // ����Ȼ��������ʽ�ӿڡ�   
                        CComPtr<IHTMLStyle> spStyle;   
                        hr = spElement->get_style(&spStyle);   
                        // ���� display="none" ������ͼ��   
                        if (hr == S_OK && spStyle != NULL)   
                        {   
                            static const CComBSTR sbstrNone(L"none");   
                            spStyle->put_display(sbstrNone);  
                        }  
                    }   
                }   
            }   
        }   
    }   
} 

STDMETHODIMP CCBHOImp::QueryStatus(const GUID* pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT* pCmdText)  
{  
    if (cCmds == 0) return E_INVALIDARG;  
    if (prgCmds == 0) return E_POINTER;  
    prgCmds[0].cmdf = OLECMDF_ENABLED;  

    return S_OK;  
}  

STDMETHODIMP CCBHOImp::Exec(const GUID*, DWORD nCmdID, DWORD, VARIANTARG*, VARIANTARG*)
{
    if(m_spUnkSite == 0 || m_spWebBrowser == 0) return S_OK;
    HRESULT hRes = S_OK;

    CComPtr<IDispatch>        pDocDisp;
    CComQIPtr<IHTMLDocument2> pHtmlDoc2;
    hRes = m_spWebBrowser->get_Document(&pDocDisp);

    if(SUCCEEDED(hRes) && pDocDisp)
    {
        hRes = pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc2);
        if(SUCCEEDED(hRes) && pHtmlDoc2)
        {
            SHANDLE_PTR nBrowser = 0;
            m_spWebBrowser->get_HWND(&nBrowser);
            HWND hWndParent = (HWND)nBrowser;

            HINSTANCE hInstance = _AtlBaseModule.GetModuleInstance();

            MessageBox(NULL, _T("Hello world!"), _T("Hello"), MB_OK|MB_ICONEXCLAMATION);
        }
    }
    return S_OK;
}
