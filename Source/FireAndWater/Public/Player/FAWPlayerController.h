// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online.h"
#include "GameFramework/PlayerController.h"
#include "FAWPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FIREANDWATER_API AFAWPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	AFAWPlayerController();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "PlayerController")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "PlayerController")
	void PauseGame();

	void RestartWithDelay(float Delay);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MenuMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseGameAction;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

private:
	void InputPause(const FInputActionValue& Value);

};
