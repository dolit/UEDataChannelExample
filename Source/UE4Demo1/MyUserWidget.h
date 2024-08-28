// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"


class UCanvasPanel;

/**
 * 
 */
UCLASS()
class UE4DEMO1_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
 
	UMyUserWidget(const FObjectInitializer& objectInitializer);
 
	virtual bool Initialize() override;
};
