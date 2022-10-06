// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"


// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//创建Camera Boom （出现碰撞时，推向玩家）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	//设置弹簧臂 - 并将摄像机跟随这一方向
	CameraBoom->TargetArmLength = 600.f;
	//旋转基于控制器 - 控制器属于Pawn类
	CameraBoom->bUsePawnControlRotation = true;

	//为碰撞胶囊体设定大小
	GetCapsuleComponent()->SetCapsuleSize(25.f, 74.f);

	//创建Follow摄像机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//设置Attach，Attach不是设置到RootComponent，摄像机要附加到CameraBoom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//将摄像机附加到Boom末端，并让Pawn进行调整，来实现匹配控制器的方向
	//只将摄像机附加到CameraBoom，并跟随CameraBoom移动，不依赖于控制器
	FollowCamera->bUsePawnControlRotation = false;

	//为输入设置转向速率
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//控制器旋转的时候不要旋转
	//仅影响摄像机
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//设置面向角色移动
	GetCharacterMovement()->bOrientRotationToMovement = true;//角色沿着基于输入的方向移动
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);//基于此旋转速率
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//设置Player Stats默认数值
	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 350.f;
	Stamina = 120.f;
	Coins = 0;

	//初始化Running和SpringSpeed并赋值
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	//bShiftKeyDown
	bShiftKeyDown = false;

	//bLMBDown
	bLMBDown = false;



	bFDown = false;



	//bESCDown
	bESCDown = false;

	//Stamina - 初始化Enum
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	//初始化体力消耗率和最小冲刺体力值
	StaminaDrainRate = 25.f;
	MinSprintStamina = 75.f;

	//设置InterSpeed和InterToEnemy默认数值
	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	//bHasCombatTarget
	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
	//设置MainplayerController
	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "Farm")
	{
		//调用LoadGameNoSwitch
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//使用比率来实现消耗体力
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	//实现StaminaStatus的四种状态
	switch (StaminaStatus)
	{
		//实现Normal状态情况
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);//在疲惫状态恢复中
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);//在正常状态中
			}
		}
		//ShiftKeyUp - 判定体力是否恢复
		else
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

		//实现BelowMinimum状态情况
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);//在疲惫状态恢复中
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);//在正常状态中
				}
			}
		}
		//shift key up
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

		//实现Exhausted状态情况
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		//shift key up
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		//在Exhausted状态不会sprinting
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

		//实现ExhaustedRecovering状态情况
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	//检查插值和检查是否在对战
	if (bInterpToEnemy && CombatTarget)
	{
		//锁定目标
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		//使用插值 - 作为过度帧 -平稳过度视角
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	//更新对战目标的位置
	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		//利用对战目标位置更新控制器
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

//GetLookAtRotationYaw函数
FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//检查函数有效性
	check(PlayerInputComponent);

	//动作映射的跳跃绑定 - 一次性的  - 关键词 -IE_Pressed - IE_Released - 按下/松开键
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//动作映射的冲刺绑定 - 一次性的  - 关键词 - IE_Pressed - IE_Released - 按下/松开键
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	//动作映射的ESC绑定 - 一次性的  - 关键词 - IE_Pressed - IE_Released - 按下/松开键
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);



	PlayerInputComponent->BindAction("F", IE_Pressed, this, &AMain::FDown);
	PlayerInputComponent->BindAction("F", IE_Released, this, &AMain::FUp);



	//动作映射的冲刺绑定 - 一次性的  - 关键词 - IE_Pressed - IE_Released - 按下/松开键
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBUp);

	//为键盘添加
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	//为鼠标添加
	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::Lookup);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}

//实现菜单出现停止角色在世界的所有运动
bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) && 
			(!bAttacking) &&
			(MovementStatus != EMovementStatus::EMS_Dead) &&
			!MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

//需要Yaw Rotation的输入
void AMain::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

//需要Pitch Rotation的输入
void AMain::Lookup(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}


void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		//找出前方
		const FRotator Rotation = Controller->GetControlRotation();//控制器的旋转
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//创建向前矢量 - RotationMatrix是用来获取特定方向的矢量，例如actor的坐标和世界坐标不统一时；控制器的向前矢量
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMain::TurnAtRate(float Rate)
{

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


//创建LMBDown/Up
void AMain::LMBDown()
{
	bLMBDown = true;
	//死亡后不能用LMB功能
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//添加控制器判断 - 显示菜单后不能触发攻击等
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;


	//设置攻击
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;

}





void AMain::FDown()
{
	bFDown = true;
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			//将SetActiveOverlappingOverItem设为nullptr
			SetActiveOverlappingItem(nullptr);
		}
	}
}
void AMain::FUp()
{
	bFDown = false;
}












//实现ESC
void AMain::ESCDown()
{
	bESCDown = true;

	//启用菜单的显示属性
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}

}
void AMain::ESCUp()
{
	bESCDown = false;
}



//创建:DecrementHealth
void AMain::DecrementHealth(float Amount)
{
	
}

//创建IncrementCoins
void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

//创建增加血量机制函数
void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}


//创建Die
void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

//角色死亡不在调用Jump
void AMain::Jump()
{
	//添加控制器判断 - 显示菜单后不能触发跳跃等
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}


//创建DeathEnd -处理角色死亡后可以移动
void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

//创建Setter - MovementStatus
void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	//判断进入冲刺
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

//Shift按下函数主体
void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

//Shift松开函数主体
void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

//实现ShowPickupLocations功能
void AMain::ShowPickupLocations()
{
	//标记所有收集的硬币每一个的位置
	for (auto Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, .5f);
	}
}


//创建一个设置装备武器的Setter
void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}


//设置Attack函数
void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy(true);
		//在适当时间进行插值
		SetInterpToEnemy(true);

		bAttacking = true;
		//获取Anim实例
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				//应用Montage Play函数来播放蒙太奇 -播放速度为2.2倍速
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				//在播放蒙太奇时，多段动画资产特定跳至某一片段
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				//应用Montage Play函数来播放蒙太奇 -播放速度为2.2倍速
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				//在播放蒙太奇时，多段动画资产特定跳至某一片段
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			default:
				;
			}
		}
	}
}

//创建AttackEnd函数
void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	//按住LMB可以持续攻击
	if (bLMBDown)
	{
		Attack();
	}
}

//PlaySwingSound
void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}



//SetInterpToEnemy函数
void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}


//创建TakeDamge伤害函数
float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//判断角色是否应该死亡及血条减少
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		//转换为敌人
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				//角色死亡enemy的布尔为false
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

//函数 UpdateCombatTarget
void  AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	//获取Enemy
	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

//创建过渡关卡函数
void  AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}



//创建存储和载入游戏函数
void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	//设置变量
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;

	//保存关卡名
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	//判断是否装备武器
	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);

}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	//为WeaponStroage（U类） - 创建一个实例 - 去生成一个Actor
	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}

	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	//存 -载入游戏中的角色状态问题
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	//判断关卡名
	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName = (*LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}
}


//创建LoadGameNoSwitch - 用于关卡间的切换
void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	//为WeaponStroage（U类） - 创建一个实例 - 去生成一个Actor
	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}
	//存 -载入游戏中的角色状态问题
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

}