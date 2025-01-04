// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/FAWHealthComponent.h"

UFAWHealthComponent::UFAWHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFAWHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHealth(MaxHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UFAWHealthComponent::OnTakeAnyDamage);
		ComponentOwner->OnTakePointDamage.AddDynamic(this, &UFAWHealthComponent::OnTakePointDamage);
		ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &UFAWHealthComponent::OnTakeRadialDamage);
	}
}

void UFAWHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	ApplyDamage(Damage);
}

void UFAWHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{

}

void UFAWHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
}

void UFAWHealthComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0 || IsDead())
	{
		return;
	}

	SetHealth(CurrentHealth - Damage);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

void UFAWHealthComponent::SetHealth(float NewHealth)
{
	const float NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const float HealthDelta = NextHealth - CurrentHealth;

	CurrentHealth = NextHealth;
	OnHealthChanged.Broadcast(CurrentHealth, HealthDelta);
}
