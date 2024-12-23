// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FAWBaseMenuWidget.h"
#include "FAWPauseMenuWidget.generated.h"

class UEnhancedInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FIREANDWATER_API UFAWPauseMenuWidget : public UFAWBaseMenuWidget
{
	GENERATED_BODY()


public:
	virtual void Show() override;
	virtual void Hide() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ApplyAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CancelAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	float MovementFrequency = 0.1f;

	virtual void NativeOnInitialized() override;
	virtual void SetupInput(UEnhancedInputComponent* EnhancedInputComponent);

	virtual void MoveWithFrequency(const FVector2D& Value);

	virtual void InputMove(const FInputActionValue& Value);
	virtual void InputApply(const FInputActionValue& Value);
	virtual void InputCancel(const FInputActionValue& Value);

private:
	float LastTimeMovement = 0.0f;
	float LastTimeMovementAdditive = 0.0f;
};
