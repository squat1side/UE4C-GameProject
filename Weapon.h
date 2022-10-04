// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


//��ӻ������������õ�״̬���ڶ�ս - �����򻯵�enum
UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	//����pickup��Equipped״̬
	EWS_Pickup		UMETA(DisplayName = "Pickip"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	//����һ��Ĭ��ֵ
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


	//��Weaponһ��WeaponState����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;


	//����һ������ֵ - ������һ��Weapon Particle������������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	//������������ - ��װ������ʱ��ʾ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	//������������ - ��ʹ����������ʱ��ʾ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	//�������ʲ��ǹ���������� - ����USkeletalMeshComponent -����һ�������������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	//����������˺�Ч�� -- ���һ����ײ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	//���Damage��float����˺�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;


	//�������󶨵��ص���ʼ/�������¼���- ʵ��BeginPlay
protected:

	virtual void BeginPlay() override;


public:

	//����OnOverlapBegin����
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//����OnOverlapEnd����
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//����Equip����ʵ������װ����ɫ
	void Equip(class AMain* Char);

	//��������WeaponState������Setter��getter
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }


	//����CombatOnOverlapBegin����
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//����CombatOnOverlapEnd����
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//����Activate/DeactivateCollision����
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	//�Ե�������˺�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	//����������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

	//Setter
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }


};
