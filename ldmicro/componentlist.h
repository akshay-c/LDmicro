//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// Constants, structures, declarations etc. for the PIC ladder logic compiler
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------

#ifndef __COMPONENTLIST_H
#define __COMPONENTLIST_H

#define MAX_COMPONENTS            999
#define MAX_IMAGES                10

#define COMPONENT_WIDTH           100
#define COMPONENT_HEIGHT          100

extern  HDC             HdcMem;
extern  HWND            ComponentList;
extern  HBITMAP         test;
extern  HIMAGELIST      ComponentDiagrams;
extern  HWND            AdvancedWorkspace;
extern  BOOL            Dragging;
extern  int             ImagesDrawn;
extern  int             DragX;
extern  int             DragY;
extern  UINT            UniquePinId;
extern  UINT            UniqueImgId;


/*map<int, HIMAGELIST, int, int, int> Imagemap;                   //To relate image with it's drawing area(Index,Images,x,y,maptype)
map<int, BOOL, BOOL, BOOL> Switchmap;  //Add MCU pin later      //(Index(same as Imagemap), Selected, Default Connected,Latch Action)
map<int, BOOL>  Relaymap;              //Add MCU pin later      //(Index(same as Imagemap), Selected)

extern Imagemap test;*/

void ComponentListProc(NMHDR *h);
void ComponentListInitiate(void);

void InitializeImageList(HIMAGELIST *il);

int  BeginComponentDrag(int x, int y);
int  ComponentDrag(int x, int y);
int  EndComponentDrag(int x, int y);

// Componentimages.cpp functions
//Function Definitions

void               InitializeComponentImage(int Component, HIMAGELIST *il);
IStream *          CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType);
IWICBitmapSource * LoadBitmapFromStream(IStream * ipImageStream);
HBITMAP            CreateHBITMAP(IWICBitmapSource * ipBitmap);
HBITMAP            LoadComponentImage(int resource);


#endif