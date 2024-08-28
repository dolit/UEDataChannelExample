// Copyright Epic Games, Inc. All Rights Reserved.


#include "UE4Demo1GameModeBase.h"

#include "UDLCADataChannelActorComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Components/MultiLineEditableTextBox.h"


AUE4Demo1GameModeBase::AUE4Demo1GameModeBase()
{
   
}

void AUE4Demo1GameModeBase::BeginPlay()
{
   Super::BeginPlay();
   ChangeMenuWidget(StartingWidgetClass);
}

void AUE4Demo1GameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget != nullptr)
    {
        CurrentWidget->RemoveFromViewport();
        CurrentWidget = nullptr;
    }
    if (NewWidgetClass != nullptr)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
        if (CurrentWidget != nullptr)
        {
            CurrentWidget->AddToViewport();
        }
    }
}

void AUE4Demo1GameModeBase::SendText(FString text)
{

}

void AUE4Demo1GameModeBase::OnText(FString text)
{
    static FString kRotation = TEXT("旋转");
    static FString kRed = TEXT("变为红色");
    static FString kGreen = TEXT("变为绿色");
    static FString kBlue = TEXT("变为蓝色");

    if(CurrentWidget)
    {
       UMultiLineEditableTextBox* TexBox = (UMultiLineEditableTextBox*)CurrentWidget->GetWidgetFromName(TEXT("MultiLineEditableTextBox_31"));
       if(text == kRotation || text == kRed || text == kGreen || text == kBlue)
       {
            TexBox->SetText(FText::Join(FText::FromString("\r\n"),FText::FromString(FString(TEXT("序号:")) + FString::FormatAsNumber(++TextSeq) + TEXT(" 指令:") + text),TexBox->GetText()));
       }
       else
       {
            TexBox->SetText(FText::Join(FText::FromString("\r\n"),FText::FromString(FString(TEXT("序号:")) + FString::FormatAsNumber(++TextSeq) + TEXT(" 内容:") + text),TexBox->GetText()));
       } 
    }

    ULevel* Level = GetLevel();
    if(Level)
    {
        for(auto Actor : Level->Actors)
        {
             if(Actor)
             {
                 FString Name;
                 Actor->GetName(Name);
                 if(Name == TEXT("Cube_4"))
                 {
                     FLinearColor NewColor;
                     bool bSwitchColor = false;
                     if(text == kRotation)
                     {
                        Actor->AddActorLocalRotation(FQuat(FVector::UpVector,FMath::DegreesToRadians(10)));
                     }
                     else if(text == kRed)
                     {
                        NewColor = FLinearColor::Red;
                        bSwitchColor = true;
                     }
                     else if(text == kGreen)
                     {
                        NewColor = FLinearColor::Green;
                        bSwitchColor = true;
                     }
                     else if(text == kBlue)
                     {
                        NewColor = FLinearColor::Blue;
                        bSwitchColor = true;
                     }
                     else
                     {
                         //UFunction *Func = CurrentWidget->FindFunction("ShowText");
                         //if(Func)
                         //{
                         //    TTuple<> t;
                         //    InvokeFunction<>(CurrentWidget->GetClass(),CurrentWidget,Func,t,text);
                         //}
                     }
                     if(bSwitchColor)
                     {
                         UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
                         if(MeshComp)
                         {
                              if (UMaterialInterface* Mat = MeshComp->GetMaterial(0))
	                          {
                                  FString clazzName = Mat->GetClass()->GetName();
                                  if (!Mat->IsA(UMaterialInstanceDynamic::StaticClass()))
                                  {
                                        if (UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(Mat, MeshComp))
                                        {
                                            MeshComp->SetMaterial(0, MatInstance);
                                            Mat = MatInstance;
                                        }
                                  }
                                  if(UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(Mat))
                                  {
                                        DynamicMat->SetVectorParameterValue(TEXT("Color"),NewColor);
                                  }
                              }
                         }
                     }
                 }
             } 
        } 
    }
}