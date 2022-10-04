// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	PlayerName = TEXT("Default");

	UserIndex = 0;

	CharacterStats.WeaponName = TEXT("");

	//给关卡名一个默认值
	CharacterStats.LevelName = TEXT("");

}