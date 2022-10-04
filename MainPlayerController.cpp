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
	//��HUDOverlay��ӵ��Ӵ���
	HUDOverlay->AddToViewport();
	//�����ض���HUD�Ƿ���ʾ����Ļ��
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	//�ж�����Ѫ��
	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		//���ö���ؼ�
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	//�ж���Ϸ�˵�
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

//�ж�Ѫ���Ƿ���ʾ����������Ļ
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
	//����super
	Super::Tick(DeltaTime);

	//������Ѫ��
	if (EnemyHealthBar)
	{
		//����һ����ά���� - ��������ĻX��Y�� -2D������FVector2D)
		FVector2D PositionInViewport;
		//���ú�����ȡһ������λ�� - ӳ�䵽��ά����
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		//����Ѫ����Enemy��λ�� - Y�᣺
		PositionInViewport.Y -= 80.f;

		//ʹ��FVector������Ѫ���ؼ��Ĵ�С
		FVector2D SizeInViewport = FVector2D(200.f, 20.f);

		//���� - �����ߴ�
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

//��Ϸ�˵���ʾ����������Ļ
void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		//ʵ������굥����ť����
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

		//�˵�����
		bPauseMenuVisible = false;
	}
}

//���PauseMenu�Ƿ��
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
	//ʵ������굥����ť���ܲ�Ӱ����Ϸ����
	FInputModeGameOnly InputModeGameOnly;

	SetInputMode(InputModeGameOnly);
}