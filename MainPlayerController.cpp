// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"



void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	//将HUDOverlay添加到视窗中
	HUDOverlay->AddToViewport();
	//设置特定的HUD是否显示在屏幕上
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	//判定敌人血条
	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		//设置对齐控件
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	//判定游戏菜单
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

//判定血条是否显示或隐藏在屏幕
void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

//Tick
void AMainPlayerController::Tick(float DeltaTime)
{
	//调用super
	Super::Tick(DeltaTime);

	//检查敌人血条
	if (EnemyHealthBar)
	{
		//设置一个二维向量 - 来代表屏幕X和Y轴 -2D向量（FVector2D)
		FVector2D PositionInViewport;
		//调用函数获取一个世界位置 - 映射到二维坐标
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		//调整血条在Enemy的位置 - Y轴：
		PositionInViewport.Y -= 80.f;

		//使用FVector来控制血条控件的大小
		FVector2D SizeInViewport = FVector2D(200.f, 20.f);

		//调用 - 调整尺寸
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

//游戏菜单显示或隐藏在屏幕
void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		//实现鼠标贯标单击按钮功能
		FInputModeGameAndUI InputModeGameAndUI;

		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();

		bShowMouseCursor = false;

		//菜单隐藏
		bPauseMenuVisible = false;
	}
}

//检查PauseMenu是否打开
void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}


void AMainPlayerController::GameModeOnly()
{
	//实现鼠标贯标单击按钮功能并影响游戏功能
	FInputModeGameOnly InputModeGameOnly;

	SetInputMode(InputModeGameOnly);
}