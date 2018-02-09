#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincodec.h>

#include "ldmicro.h"
#include "advanceddialog.h"
#include "componentlist.h"
#include "components\componentimages.h"
#include "components\components.h"

// I/O that we have seen recently, so that we don't forget pin assignments
// when we re-extract the list

// static HBITMAP testmask=NULL;

HIMAGELIST      ComponentDiagrams;
static int      ComponentCount;
ImageLocation   ImageStack[MAX_SCREEN_ITEMS];
PinInfo         PinStack[MAX_PINS];
int             ImagesDrawn = 0;
int             DragIndex   = 0;
int             DragOffsetX = 0;
int             DragOffsetY = 0;
int             DragX       = 0;
int             DragY       = 0;
BOOL            Dragging    = FALSE;
UINT            UniqueImgId = 0;

// Imagemap test;

void ComponentListInitiate(void)
{
    int i;
    LVITEM lvi;
    for(i = 0; i < TOTAL_COMPONENTS; i++)
    {
        lvi.mask        = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        lvi.state       = lvi.stateMask = 0;
        lvi.iItem       = i;
        lvi.iSubItem    = 0;
        lvi.pszText     = rgCompData[i].ComponentName;
        lvi.lParam      = rgCompData[i].Index;
        if(ListView_InsertItem(ComponentList, &lvi) < 0) oops();
    }
    // test.insert(make_pair(1, ComponentDiagrams, 100, 100, 6000));
}

void ComponentListProc(NMHDR *h)
{
    switch(h->code) {
        case LVN_ITEMACTIVATE:
        {
            int Img, Index;
            NMITEMACTIVATE *i = (NMITEMACTIVATE *)h;
            LVITEM lvi;
            lvi.mask = LVIF_PARAM;
            lvi.iItem = ListView_GetNextItem(ComponentList, -1,  LVNI_SELECTED); 
            ListView_GetItem(ComponentList, &lvi);
            Index = (int) lvi.lParam;
            int CompId = rgCompData[Index].ComponentId;
            ImageStack[ImagesDrawn].Image = 
                (ImageStruct *)AllocImageHeap(sizeof(ImageStruct));
            ImageStack[ImagesDrawn].Image->ComponentId = CompId;
            ImageStack[ImagesDrawn].Properties =
                AllocImageHeap(GetStructSize(CompId));
            ImageStack[ImagesDrawn].PinName =
                AllocImageHeap(GetNameSize(Index));
            ImageStack[ImagesDrawn].Id = ++UniqueImgId;
            ImageStack[ImagesDrawn].Index = Index;
            Img = InitializeComponentProperties(
                ImageStack[ImagesDrawn].Properties, CompId);
            InitializeImageList(&ImageStack[ImagesDrawn].Image->Images);
            InitializeComponentImage(Img,
                &ImageStack[ImagesDrawn].Image->Images);
            ImageStack[ImagesDrawn].x = 100;
            ImageStack[ImagesDrawn].y = 100;
            // RECT TempRect;
            // GetWindowRect(AdvancedWorkspace, &TempRect);
            InvalidateRect(AdvancedWorkspace, NULL, TRUE);

            ImagesDrawn++;
            // char a[4];
            // MessageBox(ComponentList, (LPCTSTR)itoa(lvi.lParam, a, 10), "Info", MB_OK | MB_ICONEXCLAMATION);
                // InitializeComponentImage((int)lvi.lParam);
            // int *temp;
            // temp = (int *)i->lParam;
            // MessageBox(ComponentList, (LPCTSTR)itoa(*temp, a, 10), "Info", MB_OK | MB_ICONEXCLAMATION);
            //Selection Code Here

            // test=LoadComponentImage(RELAY_NC);
            // if(test ==NULL){
            //     MessageBox(ComponentList, "Could not load Bitmap!", "Error", MB_OK | MB_ICONEXCLAMATION);
            // }
        }
        break;
        default:
        break;
    }
}

void InitializeImageList(HIMAGELIST *il)
{
    *il = ImageList_Create(COMPONENT_WIDTH, COMPONENT_HEIGHT, ILC_MASK | ILC_COLOR8, 0, MAX_SCREEN_ITEMS);
    if(*il == NULL){
        MessageBox(ComponentList, "Could not Initialize ImageList", "Error", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
}

void RefreshImages()
{
    InvalidateRect(AdvancedWorkspace, NULL, TRUE);
}

void SetImage(int Component, void *il)
{
    InitializeComponentImage(Component,(HIMAGELIST*) il);
}

int BeginComponentDrag(int x, int y)
{
    int i;
    for(i = 0; i < ImagesDrawn; i++)
    {
        if((((ImageStack[i].x + COMPONENT_WIDTH) > x) && (ImageStack[i].x < x)
            && (ImageStack[i].y < y) && ((ImageStack[i].y + COMPONENT_HEIGHT) > y)))
        {
            // RECT TempRect;
            // TempRect.left = ImageStack[i].x;
            // TempRect.top = ImageStack[i].y;
            // TempRect.right = ImageStack[i].x + 100;
            // TempRect.bottom = ImageStack[i].y + 100;

            DragOffsetX = x - ImageStack[i].x;
            DragOffsetY = y - ImageStack[i].y;
            DragX = x;
            DragY = y;

            // MessageBox(ComponentList, "Clicked Image", "Error", MB_OK | MB_ICONEXCLAMATION);
            ImageList_BeginDrag(ImageStack[i].Image->Images, 0, DragOffsetX, DragOffsetY);
            ImageList_SetDragCursorImage(ImageStack[i].Image->Images, 0, 0, 0);
            ImageList_DragEnter(AdvancedWorkspace, x, y);

            // ImageStack[i].x = -200;
            // ImageStack[i].y = -200;
            // InvalidateRect(AdvancedWorkspace, &TempRect, TRUE);
            DragIndex = i;
            Dragging = TRUE;
            return 0;
        }
    }
    // HBITMAP temp=(HBITMAP)GetCurrentObject(GetDC(AdvancedWorkspace), OBJ_BITMAP);
    // if(temp == NULL)
    // {
    //     MessageBox(ComponentList, "Bitmap Found", "Information", MB_OK | MB_ICONEXCLAMATION);
    // }
    return 0;
}
int ComponentDrag(int x, int y)
{
    if(Dragging)
    {
        ImageList_DragMove(x, y);
        // ImageStack[DragIndex].x = x - DragOffsetX;
        // ImageStack[DragIndex].y = y - DragOffsetY;
        // InvalidateRect(AdvancedWorkspace, NULL, TRUE);
    }
    else
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    return 0;
}

int EndComponentDrag(int x, int y)
{
    if(Dragging)
    {
        // HBITMAP TempBM;
        // HDC HdcMem2 = GetDC(AdvancedWorkspace);
        // HDC HdcMem =GetDC(AdvancedWorkspace);
        // RECT TempRect;
        // GetWindowRect(AdvancedWorkspace, &TempRect);
        ImageList_DragLeave(AdvancedWorkspace);
        ImageList_EndDrag();
        ImageStack[DragIndex].x = x - DragOffsetX;
        ImageStack[DragIndex].y = y - DragOffsetY;
        // InvalidateRect(AdvancedWorkspace, &TempRect, TRUE);
        InvalidateRect(AdvancedWorkspace, NULL, TRUE);
        // TempBM = CreateBitmap(COMPONENT_WIDTH, COMPONENT_HEIGHT, 1, 32, NULL);

        // BitBlt(HdcMem2, x, y, COMPONENT_WIDTH, COMPONENT_HEIGHT, HdcMem, 100, 100, SRCERASE);
        Dragging =FALSE;
        return DragIndex;
    }
    // IMAGEINFO imginfo;
    // ImageList_GetImageInfo(ComponentDiagrams,0, &imginfo);
    // char vx[4],vy[4];
    // itoa(imginfo.rcImage.right,vx,10);
    // itoa(imginfo.rcImage.bottom,vy,10);
    // MessageBox(ComponentList, _(vx), _(vy), MB_OK | MB_ICONEXCLAMATION);
    // imginfo.rcImage.left=x;
    // imginfo.rcImage.top=y;
    // ImageList_Draw(ComponentDiagrams, 0, GetDC(AdvancedWorkspace), x, y, ILD_TRANSPARENT);
    return NULL;
}
