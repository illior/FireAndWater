// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FAWInteractWidget.generated.h"

class UWidgetAnimation;
class UImage;

UCLASS()
class FIREANDWATER_API UFAWInteractWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	float StartShow();
	float StartHide();

	void StartHold(float InHoldTime);
	void StopHold();

protected:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim), Category = "Interaction")
	UWidgetAnimation* Show;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim), Category = "Interaction")
	UWidgetAnimation* Hide;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim), Category = "Interaction")
	UWidgetAnimation* ProgressBarHide;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Interaction")
	UImage* ProgressBar;

	float HoldedTime;
	float HoldTime;

	FTimerHandle HoldTimer;

	UFUNCTION()
	void UpdateHold();

	void SetPercentage(float Percent);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction", meta = (DisplayName = "SetPercentage"))
	void ReceiveSetPercentage(float Percent);
};
