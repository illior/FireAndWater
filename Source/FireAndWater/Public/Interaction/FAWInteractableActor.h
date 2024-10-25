// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FAWInteractableActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FStartInteractSignature, AFAWInteractableActor, OnStartInteract, AFAWInteractableActor*, ActivatedActor, APawn*, ActivatedBy);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FStopInteractSignature, AFAWInteractableActor, OnStopInteract, AFAWInteractableActor*, ActivatedActor, APawn*, ActivatedBy);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FCompleteHoldSignature, AFAWInteractableActor, OnCompleteHold, AFAWInteractableActor*, ActivatedActor, APawn*, ActivatedBy);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FCancelHoldSignature, AFAWInteractableActor, OnCancelHold, AFAWInteractableActor*, ActivatedActor, APawn*, ActivatedBy);

class UCapsuleComponent;
class UWidgetComponent;

UCLASS()
class FIREANDWATER_API AFAWInteractableActor : public AActor
{
	GENERATED_BODY()


public:	
	AFAWInteractableActor();
	
	//~ Begin Interaction events.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FStartInteractSignature OnStartInteract;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FStopInteractSignature OnStopInteract;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FCompleteHoldSignature OnCompleteHold;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FCancelHoldSignature OnCancelHold;
	//~ End Interaction events

	//~ Begin Server functions.
	virtual void StartCanInteract(APawn* Pawn);
	virtual void StopCanInteract(APawn* Pawn);

	virtual void StartInteract();
	virtual void StopInteract();
	//~ End Server functions

	//~ Begin Client functions.
	virtual void StartHold();
	virtual void StopHold();
	//~ End Client functions

	//~ Begin RPC functions.
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Interaction")
	void Multicast_SetEnabled(bool NewValue);
	void Multicast_SetEnabled_Implementation(bool NewValue);
	//~ End RPC functions

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction", meta = (EditCondition = "ShouldHold"))
	bool ShouldKeepHold = false;

	bool CompleteHold = false;

	FTimerHandle CooldownTimer;
	FTimerHandle HoldTimer;
	FTimerHandle HideWidgetTimer;

	//~ Begin RPC functions.
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StartCanInteract(APawn* Pawn);
	void Multicast_StartCanInteract_Implementation(APawn* Pawn);
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_StopCanInteract(APawn* Pawn);
	void Multicast_StopCanInteract_Implementation(APawn* Pawn);
	//~ End RPC functions

	void Complete();

	//~ Begin Client functions.
	void ShowWidget();
	void HideWidget();

	virtual void Enable();
	virtual void Disable();
	//~ End Client functions

	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Enable"))
	void ReceiveEnable();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "Disable"))
	void ReceiveDisable();
};
