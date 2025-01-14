
/*
MIT License

Copyright (c) 2022 Gothic Multiplayer Team (pampi, skejt23, mecio)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*****************************************************************************
**																			**
**	File name:		CGmpClient/CSanta.h		   								**
**																			**
**	Created by:		04/12/11	-	skejt23									**
**																			**
**	Description:	Santa Claus	 											**
**																			**
*****************************************************************************/

#pragma once

#include "g2Api.h"
#include <vector>
#include <ctime>

using namespace std;

struct FlyStage
{
	zVEC3 StagePos;
};
class CSanta
{
private:
	time_t moveframe;
	time_t throwtime;
	oCVisualFX* Throw;
	oCNpc* SantaNPC;
	zCVob* Mover;
	zCVob* Mover1;
	short MaxStages;
	short CurrentStage;
	vector<FlyStage> Stages;
public:
	CSanta(short Map);
	~CSanta();
	void MoveToStage(FlyStage Stage);
	void RenderSanta();
};