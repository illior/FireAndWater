// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FAWAnimInstance.generated.h"

UCLASS()
class FIREANDWATER_API UFAWAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterData")
	float GroundDistance = -1.0f;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
