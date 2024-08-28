// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "UE4Demo1GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE4DEMO1_API AUE4Demo1GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
    AUE4Demo1GameModeBase();
    /** Remove the current menu widget and create a new one from the specified class, if provided. */
    UFUNCTION(BlueprintCallable, Category = "UMG Game")
    void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

    UFUNCTION(BlueprintCallable, Category = "UMG Game")
    void SendText(FString text);
    
    UFUNCTION(BlueprintCallable, Category = "UMG Game")
    void OnText(FString text);

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

    /** The widget class we will use as our menu when the game starts. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
    TSubclassOf<UUserWidget> StartingWidgetClass;

    /** The widget instance that we are using as our menu. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
    UUserWidget* CurrentWidget;

    UPROPERTY()
    class UDLCADataChannelActorComponent* DataChannelComponent;

    int TextSeq = 0;
};
