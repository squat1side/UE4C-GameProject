// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"



//创建枚举Enum - 冲刺系统
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayNmae = "Normal"),
	//创建冲刺状态 - Sprinting的enum
	EMS_Sprinting UMETA(DisplayNmae = "Sprinting"),
	//创建Dead状态
	EMS_Dead UMETA(DisplayNmae = "Dead"),


	EMS_MAX UMETA(DisplayNmae = "DefaultMAX")
};


//创建一个Enum - EStaminStatus用于SprintingStatus
UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	//添加EStaninaStatus Normal -设定progressBar的正常状态
	ESS_Normal UMETA(DisplayName = "Normal"),
	//添加BelowMinimum最低限度状态
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	//添加Exhausted疲惫状态
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	//添加ExhaustedRecovering状态
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	//作为结束行，有明确的的最大值
	ESS_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	//创建一个变量 - TSubclassOf -用于存储武器
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	//添加bHasCombatTarget判断Main是否有对战目标
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	//创建bHasCombatTarget的Setter
	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	//设置敌人血条控件在屏幕显示的位置，大小 -  获取敌人位置
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	//添加控制器变量 - 用来调用显示/隐藏敌人血条函数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	//添加粒子系统
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	//添加被攻击时的音效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	//添加容器模板TArray - 类型为FVector的TArray
	TArray<FVector>PickupLocations;

	//在蓝图中可调用Debug
	UFUNCTION(BlueprintCallable)
	//标记拾取物所在位置
	void ShowPickupLocations();

	//创建第二个Enum - EMovementStatus - 用来描述角色在任意时间内的运动状态
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	//在AMain为Stamina创建enum
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	//为StaminaStatus创建setter函数：
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	//为体力消耗率添加变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	//添加最小冲刺体力值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	//设置一个插值更好的攻击Enemy
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	//创建一个碰撞目标来作为插值对象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	//设置AEnemy的setter
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	//创建一个Get Look At Rotation Yaw函数 - 用于调整角色战斗的朝向（插值）
	FRotator GetLookAtRotationYaw(FVector Target);

	//为MovementStatus添加setter函数 -设定运动状态和奔跑速度
	void SetMovementStatus(EMovementStatus Status);

	//为SetMovementStatus创建奔跑和冲刺之间的奔跑速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprinting")
	float SprintingSpeed;

	//创建布尔变量 -ShiftKeyDown - Bind - Keyboard Shitf
	bool bShiftKeyDown;

	//按下按键后就冲刺
	void ShiftKeyDown();

	//松开按键后就停止冲刺
	void ShiftKeyUp();

	//Camera Boom是把摄像机放在玩家后面，这是ue里面使用的比较统一的约定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//声明USpringArmComponent，是一个指针 CameraBoom
	class USpringArmComponent* CameraBoom;

	//FllowCamera是跟随玩家摄像机
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//声明UCameraComponent，指针为FollowCmaera
	class UCameraComponent* FollowCamera;

	//调节摄像机转向功能的基础转向功率
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;


	//玩家数据 - 血条，耐力，硬币等
//创建百分比血量的血条和耐力条
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	//创建血条的减少 - Explosive
	void DecrementHealth(float Amount);

	//创建TakeDamge伤害函数
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	//创建增加硬币 - Pickup
	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);
	//创建增加血量 - 
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amout);

	//当角色血量小于等于0时，角色死亡；
	void Die();

	//角色死亡不能调用Jump
	virtual void Jump() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//需要向前/向后的输入
	void MoveForward(float Value);

	//需要从一边到另一边的输入
	void MoveRight(float Value);

	//需要Yaw Rotation的输入
	void Turn(float Value);

	//需要Pitch Rotation的输入
	void Lookup(float Value);

	bool bMovingForward;
	bool bMovingRight;

	//实现单出现后，停止角色在世界的所有运动
	bool CanMove(float Value);


	//通过输入，实现特定速率的转向
	//@参数 - 比率的参数  - @param Rate - 标准化的比率- 即：1.0表示100%的转向速率
	void TurnAtRate(float Rate);

	//通过输入，实现特定速率的抬起/下Look up/down
	//@参数 - 比率的参数  - @param Rate - 标准化的比率- 即：1.0表示100%的抬起/下速率
	void LookUpAtRate(float Rate);

	//创建LMB鼠标左键绑定函数
	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	//创建ECS键绑定函数
	bool bESCDown;
	void ESCDown();
	void ESCUp();





	//创建F键绑定函数
	bool bFDown;
	void FDown();
	void FUp();






	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//添加一个Weapon变量,在装备武器后，立即输入这个变量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	//创建变量AItem来实现与角色重叠的Item进行鼠标等操作配备
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;


	//创建一个设置装备武器的Setter
	void SetEquippedWeapon(AWeapon* WeaponToSet);
	//为EquippedWeapon设置getter
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	//为ActiveOverlappingItem创建一个setter
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }


	//为AnimMotage添加功能：
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	//设置Attack函数
	void Attack();

	//在攻击完成后，创建AttackEnd函数停止攻击继续
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//创建动画蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	//创建PlaySwingSound
	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	//创建DeathEnd
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	//创建一个新的函数 UpdateCombatTarget - 在旧的目标死亡后就能获取新的战斗目标
	void UpdateCombatTarget();

	//创建TSubclassOf<AActor>的变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	//创建过渡关卡函数
	void SwitchLevel(FName LevelName);


	//创建存储和载入游戏函数
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	//创建LoadGameNoSwitch - 用于关卡间的切换
	void LoadGameNoSwitch();

};
