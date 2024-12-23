// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FAWBaseMenuWidget.generated.h"

class UFAWBaseButtonWidget;

UCLASS()
class FIREANDWATER_API UFAWBaseMenuWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void AddButton(UFAWBaseButtonWidget* InButton);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	virtual void Show();
	UFUNCTION(BlueprintCallable, Category = "Menu")
	virtual void Hide();

	void HideSubMenu();

	void SetSelected(UFAWBaseButtonWidget* InButton);

	virtual void Apply();
	virtual void Cancel();
	virtual void MoveVertical(const int32 Value);
	virtual void MoveHorizontal(const int32 Value);

	void SetSubMenu(UFAWBaseMenuWidget* InSubMenu);
	UFAWBaseMenuWidget* GetSubMenu() const { return SubMenu.Get(); };

	void SetSupMenu(UFAWBaseMenuWidget* InSupMenu);
	UFAWBaseMenuWidget* GetSupMenu() const { return SupMenu.Get(); };

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	TArray<UFAWBaseButtonWidget*> Buttons;

	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	TObjectPtr<UFAWBaseMenuWidget> SubMenu;
	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	TObjectPtr<UFAWBaseMenuWidget> SupMenu;

	int32 CurrentIndex = -1;

	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu", meta = (DisplayName = "OnShow"))
	void ReceiveShow();
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu", meta = (DisplayName = "OnHide"))
	void ReceiveHide();
};
