// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FAWHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, HealthDelta);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIREANDWATER_API UFAWHealthComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UFAWHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return FMath::IsNearlyZero(CurrentHealth); }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return CurrentHealth; }

	float GetMaxHealth() const { return MaxHealth; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
	float MaxHealth = 100.0f;

	virtual void BeginPlay() override;

private:
	float CurrentHealth = 0.0f;

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	void ApplyDamage(float Damage);

	void SetHealth(float NewHealth);
};
