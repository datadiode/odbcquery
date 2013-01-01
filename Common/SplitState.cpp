/*/SplitState.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2006 Jochen Neubeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "StdAfx.h"
#include "SplitState.h"

BOOL CSplitState::Split(HWND hwndOuter)
{
	int bKeepCursor = 0;
	if (const LONG *pScript = SplitScript)
	{
		HDWP hdwp = ::BeginDeferWindowPos(2);
		static POINT pntTrack;
		static POINT pntReset;
		static HWND hwndTrack;
		static int ht = 0;
		WINDOWPLACEMENT wp;
		wp.length = sizeof wp;
		::GetWindowPlacement(hwndOuter, &wp);
		SHORT vkButton = GetKeyState(VK_LBUTTON);
		POINT pntCursor;
		::GetCursorPos(&pntCursor);
		BOOL bCancel = FALSE;
		if (vkButton >= 0)
		{
			::ReleaseCapture();
			hwndTrack = 0;
			ht = 0;
		}
		else if (::GetCapture() == 0)
		{
			if (hwndTrack)
			{
				pntCursor = pntReset;
				bCancel = TRUE;
			}
			else
			{
				pntTrack = pntCursor;
			}
		}
		const LONG *pMetric = pScript;
		while (!bKeepCursor && *pScript)
		{
			while (!(*pMetric & LONG_MIN))
				++pMetric;

			if (pScript[1] & LONG_MIN)
			{
				pMetric = pScript = pMetric + 1;
				continue;
			}

			HWND hwndPane[2] =
			{
				::GetDlgItem(hwndOuter, pScript[0]),
				::GetDlgItem(hwndOuter, pScript[1])
			};

			int cxyMin[2] =
			{
				~*pMetric,
				~(pMetric[1] & LONG_MIN ? *++pMetric : *pMetric)
			};

			RECT rctPane[2];
			::GetWindowRect(hwndPane[0], &rctPane[0]);
			::GetWindowRect(hwndPane[1], &rctPane[1]);

			RECT rctGap;
			enum
			{
				vSplitterBox = 1,
				hSplitterBox = 2
			};
			int cx = 0, cy = 0;
			if (rctPane[1].top > rctPane[0].bottom && ht != hSplitterBox)
			{
				rctGap.left = rctPane[0].left;
				rctGap.top = rctPane[0].bottom;
				rctGap.right = rctPane[1].right;
				rctGap.bottom = rctPane[1].top;
				cy = pntCursor.y - pntTrack.y;
				if (rctPane[0].bottom + cy < rctPane[0].top + cxyMin[0])
				{
					cy = rctPane[0].top + cxyMin[0] - rctPane[0].bottom;
				}
				if (rctPane[1].top + cy > rctPane[1].bottom - cxyMin[1])
				{
					cy = rctPane[1].bottom - cxyMin[1] - rctPane[1].top;
				}
				ht = vSplitterBox;
			}
			else
			{
				if (rctPane[0].top > rctPane[1].bottom && rctPane[0].left > rctPane[1].right)
				{
					RECT rctTemp = rctPane[0];
					rctPane[0] = rctPane[1];
					rctPane[1] = rctTemp;
					HWND hwndTemp = hwndPane[0];
					hwndPane[0] = hwndPane[1];
					hwndPane[1] = hwndTemp;
				}
				rctGap.left = rctPane[0].right;
				rctGap.top = rctPane[0].top;
				rctGap.right = rctPane[1].left;
				rctGap.bottom = rctPane[1].bottom;
				cx = pntCursor.x - pntTrack.x;
				if (rctPane[0].right + cx < rctPane[0].left + cxyMin[0])
				{
					cx = rctPane[0].left + cxyMin[0] - rctPane[0].right;
				}
				if (rctPane[1].left + cx > rctPane[1].right - cxyMin[1])
				{
					cx = rctPane[1].right - cxyMin[1] - rctPane[1].left;
				}
				ht = hSplitterBox;
			}
			if (hwndTrack == 0 && ::PtInRect(&rctGap, pntCursor) || hwndTrack == *hwndPane)
			{
				bKeepCursor = 1;
				::SetCursor(::LoadCursor(0, ht == vSplitterBox ? IDC_SIZENS : IDC_SIZEWE));
				if (vkButton < 0)
				{
					if (::GetCapture() == 0 && !bCancel)
					{
						::SetCapture(hwndOuter);
						pntReset = pntCursor;
					}
					if (cx || cy)
					{
						::MapWindowPoints(0, hwndOuter, (LPPOINT)&rctPane[0], 2);
						do
						{
							::GetWindowRect(hwndPane[1], &rctPane[1]);
							::MapWindowPoints(0, hwndOuter, (LPPOINT)&rctPane[1], 2);
							(rctPane[1].left > rctPane[0].right ? rctPane[1].left : rctPane[1].right) += cx;
							(rctPane[1].top > rctPane[0].top ? rctPane[1].top : rctPane[1].bottom) += cy;
							rctPane[1].right -= rctPane[1].left;
							rctPane[1].bottom -= rctPane[1].top;
							::UpdateWindow(hwndOuter);
							hdwp = ::DeferWindowPos(hdwp, hwndPane[1], 0, rctPane[1].left, rctPane[1].top, rctPane[1].right, rctPane[1].bottom, SWP_NOZORDER);
							if (hwndPane[1] == hwndPane[0])
								break;
							hwndPane[1] = ::GetWindow(hwndPane[1], GW_HWNDPREV);
						} while (hwndPane[1]);
						_ASSERT(hwndPane[1]);
						bKeepCursor = 2;
					}
					hwndTrack = *hwndPane;
				}
			}
			++pScript;
		}
		::EndDeferWindowPos(hdwp);
		pntTrack = pntCursor;
	}
	return bKeepCursor;
}
