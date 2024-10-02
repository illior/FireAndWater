// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FAWBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UInputAction;
struct FInputActionValue;

UCLASS()
class FIREANDWATER_API AFAWBaseCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	AFAWBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	virtual void BeginPlay() override;

private:
	void InputMove(const FInputActionValue& Value);
	void InputLook (const FInputActionValue& Value);

	void InputStartJump(const FInputActionValue& Value);
	void InputStopJump(const FInputActionValue& Value);

};
