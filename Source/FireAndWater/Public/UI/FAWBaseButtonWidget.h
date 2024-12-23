// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FAWBaseButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonClickSignature);

class UImage;
class UTextBlock;
class UFAWBaseMenuWidget;

UENUM(BlueprintType)
enum class EFAWButtonState : uint8
{
	Normal,
	Hovered,
	Pressed,
	Disabled
};

UCLASS()
class FIREANDWATER_API UFAWBaseButtonWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FButtonClickSignature OnClicked;

	UFUNCTION(BlueprintCallable, Category = "Button")
	virtual void SetState(EFAWButtonState NewState);
	EFAWButtonState GetState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Button")
	void SetParentMenu(UFAWBaseMenuWidget* InMenu);
	UFUNCTION(BlueprintCallable, Category = "Button")
	UFAWBaseMenuWidget* GetParentMenu() const { return ParentMenu.Get(); };

	UFUNCTION(BlueprintCallable, Category = "Button")
	virtual void Click();

	UFUNCTION(BlueprintImplementableEvent, Category = "Button", meta = (DisplayName = "StateChanged"))
	void ReceiveStateChanged(EFAWButtonState OldState);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Button")
	TObjectPtr<UImage> ButtonImage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Button")
	TObjectPtr<UTextBlock> ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush NormalBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush HoveredBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush PressedBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush DisabledBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	FText Text = FText::FromString(TEXT("ButtonText"));

	UPROPERTY(BlueprintReadOnly, Category = "Button")
	EFAWButtonState CurrentState = EFAWButtonState::Normal;

	TWeakObjectPtr<UFAWBaseMenuWidget> ParentMenu;

	virtual void NativePreConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
