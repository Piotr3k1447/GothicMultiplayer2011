
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


#pragma warning( disable : 4996 )
#pragma warning( disable : 4244 )
#pragma once

//Includes
#include <vector>
#include <iostream>
#include <fstream>
#include "CBuilder.h"
#include "math.h"
#include "..\patch.h"
#include "..\HooksManager.h"
//

// Global
using namespace std;
#define PI 3.14159265
extern zSTRING FDefault;
extern bool WritingMapSave;
zSTRING fhuge = "FONT_OLD_20_WHITE.TGA";
zSTRING placed = " Placed.";
zSTRING PlayerSpawned = "Player spawned in vob position.";
zSTRING PlacedTest;
extern CBuilder* Builder;
//
void RenderEvent()
{
	if(Builder)Builder->Render();
};

	CBuilder::CBuilder() // Constructor
	{
		MobsWin = NULL;
		ParticleWin = NULL;
		ItemsWin = NULL;
		ObjMenu = NULL;
		FillMobNames();
		FillParticleNames();
		Game = oCGame::GetGame();
		screen = zCView::GetScreen();
		Hero = oCNpc::GetHero();
		keyboard = zCInput::GetInput();
		SMode = MOBS;
		CurrentMobType = MOB_NORMAL;
		CreateModeBoxes();
		DeleteAllNpcs();
		DirectionAngle = 90;
		DisableAllItemsInWorld();
		CurrentInter = oCMobInter::_CreateNewInstance();
		VisualName = MobNames[0];
		activeparticlename = 0, activemobname = 0, MoveSpeed = 1;
		LoadAllVobsFromTheWorld();
		TargetVisual = zCVisual::LoadVisual(MobNames[0]);
		CurrentInter->SetVisualInter(TargetVisual);
		Hero->SetPosition(0,0,0);
		Hero->SetMovLock(1);
		Game->GetWorld()->AddVob(CurrentInter);
		cam = zCAICamera::GetCurrent();
		cam->SetTarget(CurrentInter);
		oCNpc::SetGodMode(1);
		zCAICamera::SetLookingOnNpcCamera(true);
		oCGame::GetGame()->GetWorld()->SetVobFarClipZ(30000);
		oCGame::GetGame()->GetWorld()->GetOutdoorSkyController()->SetFarZScalability(3);
		CamDistHuman = cam->GetDistance();
		cam->SetMaxRange(30.0f);
		cam->SetDistance(7.0f);
		MobCollision = true;
		Patch::PlayerInterfaceEnabled(false);
		BuildMessages.push_back(zSTRING("World Builder 0.7 Alpha By Skejt23"));
		Mode = EDITING;
		HooksManager::GetInstance()->AddHook(HT_RENDER, (DWORD)RenderEvent, false);
	};

	CBuilder::~CBuilder()
	{
		oCNpc::SetGodMode(0);
		ClearUpBoxes();
		zCAICamera::SetLookingOnNpcCamera(false);
		cam->SetTarget(oCNpc::GetHero());
		CurrentInter->RemoveVobFromWorld();
		for(int i = 0; i < (int)SpawnedVobs.size(); i++)
		{
			SpawnedVobs[i].Vob->RemoveVobFromWorld();
		}
		SpawnedVobs.clear();
	};

	void CBuilder::Render()
	{
		if(!this) return;
		Patch::PlayerInterfaceEnabled(false);
		screen->SetFont(FDefault);
		if((int)BuildMessages.size()>0) for(int v=0; v<(int)BuildMessages.size(); v++) screen->Print(0, v*200, BuildMessages[v]);
		switch(Mode)
		{
		case EDITING: // EDITING START
		if(keyboard->KeyToggled(KEY_T) && !WritingMapSave){
			Mode = TESTING;
			LastPos = CurrentInter->GetPositionWorld();
			SaveVobMatrix(CurrentInter, LastMatrix);
			Hero->SetMovLock(0);
			ClearUpBoxes();
			zCAICamera::SetLookingOnNpcCamera(false);
			CollisionInObjectsEnabled(true);
			CheckMsgSize();
			BuildMessages.push_back(zSTRING("Test is starting!"));
			CamDistEditor = cam->GetDistance();
			cam->SetDistance(CamDistHuman);
			cam->SetTarget(Hero);
			CurrentInter->RemoveVobFromWorld();

		}
		if(keyboard->KeyToggled(KEY_F1)){
			if(SpawnedVobs.size() > 0){
				CheckMsgSize();
				sprintf(buffer,"Undo. Removed : %s", SpawnedVobs.back().VisualName.c_str());
				PlacedTest = buffer;
				BuildMessages.push_back(PlacedTest);
				PlacedTest.Clear();
				SpawnedVobs.back().Vob->RemoveVobFromWorld();
				SpawnedVobs.erase(SpawnedVobs.end()-1);
			}
		}
		if(keyboard->KeyToggled(KEY_F2)){
			if(SpawnedVobs.size() > 0){
				ObjMenu = new CObjectMenu();
				Mode = OBJECTMENU;
				LastPos = CurrentInter->GetPositionWorld();
				SaveVobMatrix(CurrentInter, LastMatrix);
				CurrentInter->RemoveVobFromWorld();
				ClearUpBoxes();
			}
		}
		if(keyboard->KeyToggled(KEY_HOME)){
			if(CurrentMobType >= MOB_DOOR) CurrentMobType = MOB_NORMAL;
			else CurrentMobType++;
			RecreateMobInstance(true, false);
			cam->SetTarget(CurrentInter);
		}
		if(keyboard->KeyToggled(KEY_END)){
			MobCollision = !MobCollision;
		}
		if(keyboard->KeyPressed(KEY_NUMPAD8)){
			CurrentInter->RotateWorldZ(1);
		}
		if(keyboard->KeyPressed(KEY_NUMPAD2)){
			CurrentInter->RotateWorldZ(-1);
		}
		if(keyboard->KeyPressed(KEY_NUMPAD4)){
			CurrentInter->RotateWorldX(1);
		}
		if(keyboard->KeyPressed(KEY_NUMPAD6)){
			CurrentInter->RotateWorldX(-1);
		}
		if(keyboard->KeyToggled(KEY_NUMPAD1)){
			if(MoveSpeed > 1) MoveSpeed--;
			else if (MoveSpeed > 0.25f && MoveSpeed <= 1) MoveSpeed-=0.25f;
		}
		if(keyboard->KeyToggled(KEY_NUMPAD3)){
			if(MoveSpeed < 6 && MoveSpeed >= 1) MoveSpeed++;
			else if (MoveSpeed >= 0.25f && MoveSpeed < 1) MoveSpeed+=0.25f;
		}
		if(keyboard->KeyPressed(KEY_LEFT)){
			DirectionAngle++;
			CurrentInter->RotateWorldY(-1);
		}
		if(keyboard->KeyPressed(KEY_RIGHT)){
			DirectionAngle--;
			CurrentInter->RotateWorldY(1);
		}
		if(keyboard->KeyPressed(KEY_UP)){
			double radian = CalculateRadians(DirectionAngle);
			float x,y;
			if(cam->GetDistance() < 7){
				x = ((float)cosf(radian) * 6) * MoveSpeed;
				y = ((float)sinf(radian) * 6) * MoveSpeed;
			}
			else{
				x = ((float)cosf(radian) * cam->GetDistance()+1) * MoveSpeed;
				y = ((float)sinf(radian) * cam->GetDistance()+1) * MoveSpeed;
			}
			CurrentInter->MoveWorld(x,0,y);
		}
		if(keyboard->KeyPressed(KEY_DOWN)){
			double radian = CalculateRadians(DirectionAngle);
			float x,y;
			if(cam->GetDistance() < 7){
				x = (-(float)cosf(radian) * 6) * MoveSpeed;
				y = (-(float)sinf(radian) * 6) * MoveSpeed; 
			}
			else{
				x = (-(float)cosf(radian) * cam->GetDistance()+1) * MoveSpeed;
				y = (-(float)sinf(radian) * cam->GetDistance()+1) * MoveSpeed; 
			}
			CurrentInter->MoveWorld(x,0,y);
		}
		if(keyboard->KeyPressed(KEY_DELETE)){
			double radian = CalculateRadians(DirectionAngle+90);
			float x,y;
			if(cam->GetDistance() < 7){
				x = ((float)cosf(radian) * 6) * MoveSpeed;
				y = ((float)sinf(radian) * 6) * MoveSpeed;
			}
			else{
				x = ((float)cosf(radian) * cam->GetDistance()+1) * MoveSpeed;
				y = ((float)sinf(radian) * cam->GetDistance()+1) * MoveSpeed;
			}
			CurrentInter->MoveWorld(x,0,y);
		}
		if(keyboard->KeyPressed(KEY_PGDN)){
			double radian = CalculateRadians(DirectionAngle-90);
			float x,y;
			if(cam->GetDistance() < 7){
				x = ((float)cosf(radian) * 6) * MoveSpeed;
				y = ((float)sinf(radian) * 6) * MoveSpeed;
			}
			else{
				x = ((float)cosf(radian) * cam->GetDistance()+1) * MoveSpeed;
				y = ((float)sinf(radian) * cam->GetDistance()+1) * MoveSpeed;
			}
			CurrentInter->MoveWorld(x,0,y);
		}
		if(keyboard->KeyPressed(KEY_X)){
			short y = 6 * MoveSpeed;
			CurrentInter->MoveWorld(0,y,0);
		}
		if(keyboard->KeyPressed(KEY_Z)){
			short y = -6 * MoveSpeed;
			CurrentInter->MoveWorld(0,y,0);
		}
		if(keyboard->KeyToggled(KEY_G)){
			Hero->ResetPos(CurrentInter->GetPositionWorld());
			Hero->SetMovLock(1);
			CheckMsgSize();
			BuildMessages.push_back(PlayerSpawned);
		}
		if(keyboard->KeyToggled(KEY_NUMPAD0)){
			CurrentInter->ResetRotationsWorld();
			zCAICamera::GetCurrent()->SetCameraAngle(90.0f);
			DirectionAngle = 90;
			CheckMsgSize();
			BuildMessages.push_back(zSTRING("Rotation has been reset."));
		}
		if(keyboard->KeyToggled(KEY_Q)){
			switch(SMode)
			{
			case MOBS:
				activemobname--;
				if(activemobname < 0) activemobname = (int)MobNames.size()-1;
				VisualName = MobNames[activemobname];
				TargetVisual = zCVisual::LoadVisual(MobNames[activemobname]);
				CurrentInter->SetVisualInter(TargetVisual);
			break;
			case PARTICLES:
				activeparticlename--;
				if(activeparticlename < 0) activeparticlename = (int)ParticleNames.size()-1;
				VisualName = ParticleNames[activeparticlename];
				TargetVisual = zCParticleFX::Load(ParticleNames[activeparticlename]);
				zCParticleFX* ParTemp = (zCParticleFX*)TargetVisual;
				if(!ParTemp->GetEmitter()->IsLooping()) ParTemp->GetEmitter()->SetLooping(1);
				CurrentInter->SetVisualInter(TargetVisual);
			}
		}
		if(keyboard->KeyToggled(KEY_E)){
			switch(SMode)
			{
			case MOBS:
				activemobname++;
				if(activemobname > (int)MobNames.size()-1) activemobname = 0;
				VisualName = MobNames[activemobname];
				TargetVisual = zCVisual::LoadVisual(MobNames[activemobname]);
				CurrentInter->SetVisualInter(TargetVisual);
			break;
			case PARTICLES:
				activeparticlename++;
				if(activeparticlename > (int)ParticleNames.size()-1) activeparticlename = 0;
				VisualName = ParticleNames[activeparticlename];
				TargetVisual = zCParticleFX::Load(ParticleNames[activeparticlename]);
				zCParticleFX* ParTemp = (zCParticleFX*)TargetVisual;
				if(!ParTemp->GetEmitter()->IsLooping()) ParTemp->GetEmitter()->SetLooping(1);
				CurrentInter->SetVisualInter(TargetVisual);
			break;
			}
		}
		if(keyboard->KeyToggled(KEY_NUMPADENTER) || keyboard->KeyToggled(KEY_S)){
			SpawnObject();
		}
		if(keyboard->KeyToggled(KEY_F5)){
			if(SMode != MOBS){
				SMode = MOBS;
				CreateModeBoxes();
				VisualName = MobNames[activemobname];
				TargetVisual = zCVisual::LoadVisual(MobNames[activemobname]);
				CurrentInter->SetVisualInter(TargetVisual);
			}
		}
		if(keyboard->KeyToggled(KEY_F6)){
			if(SMode != PARTICLES){
				SMode = PARTICLES;
				CreateModeBoxes();
				VisualName = ParticleNames[activeparticlename];
				TargetVisual = zCParticleFX::Load(ParticleNames[activeparticlename]);
				zCParticleFX* ParTemp = (zCParticleFX*)TargetVisual;
				if(!ParTemp->GetEmitter()->IsLooping()) ParTemp->GetEmitter()->SetLooping(1);
				CurrentInter->SetVisualInter(TargetVisual);
			}
		}
		/*if(keyboard->KeyToggled(KEY_F7)){
			if(SMode != ITEMS){
				SMode = ITEMS;
				CreateModeBoxes();
			}
		}*/
		// Loop things
		cam->SetMaxRange(30.0f);
		CurPos = CurrentInter->GetPositionWorld();
		PrintItemIds();
		break; // EDITING END
		case TESTING: // TESTING START
			if(keyboard->KeyToggled(KEY_T) && !WritingMapSave){
				Hero->SetMovLock(1);
				Hero->SetCollDet(0);
				Mode = EDITING;
				CreateModeBoxes();
				zCAICamera::SetLookingOnNpcCamera(true);
				CollisionInObjectsEnabled(false);
				RecreateMobInstance(false, true);
				CamDistHuman = cam->GetDistance();
				cam->SetTarget(CurrentInter);
				cam->SetDistance(CamDistEditor);
				CheckMsgSize();
				BuildMessages.push_back(zSTRING("Test has ended."));
			}
		// Loop things
			CurPos = Hero->GetPosition();
			cam->SetMaxRange(10.0f);
		break; // TESTING END
		case OBJECTMENU:
			if(ObjMenu) ObjMenu->Draw();
			if(keyboard->KeyToggled(KEY_F2) || keyboard->KeyPressed(KEY_ESCAPE)){
				keyboard->ClearKeyBuffer();
				ClearAfterObjMenu();
			}
		break; // OBJECTMENU END
		}
		if(Mode != OBJECTMENU){
			sprintf_s(buffer,"X : %d",static_cast<int>(CurPos.x));
			TextPosPrint = buffer;
			screen->Print(6300,3100,TextPosPrint);
			sprintf_s(buffer,"Z : %d",static_cast<int>(CurPos.y));
			TextPosPrint = buffer;
			screen->Print(6300,3300,TextPosPrint);
			sprintf_s(buffer,"Y : %d",static_cast<int>(CurPos.z));
			TextPosPrint = buffer;
			screen->Print(6300,3500,TextPosPrint);
			sprintf_s(buffer,"Speed: %4.2fx",MoveSpeed);
			TextPosPrint = buffer;
			screen->Print(6300,3700,TextPosPrint);
			if(MobCollision) TextPosPrint = "Mob Collision: ON";
			else TextPosPrint = "Mob Collision: OFF";
			screen->Print(6300,3900,TextPosPrint);
			switch(CurrentMobType)
			{
				case MOB_NORMAL:
					TextPosPrint = "Mob Type: NORMAL";
					screen->Print(6300,4100,TextPosPrint);
				break;
				case MOB_DOOR:
					TextPosPrint = "Mob Type: DOOR";
					screen->Print(6300,4100,TextPosPrint);
				break;
				case MOB_CONTAINER:
					TextPosPrint = "Mob Type: CONTAINER";
					screen->Print(6300,4100,TextPosPrint);
				break;
				case MOB_LADDER:
					TextPosPrint = "Mob Type: LADDER";
					screen->Print(6300,4100,TextPosPrint);
				break;
			};
		}
	};

	double CBuilder::CalculateRadians(double degree)
	{
		double radian = 0;
		radian = degree * (PI/180);
		return radian;
	};
	
	void CBuilder::RecreateMobInstance(bool RemoveOld, bool DoNotSave)
	{
		if(!DoNotSave){
			LastPos = CurrentInter->GetPositionWorld();
			SaveVobMatrix(CurrentInter, LastMatrix);
			if(RemoveOld) CurrentInter->RemoveVobFromWorld();
		}
		switch(CurrentMobType)
		{
			case MOB_NORMAL:
				CurrentInter = oCMobInter::_CreateNewInstance();
			break;
			case MOB_DOOR:
				CurrentInter = oCMobDoor::_CreateNewInstance();
			break;
			case MOB_CONTAINER:
				CurrentInter = oCMobContainer::_CreateNewInstance();
			break;
			case MOB_LADDER:
				CurrentInter = oCMobLadder::_CreateNewInstance();
			break;
		};
		CurrentInter->SetVisualInter(TargetVisual);
		Game->GetWorld()->AddVob(CurrentInter);
		CurrentInter->SetPositionWorld(LastPos);
		RestoreVobMatrix(CurrentInter, LastMatrix);
	};
	void CBuilder::SpawnObject()
	{
		switch(SMode)
		{
		case MOBS:
			CheckMsgSize();
			PlacedTest = MobNames[activemobname];
			PlacedTest = PlacedTest + placed;
			BuildMessages.push_back(PlacedTest);
			PlacedTest.Clear();
			LastPos = CurrentInter->GetPositionWorld();
			info.VisualName = MobNames[activemobname].ToChar();
			info.Vob = CurrentInter;
			if(MobCollision)info.Type = TYPE_MOB;
			else info.Type = TYPE_MOBNOCOLLIDE;
			info.MType = CurrentMobType;
			SpawnedVobs.push_back(info);
			RecreateMobInstance(false, false);
			cam->SetTarget(CurrentInter);
		break;
		case PARTICLES:
			CheckMsgSize();
			PlacedTest = ParticleNames[activeparticlename];
			PlacedTest = PlacedTest + placed;
			BuildMessages.push_back(PlacedTest);
			PlacedTest.Clear();
			LastPos = CurrentInter->GetPositionWorld();
			LastTrafo = CurrentInter->GetAngle();
			info.VisualName = ParticleNames[activeparticlename].ToChar();
			info.Vob = CurrentInter;
			info.Type = TYPE_PARTICLE;
			SpawnedVobs.push_back(info);
			CurrentInter = oCMobInter::_CreateNewInstance();
			zCVisual* vis = zCParticleFX::Load(ParticleNames[activeparticlename]);
			CurrentInter->SetVisualInter(vis);
			Game->GetWorld()->AddVob(CurrentInter);
			CurrentInter->SetPositionWorld(LastPos);
			CurrentInter->SetAngle(LastTrafo.x,LastTrafo.z);
			cam->SetTarget(CurrentInter);
		break;
		}
	}

	void CBuilder::PrintItemIds()
	{
		switch(SMode)
		{
		case MOBS:
			if(activemobname >= 0 && activemobname <= (int)MobNames.size()-1){
				sprintf_s(buffer,"Id : %d/%d",activemobname,(int)MobNames.size()-1);
				IdText = buffer;
				screen->Print(0,1600,IdText);
				screen->Print(0,1800,MobNames[activemobname]);
			}
		break;
		case PARTICLES:
			if(activeparticlename >= 0 && activeparticlename <= (int)ParticleNames.size()-1){
				sprintf_s(buffer,"Id : %d/%d",activeparticlename,(int)ParticleNames.size()-1);
				IdText = buffer;
				screen->Print(0,1600,IdText);
				screen->Print(0,1800,ParticleNames[activeparticlename]);
			}
		break;
		}
	}

	void CBuilder::ClearAfterObjMenu()
	{
		Mode = EDITING;
		RestoreVobMatrix(SpawnedVobs[ObjMenu->MenuPos].Vob, ObjMenu->LastAngle);
		CreateModeBoxes();
		RecreateMobInstance(false, true);
		cam->SetTarget(CurrentInter);
		delete ObjMenu;
		ObjMenu = NULL;
	}
	void CBuilder::ClearUpBoxes()
	{
		if(MobsWin) 
		{
			screen->RemoveItem(MobsWin);
			delete MobsWin;
			MobsWin = NULL;
		}
		if(ParticleWin) 
		{
			screen->RemoveItem(ParticleWin);
			delete ParticleWin;
			ParticleWin = NULL;
		}
		if(ItemsWin) 
		{
			screen->RemoveItem(ItemsWin);
			delete ItemsWin;
			ItemsWin = NULL;
		}
	}

	void CBuilder::CreateModeBoxes()
	{
		if(MobsWin) 
		{
			screen->RemoveItem(MobsWin);
			delete MobsWin;
		}
		if(ParticleWin) 
		{
			screen->RemoveItem(ParticleWin);
			delete ParticleWin;
		}
		if(ItemsWin) 
		{
			screen->RemoveItem(ItemsWin);
			delete ItemsWin;
		}
		MobsWin = new zCView(0,0,8192,8192,VIEW_ITEM);
		MobsWin->SetPos(2650,0);
		MobsWin->SetSize(800,250);
		MobsWin->InsertBack(zSTRING("MENU_INGAME.TGA"));
		if(SMode != MOBS){
		MobsWin->SetFontColor(zCOLOR(255,255,255,200));
		MobsWin->SetTransparency(50);}
		MobsWin->Print(500,350,zSTRING("Mobs (F5)"));
		screen->InsertItem(MobsWin,false);
		ParticleWin = new zCView(0,0,8192,8192,VIEW_ITEM);
		ParticleWin->SetPos(3450,0);
		ParticleWin->SetSize(1100,250);
		ParticleWin->InsertBack(zSTRING("MENU_INGAME.TGA"));
		if(SMode != PARTICLES){
		ParticleWin->SetFontColor(zCOLOR(255,255,255,200));
		ParticleWin->SetTransparency(50);}
		ParticleWin->Print(500,350,zSTRING("Particles (F6)"));
		screen->InsertItem(ParticleWin,false);
		/*ItemsWin = new zCView(0,0,8192,8192,VIEW_ITEM);
		ItemsWin->SetPos(4550,0);
		ItemsWin->SetSize(900,250);
		ItemsWin->InsertBack(zSTRING("MENU_INGAME.TGA"));
		if(SMode != ITEMS){
		ItemsWin->SetFontColor(zCOLOR(255,255,255,200));
		ItemsWin->SetTransparency(50);}
		ItemsWin->Print(500,350,zSTRING("Items (F7)"));
		screen->InsertItem(ItemsWin,false);*/
	}

	void CBuilder::CollisionInObjectsEnabled(bool isit)
	{
		if(isit){
		for(int i = 0; i < (int)SpawnedVobs.size(); i++)
		{
			if(SpawnedVobs[i].Type == TYPE_MOB) SpawnedVobs[i].Vob->SetCollDet(1);
		}
		}
		else{
			for(int i = 0; i < (int)SpawnedVobs.size(); i++)
			{
				if(SpawnedVobs[i].Type == TYPE_MOB) SpawnedVobs[i].Vob->SetCollDet(0);
			}
		}
	};

	void CBuilder::SaveVobMatrix(zCVob* Vob, zMAT4& Matrix)
	{
		Matrix.m[0][0] = Vob->trafoObjToWorld.m[0][0];
		Matrix.m[1][0] = Vob->trafoObjToWorld.m[1][0];
		Matrix.m[2][0] = Vob->trafoObjToWorld.m[2][0];
		Matrix.m[0][2] = Vob->trafoObjToWorld.m[0][2];
		Matrix.m[1][2] = Vob->trafoObjToWorld.m[1][2];
		Matrix.m[2][2] = Vob->trafoObjToWorld.m[2][2];
		Matrix.m[0][1] = Vob->trafoObjToWorld.m[0][1];
		Matrix.m[1][1] = Vob->trafoObjToWorld.m[1][1];
		Matrix.m[2][1] = Vob->trafoObjToWorld.m[2][1];
	};

	void CBuilder::RestoreVobMatrix(zCVob* Vob, zMAT4& Matrix)
	{
		Vob->trafoObjToWorld.m[0][0] = Matrix.m[0][0];
		Vob->trafoObjToWorld.m[1][0] = Matrix.m[1][0];
		Vob->trafoObjToWorld.m[2][0] = Matrix.m[2][0];
		Vob->trafoObjToWorld.m[0][2] = Matrix.m[0][2];
		Vob->trafoObjToWorld.m[1][2] = Matrix.m[1][2];
		Vob->trafoObjToWorld.m[2][2] = Matrix.m[2][2];
		Vob->trafoObjToWorld.m[0][1] = Matrix.m[0][1];
		Vob->trafoObjToWorld.m[1][1] = Matrix.m[1][1];
		Vob->trafoObjToWorld.m[2][1] = Matrix.m[2][1];
	};

	// Load all vobs from the world to vector
	void CBuilder::LoadAllVobsFromTheWorld()
	{
		zCListSort<zCVob>* VobList = oCGame::GetGame()->GetWorld()->GetVobList();
		int size = VobList->GetSize();
		for(int i=0; i<size; i++){
			VobList = VobList->GetNext();
			zCVob* VobInList = VobList->GetData();
			VobsInWorld.push_back(VobInList);
		}
	}

	void CBuilder::CheckMsgSize()
	{
		if(BuildMessages.size()>6) BuildMessages.erase(BuildMessages.begin()+1);
	};

	// Fills mob names
	void CBuilder::FillMobNames()
	{
		ifstream g2names(".\\WorldBuilder\\g2mobs.wb");
		char _buff[1024];
		if(g2names.good()){
   			while( !g2names.eof() ){
				g2names.getline(_buff,1024);
				MobNames.push_back(zSTRING(_buff));
			}
		}
		g2names.close();
	};

	void CBuilder::FillParticleNames()
	{
		ifstream g2particles(".\\WorldBuilder\\g2particles.wb");
		char _buff[1024];
		if(g2particles.good()){
   			while( !g2particles.eof() ){
				g2particles.getline(_buff,1024);
				ParticleNames.push_back(zSTRING(_buff));
			}
		}
		g2particles.close();
	};

	// Delete all npcs in current world
	void CBuilder::DeleteAllNpcs(){
		zCListSort<oCNpc>* NpcList=Game->GetWorld()->GetNpcList();
		int size=NpcList->GetSize();
		for(int i=0; i<size; i++){
			NpcList = NpcList->GetNext();
			oCNpc* NpcOnList = NpcList->GetData();
			if(NpcOnList->GetInstance()!=11471) NpcOnList->Disable();
		}
		Game->GetSpawnManager()->SetSpawningEnabled(0);
	};

	// Disabling all items in current world
	void CBuilder::DisableAllItemsInWorld()
	{
		zCListSort<oCItem>* ItemList = oCGame::GetGame()->GetWorld()->GetItemList();
		int size = ItemList->GetSize();
		oCWorld* World = Game->GetGameWorld();
		for(int i=0; i<size; i++){
			ItemList = ItemList->GetNext();
			oCItem* ItemInList = ItemList->GetData();
			World->DisableVob(ItemInList);
		}
	};