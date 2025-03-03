#pragma once
#include "CoreMinimal.h" 
#include "Components/ActorComponent.h" 
#include "UDLCADataChannelActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMuitDynamicDataChannelTextDelegate, FString, dataText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMuitDynamicDataChannelBinrayDelegate, TArray<uint8>, dataText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMuitDynamicDataChannelConnectStatusChanged, bool, connected);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DLCADATACHANNEL_API UDLCADataChannelActorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UDLCADataChannelActorComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category =
		"Events|DLCADataChannelApi")
		FMuitDynamicDataChannelTextDelegate MuitDynamicTextDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category =
		"Events|DLCADataChannelApi")
		FMuitDynamicDataChannelBinrayDelegate MuitDynamicBinrayDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category =
		"Events|DLCADataChannelApi")
		FMuitDynamicDataChannelConnectStatusChanged MuitDynamicDataChannelConnectStatusChanged;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
public:
	/**
	 * Connect 连接DLCA云流化服务,请确保云流化服务已启动，否则会连接失败。
	 */
	UFUNCTION(BlueprintCallable, Category = "DLCADataChannelApi")
	bool Connect();

	/**
	 * 发送字符串
	 */
	UFUNCTION(BlueprintCallable, Category = "DLCADataChannelApi")
	bool SendText(FString SendData);

	/**
	 * 发送二进制数据。
	 */
	UFUNCTION(BlueprintCallable, Category = "DLCADataChannelApi")
	bool SendBinary(TArray<uint8> SendData);

	/**
	 * 结束调用 dlca_data_channel_disconenct
	 */
	UFUNCTION(BlueprintCallable, Category = "DLCADataChannelApi")
	void DisConnect();

	/**
	 * Helper function to extract a string field from a JSON descriptor of a
	 * UI interaction given its field name.
	 * The field name may be hierarchical, delimited by a period. For example,
	 * to access the Width value of a Resolution command above you should use
	 * "Resolution.Width" to get the width value.
	 * @param Descriptor - The UI interaction JSON descriptor.
	 * @param FieldName - The name of the field to look for in the JSON.
	 * @param StringValue - The string value associated with the field name.
	 * @param Success - True if the field exists in the JSON data.
	 */
	UFUNCTION(BlueprintPure, Category = "DLCADataChannelApi")
	static void GetJsonStringValue(FString Descriptor, FString FieldName, FString& StringValue, bool& Success);

	/**
	 * Helper function to add a string field to a JSON descriptor. This produces
	 * a new descriptor which may then be chained to add further string fields.
	 * @param Descriptor - The initial JSON descriptor which may be blank initially.
	 * @param FieldName - The name of the field to add to the JSON.
	 * @param StringValue - The string value associated with the field name.
	 * @param NewDescriptor - The JSON descriptor with the string field added.
	 * @param Success - True if the string field could be added successfully.
	 */
	UFUNCTION(BlueprintPure, Category = "DLCADataChannelApi")
	static void AddJsonStringValue(const FString& Descriptor, FString FieldName, FString StringValue, FString& NewDescriptor, bool& Success);
private:
	/**
	 * 从命令行参数中获取端口
	 */
	int GetPort();
	void DesktoryWebSocketObject();
	void ReConnect();
	TSharedPtr<class IWebSocket> IpcSocket;
	bool StopFlag = false;
	FTimerHandle ReConnectTimerHandle;
};