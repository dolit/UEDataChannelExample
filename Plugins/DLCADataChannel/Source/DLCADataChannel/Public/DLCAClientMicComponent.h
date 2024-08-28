// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DLCAClientMicComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDLCAClientMicPcmFormatDelegate, FString, format);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDLCAClientMicPcmBinaryDelegate, TArray<uint8>, pcm);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DLCADATACHANNEL_API UDLCAClientMicComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category =
		"Events|DLCADataChannelApi")
		FDLCAClientMicPcmFormatDelegate ClientMicPcmFormatDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category =
		"Events|DLCADataChannelApi")
		FDLCAClientMicPcmBinaryDelegate ClientMicPcmBinaryDelegate;

public:	
	// Sets default values for this component's properties
	UDLCAClientMicComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

private:
	bool Connect();
	/**
	 * 从命令行参数中获取端口
	 */
	int GetPort();
	void DesktoryWebSocketObject();
	void ReConnect();
	void DisConnect();
	TSharedPtr<class IWebSocket> IpcSocket;
	bool StopFlag = false;
	FTimerHandle ReConnectTimerHandle;
		
};
