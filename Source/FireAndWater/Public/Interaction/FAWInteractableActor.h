// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FAWInteractableActor.generated.h"

class UCapsuleComponent;
class UWidgetComponent;

UCLASS()
class FIREANDWATER_API AFAWInteractableActor : public AActor
{
	GENERATED_BODY()


public:	
	AFAWInteractableActor();

	virtual void StartCanInteract(APawn* Pawn);
	virtual void StopCanInteract(APawn* Pawn);

	virtual void StartInteract();
	virtual void StopInteract();

	virtual void StartHold();
	virtual void StopHold();

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Interaction")
	void Multicast_SetEnabled(bool NewValue);

	bool GetIsEnabled() const { return IsEnabled; };
	APawn* GetInteractedPawn() const { return InteractedPawn.Get(); };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCapsuleComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<APawn> InteractedPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool IsEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	bool IsReusable = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "IsReusable"))
	float CooldownTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction")
	bool ShouldHold = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "ShouldHold"))
	float HoldTime = 3.0f;

	FTimerHandle CooldownTimer;
	FTimerHandle HoldTimer;
	FTimerHandle HideWidgetTimer;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StartCanInteract(APawn* Pawn);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StopCanInteract(APawn* Pawn);

	UFUNCTION()
	void Complete();

	void ShowWidget();
	void HideWidget();

	virtual void Enable();
	virtual void Disable();

	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "StartInteract"))
	void ReceiveStartInteract();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "StopInteract"))
	void ReceiveStopInteract();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Enable"))
	void ReceiveEnable();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Disable"))
	void ReceiveDisable();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Complete"))
	void ReceiveComplete();
};
