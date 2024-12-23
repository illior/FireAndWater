// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FAWBaseMenuWidget.h"

#include "UI/FAWBaseButtonWidget.h"
#include "UI/FAWSlideButton.h"

void UFAWBaseMenuWidget::AddButton(UFAWBaseButtonWidget* InButton)
{
	if (InButton == nullptr || Buttons.Contains(InButton))
	{
		return;
	}

	Buttons.Add(InButton);
	InButton->SetParentMenu(this);
}

void UFAWBaseMenuWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
	ReceiveShow();
}

void UFAWBaseMenuWidget::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
	ReceiveHide();
}

void UFAWBaseMenuWidget::HideSubMenu()
{
	SubMenu->Hide();
	SubMenu = TObjectPtr<UFAWBaseMenuWidget>();

	Show();
}

void UFAWBaseMenuWidget::SetSelected(UFAWBaseButtonWidget* InButton)
{
	if (!Buttons.Contains(InButton))
	{
		return;
	}

	if (Buttons.IsValidIndex(CurrentIndex))
	{
		Buttons[CurrentIndex]->SetState(EFAWButtonState::Normal);
	}

	CurrentIndex = Buttons.Find(InButton);
	if (CurrentIndex != INDEX_NONE)
	{
		Buttons[CurrentIndex]->SetState(EFAWButtonState::Hovered);
	}
}

void UFAWBaseMenuWidget::Apply()
{
	if (SubMenu)
	{
		SubMenu->Apply();
		return;
	}

	if (Buttons.IsValidIndex(CurrentIndex))
	{
		Buttons[CurrentIndex]->Click();
	}
}

void UFAWBaseMenuWidget::Cancel()
{
	if (SubMenu)
	{
		SubMenu->Cancel();
		return;
	}

	if (SupMenu)
	{
		SupMenu->HideSubMenu();
	}
}

void UFAWBaseMenuWidget::MoveVertical(const int32 Value)
{
	if (SubMenu)
	{
		SubMenu->MoveVertical(Value);
		return;
	}

	if (Buttons.IsValidIndex(CurrentIndex))
	{
		Buttons[CurrentIndex]->SetState(EFAWButtonState::Normal);
	}

	for (int32 NextIndex = CurrentIndex + Value; NextIndex > -1 && NextIndex < Buttons.Num(); NextIndex += Value)
	{
		if (Buttons[NextIndex]->GetState() != EFAWButtonState::Disabled)
		{
			CurrentIndex = NextIndex;
			Buttons[CurrentIndex]->SetState(EFAWButtonState::Hovered);
			return;
		}
	}

	if (Buttons.IsValidIndex(CurrentIndex))
	{
		Buttons[CurrentIndex]->SetState(EFAWButtonState::Hovered);
	}
}

void UFAWBaseMenuWidget::MoveHorizontal(const int32 Value)
{
	if (SubMenu)
	{
		SubMenu->MoveHorizontal(Value);
		return;
	}

	if (Buttons.IsValidIndex(CurrentIndex) && Buttons[CurrentIndex]->Implements<UFAWSlideButton>())
	{
		IFAWSlideButton::Execute_MoveHorizontal(Buttons[CurrentIndex], Value);
	}
}

void UFAWBaseMenuWidget::SetSubMenu(UFAWBaseMenuWidget* InSubMenu)
{
	SubMenu = TObjectPtr<UFAWBaseMenuWidget>(InSubMenu);
}

void UFAWBaseMenuWidget::SetSupMenu(UFAWBaseMenuWidget* InSupMenu)
{
	SupMenu = TObjectPtr<UFAWBaseMenuWidget>(InSupMenu);
}

void UFAWBaseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
