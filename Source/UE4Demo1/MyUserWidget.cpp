// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& objectInitializer) :Super(objectInitializer)
{
 
}

bool UMyUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	return true;
}
