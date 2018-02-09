
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincodec.h>

#include "ldmicro.h"
#include "componentlist.h"
#include "components/componentimages.h"

static int temp = 1;

void InitializeComponentImage(int Component, HIMAGELIST *il)
{
    int i;
    // char a[4];
    // MessageBox(ComponentList, (LPCTSTR)itoa(rgCompData[Component], a, 10),
        // "Info", MB_OK | MB_ICONEXCLAMATION);
    // test=LoadComponentImage(rgCompData[Component].Images[0]);
    //     if(test ==NULL){
    //         MessageBox(ComponentList, "Could not load Bitmap!",
         // "Error", MB_OK | MB_ICONEXCLAMATION);
    //     }
    // HDC hdc=GetDC(AdvancedWorkspace);
    // HdcMem = CreateCompatibleDC(hdc);
    // HBITMAP bm = (HBITMAP)SelectObject(HdcMem, test);
    // BitBlt(hdc, 10 * temp, 10 * temp, COMPONENT_WIDTH,
        // COMPONENT_HEIGHT, HdcMem, 0, 0, SRCCOPY);
    // temp++;
    // DeleteDC(hdc);
    // if(ComponentDiagrams == NULL){
    //     InitializeImageList();
    // }
    HBITMAP bm = LoadComponentImage(Component);
    // HBITMAP mask = CreateBitmapMask(bm, CLR_NONE);
    // HDC hdc = GetDC(AdvancedWorkspace);
    ImageList_RemoveAll(*il);
    i = ImageList_AddMasked(*il, bm, CLR_NONE);
    if(bm==NULL)
    {
       MessageBox(NULL,"Could not add image",
        "Info", MB_OK | MB_ICONEXCLAMATION); 
    }
    /*char vx[4],vy[4];
            _itoa(10,vy,10);
            _itoa(ImageStack[i].Image->Images,vx,10);
            MessageBox(NULL,
               (vx), (vy), MB_OK | MB_ICONWARNING);*/
    // _itoa(Component, a, 10);
    // MessageBox(ComponentList,a,
    //     "Info", MB_OK | MB_ICONEXCLAMATION);
    
    // ImageList_Draw(ComponentDiagrams, i, hdc, 100*temp, 100*temp, ILD_TRANSPARENT);
    // temp ++;
    // DeleteDC(hdc);
}

//-----------------------------------------------------------------------------
// Creates a stream object initialized with the data from an executable resource.
//-----------------------------------------------------------------------------

IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
 
    // initialize return value
    IStream * ipStream = NULL;

    // find the resource
    HRSRC hrsrc = FindResource(NULL, lpName, lpType);
    if (hrsrc == NULL)
    {
        return ipStream;
    }
 
    // load the resource
    DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
    HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
    if (hglbImage == NULL)
    {
        return ipStream;
    }

    // lock the resource, getting a pointer to its data
    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL)
    {
        return ipStream;
    }

    // allocate memory to hold the resource data
    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL)
    {
        return ipStream;
    }

    // get a pointer to the allocated memory
    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == NULL)
    {
        // couldn't create stream; free the memory
        GlobalFree(hgblResourceData);
        return ipStream;
    }
 
    // copy the data from the resource to the new memory block
    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
    GlobalUnlock(hgblResourceData);

    // create a stream on the HGLOBAL containing the data
    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
    {
        return ipStream;
    }
    GlobalFree(hgblResourceData);
    return ipStream;
}

//-----------------------------------------------------------------------------
// Loads a PNG image from the specified stream
// (using Windows Imaging Component)
//-----------------------------------------------------------------------------
IWICBitmapSource * LoadBitmapFromStream(IStream * ipImageStream)
{
    // initialize return value
    IWICBitmapSource * ipBitmap = NULL;

    // load WIC's PNG decoder
    IWICBitmapDecoder * ipDecoder = NULL;
    if(CoInitialize(NULL) == RPC_E_CHANGED_MODE)
    {
        MessageBox(ComponentList, "Could not Initialize COM Library!",
            "Error", MB_OK | MB_ICONEXCLAMATION);
    }
    if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER,
        __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
    {
        MessageBox(ComponentList, "Could not Create Object!",
            "Error", MB_OK | MB_ICONEXCLAMATION);
        goto Return;
    }

    // load the PNG
    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
        goto ReleaseDecoder;

    // check for the presence of the first frame in the bitmap
    UINT nFrameCount = 0;
    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
        goto ReleaseDecoder;

    // load the first frame (i.e., the image)
    IWICBitmapFrameDecode * ipFrame = NULL;
    if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
        goto ReleaseDecoder;

    // convert the image to 32bpp BGRA format with pre-multiplied alpha
    //   (it may not be stored in that format natively in the PNG resource,
    //   but we need this format to create the DIB to use on-screen)
    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();

ReleaseDecoder:
    ipDecoder->Release();

Return:
    return ipBitmap;

}

//-----------------------------------------------------------------------------
// Creates a 32-bit DIB from the specified WIC bitmap.
//-----------------------------------------------------------------------------

HBITMAP CreateHBITMAP(IWICBitmapSource * ipBitmap)
{
    // initialize return value
    HBITMAP hbmp = NULL;

    // get image attributes and check for valid image
    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
    {
        return hbmp;
    }

    // prepare structure giving bitmap information
    // (negative height indicates a top-down DIB)
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG) height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    // create a DIB section that can hold the image
    void * pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);
    if (hbmp == NULL)
    {
        return hbmp;
    }

    // extract the image into the HBITMAP
    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))
    {
        // couldn't extract image; delete HBITMAP
        DeleteObject(hbmp);
        hbmp = NULL;
    }

    return hbmp;
}

//-----------------------------------------------------------------------------
// Loads the PNG containing the splash image into a HBITMAP.
//-----------------------------------------------------------------------------

HBITMAP LoadComponentImage(int resource)
{
    HBITMAP hbmpSplash = NULL;

    // load the PNG image data into a stream
    IStream * ipImageStream = CreateStreamOnResource(
        MAKEINTRESOURCE(resource), _("PNG"));
    if (ipImageStream == NULL)
    {
        return hbmpSplash;
    }

    // load the bitmap with WIC
    IWICBitmapSource * ipBitmap = LoadBitmapFromStream(ipImageStream);
    if (ipBitmap == NULL)
    {
        ipImageStream->Release();
        return hbmpSplash;
    }

    // create a HBITMAP containing the image
    hbmpSplash = CreateHBITMAP(ipBitmap);
    ipBitmap->Release();

    ipImageStream->Release();
    return hbmpSplash;
}