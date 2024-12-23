// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FAWBaseButtonWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/FAWBaseMenuWidget.h"

void UFAWBaseButtonWidget::SetState(EFAWButtonState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	EFAWButtonState OldState = CurrentState;
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EFAWButtonState::Normal:
		ButtonImage->SetBrush(NormalBrush);
		break;
	case EFAWButtonState::Hovered:
		ButtonImage->SetBrush(HoveredBrush);
		break;
	case EFAWButtonState::Pressed:
		ButtonImage->SetBrush(PressedBrush);
		break;
	case EFAWButtonState::Disabled:
		ButtonImage->SetBrush(DisabledBrush);
		break;
	}

	ReceiveStateChanged(OldState);
}

void UFAWBaseButtonWidget::SetParentMenu(UFAWBaseMenuWidget* InMenu)
{
	ParentMenu = TWeakObjectPtr<UFAWBaseMenuWidget>(InMenu);
}

void UFAWBaseButtonWidget::Click()
{
	OnClicked.Broadcast();
}

void UFAWBaseButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (ButtonText)
	{
		ButtonText->SetText(Text);
	}

	if (ButtonImage)
	{
		switch (CurrentState)
		{
		case EFAWButtonState::Normal:
			ButtonImage->SetBrush(NormalBrush);
			break;
		case EFAWButtonState::Hovered:
			ButtonImage->SetBrush(HoveredBrush);
			break;
		case EFAWButtonState::Pressed:
			ButtonImage->SetBrush(PressedBrush);
			break;
		case EFAWButtonState::Disabled:
			ButtonImage->SetBrush(DisabledBrush);
			break;
		}
	}
}

FReply UFAWBaseButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey(FName("LeftMouseButton")) && CurrentState != EFAWButtonState::Disabled)
	{
		SetState(EFAWButtonState::Pressed);
		return FReply::Handled();
	}

	OnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Unhandled();
}

FReply UFAWBaseButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey(FName("LeftMouseButton")) && CurrentState == EFAWButtonState::Pressed)
	{
		SetState(EFAWButtonState::Hovered);
		Click();
	}

	OnMouseButtonUp(InGeometry, InMouseEvent);
	return FReply::Unhandled();
}

void UFAWBaseButtonWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (CurrentState != EFAWButtonState::Disabled)
	{
		ParentMenu.IsValid() ? ParentMenu->SetSelected(this) : SetState(EFAWButtonState::Hovered);
	}

	OnMouseEnter(InGeometry, InMouseEvent);
}

void UFAWBaseButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (CurrentState != EFAWButtonState::Disabled)
	{
		SetState(EFAWButtonState::Normal);
	}

	OnMouseLeave(InMouseEvent);
}
