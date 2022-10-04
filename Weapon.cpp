// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SKeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SKeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


//创建Weapon构造函数
AWeapon::AWeapon()
{
	//完善SkeletalMesh
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMesh"));
	//附加到根组件
	SkeletalMesh->SetupAttachment(GetRootComponent());

	//将盒体组件附加到根
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	//为盒体添加碰撞预设
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);



	//bWeaponParticles
	bWeaponParticles = false;

	//WeaponState设置默认值
	WeaponState = EWeaponState::EWS_Pickup;

	//为Damage设置默认值 -武器伤害值
	Damage = 75.f;
}

//BeginPlay
void AWeapon::BeginPlay()
{
	//调用父类
	Super::BeginPlay();

	//利用CombatCollision实现功能
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);



}

//创建OnOverlapBegin函数
void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//调用Super来调用父级版本
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}

}

//创建OnOverlapEnd函数
void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}



//实现角色装备武器  - Equip函数
void AWeapon::Equip(AMain* Char)
{
	if (Char)
	{
		//设置Instigator
		SetInstigator(Char->GetController());

		//调整忽略武器对角色和摄像机之间的影响，不会影响玩家角度的放大
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		//将CollisionResponseToChannel设置成被Pawn忽略 -不发生与Pawn碰撞
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		//停止物理模拟
		SkeletalMesh->SetSimulatePhysics(false);

		//获取RightHandSocket
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		//判定RightHandSocket有效性
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			//设置Weapon在装备人物时候不需要ratate
			bRotate = false;

			//将EquippedWeapon设定成特定武器的实例
			Char->SetEquippedWeapon(this);
			//将SetActiveOverlappingOverItem设为nullptr
			Char->SetActiveOverlappingItem(nullptr);
		}
		//判定EquipSound来调用UGmaeplayStatics
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		//判断如果拾取了武器，可以判定Weapon Particles是true还是false
		if (!bWeaponParticles)
		{
			IdelParticlesComponent->Deactivate();
		}
	}
}


//创建战斗碰撞OnOverlapBegin函数
void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				//在Socket位置生成发射器
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					//生成粒子系统
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			//处理ApplyDamage
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

//创建战斗碰撞OnOverlapEnd函数
void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


//Activate/DeactivateCollision函数
void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
