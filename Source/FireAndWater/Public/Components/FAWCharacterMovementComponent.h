// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FAWCharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FFAWCharacterGroundInfo
{
	GENERATED_BODY()


	FFAWCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

UCLASS()
class FIREANDWATER_API UFAWCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "FAW|CharacterMovement")
	const FFAWCharacterGroundInfo& GetGroundInfo();

	void ForceSprint();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetMaxSpeed() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sprinting", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxSprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sprinting")
	float SpeedTolerance = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sprinting")
	float TimeToStartSprint = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sprinting")
	float MaxTimeToSprint = 3.0f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "FAW|CharacterMovement")
	bool bSprint = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float TimeOnMaxSpeed = 0.0f;

	FFAWCharacterGroundInfo CachedGroundInfo;

	void HandleSprint(const float& DeltaTime);
};
