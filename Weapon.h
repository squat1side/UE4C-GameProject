// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


//添加基于武器所设置的状态用于对战 - 创建域化的enum
UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	//设置pickup和Equipped状态
	EWS_Pickup		UMETA(DisplayName = "Pickip"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	//设置一个默认值
	EWS_MAX			UMETA(DisplayName = "DefaultMax")
};


/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:

	AWeapon();


	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	FString Name;


	//给Weapon一个WeaponState变量
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;


	//设置一个布尔值 - 来创建一个Weapon Particle武器粒子属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	//创建声音变量 - 在装备武器时提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	//创建声音变量 - 在使用武器攻击时提示
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	//在武器资产是骨骼网格情况 - 声明USkeletalMeshComponent -创建一个骨骼网格组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	//对武器添加伤害效果 -- 添加一个碰撞盒体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	//添加Damage的float造成伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;


	//将函数绑定到重叠开始/结束的事件上- 实现BeginPlay
protected:

	virtual void BeginPlay() override;


public:

	//创建OnOverlapBegin函数
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//创建OnOverlapEnd函数
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//创建Equip函数实现武器装备角色
	void Equip(class AMain* Char);

	//创建用于WeaponState变量的Setter和getter
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }


	//创建CombatOnOverlapBegin函数
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//创建CombatOnOverlapEnd函数
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//创建Activate/DeactivateCollision函数
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	//对敌人造成伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	//武器触发者
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

	//Setter
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }


};
