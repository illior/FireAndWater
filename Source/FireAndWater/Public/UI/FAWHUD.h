// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FAWHUD.generated.h"

class UFAWPauseMenuWidget;

UCLASS()
class FIREANDWATER_API AFAWHUD : public AHUD
{
	GENERATED_BODY()


public:
	void OpenPauseMenu();
	void ClosePauseMenu();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFAWPauseMenuWidget> PauseWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UFAWPauseMenuWidget> PauseWidget;
};
