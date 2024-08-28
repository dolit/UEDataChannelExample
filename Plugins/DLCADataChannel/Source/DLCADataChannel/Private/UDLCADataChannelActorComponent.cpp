// Fill out your copyright notice in the Description page of Project Settings. 
#include "UDLCADataChannelActorComponent.h"

#include <string>

#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

// Sets default values for this component's properties
UDLCADataChannelActorComponent::UDLCADataChannelActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame. You can turn these features
	// off to improve performance if you don't need them. 
	PrimaryComponentTick.bCanEverTick = false;
	UE_LOG(LogTemp, Display, TEXT("dlca data channel create"));
}

// Called when the game starts
void UDLCADataChannelActorComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("dlca data channel begin play"));
	// ... 
	FModuleManager::Get().LoadModuleChecked("WebSockets");
}

void UDLCADataChannelActorComponent::BeginDestroy() {
	Super::BeginDestroy();
	StopFlag = true;

	DesktoryWebSocketObject();
}

bool UDLCADataChannelActorComponent::Connect()
{
	StopFlag = false;
	const FString ServerURL = FString::Printf(TEXT("ws://127.0.0.1:%d/inner/data_channel?t=123&sign=xMDVFKx5pH5Eeu9NpGVQGA=="),GetPort()); // Your server URL. You can use ws, wss or wss+insecure.
	UE_LOG(LogTemp, Warning, TEXT("dlca data channel websocket begin connect %s"),*ServerURL);
	const FString ServerProtocol = TEXT("ws");              // The WebServer protocol you want to use.
	IpcSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	// We bind all available events
	IpcSocket->OnConnected().AddLambda([=]() -> void {
		// This code will run once connected.
		UE_LOG(LogTemp, Display, TEXT("dlca data channel conencted!"));
		MuitDynamicDataChannelConnectStatusChanged.Broadcast(true);
	});

	IpcSocket->OnConnectionError().AddLambda([=](const FString& Error) -> void {
		// This code will run if the connection failed. Check Error to see what happened.
		UE_LOG(LogTemp, Warning, TEXT("dlca data channel connect error! Error:%s"),*Error);
		MuitDynamicDataChannelConnectStatusChanged.Broadcast(false);

		GetWorld()->GetTimerManager().SetTimer(ReConnectTimerHandle,this,&UDLCADataChannelActorComponent::ReConnect,1.0f,false,1.0f);
	});
	
	IpcSocket->OnClosed().AddLambda([=](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().
		UE_LOG(LogTemp, Warning, TEXT("dlca data channel on closed! StatusCode %d Reason:%s bWasClean:%d"),StatusCode,*Reason,bWasClean);
		MuitDynamicDataChannelConnectStatusChanged.Broadcast(false);

		GetWorld()->GetTimerManager().SetTimer(ReConnectTimerHandle,this,&UDLCADataChannelActorComponent::ReConnect,1.0f,false,1.0f);
	});

	IpcSocket->OnMessage().AddLambda([=](const FString& Message) -> void {
		// This code will run when we receive a string message from the server.
		MuitDynamicTextDelegate.Broadcast(Message);
	});

	IpcSocket->OnRawMessage().AddLambda([=](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		// This code will run when we receive a raw (binary) message from the server.
		TArray<uint8> Binray((uint8*)Data,Size);
		MuitDynamicBinrayDelegate.Broadcast(Binray);
	});


	// And we finally connect to the server. 
	IpcSocket->Connect();
	return true;
}

bool UDLCADataChannelActorComponent::SendText(FString SendData) {
	if(IpcSocket && IpcSocket->IsConnected()) {
		IpcSocket->Send(SendData);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("dlca data channel SendText failed."));
	return false;
}

bool UDLCADataChannelActorComponent::SendBinary(TArray<uint8> SendData)
{
	if(IpcSocket && IpcSocket->IsConnected()) {
		IpcSocket->Send(SendData.GetData(),SendData.Num(),true);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("dlca data channel SendBinary failed."));
	return false;
}

void UDLCADataChannelActorComponent::DisConnect()
{
	StopFlag = true;
	DesktoryWebSocketObject();
}

int UDLCADataChannelActorComponent::GetPort()
{
	FString Param;
	FParse::Value(FCommandLine::Get(), TEXT("-DLCAPort="), Param);
	return FCString::Atoi(*Param);
}

void UDLCADataChannelActorComponent::DesktoryWebSocketObject() {
	if(IpcSocket) {
		if(IpcSocket->IsConnected()) {
			IpcSocket->Close();
		}
		IpcSocket->OnConnected().Clear();
		IpcSocket->OnConnectionError().Clear();
		IpcSocket->OnClosed().Clear();
		IpcSocket->OnMessage().Clear();
		IpcSocket->OnRawMessage().Clear();
		IpcSocket = nullptr;
	}
}

void UDLCADataChannelActorComponent::ReConnect() {
	if (!StopFlag) {
		IpcSocket->Connect();
	}
}

void UDLCADataChannelActorComponent::GetJsonStringValue(FString Descriptor, FString FieldName, FString& StringValue, bool& Success)
{
	/*
	 * ExtractJsonFromDescriptor supports parsing nested objects, as well as keys that contain a period.
	 * eg
	 * { "Encoder.MinQP": val }
	 * vs
	 * { "Encoder": { "MinQP": val }}
	 */
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		const TSharedPtr<FJsonObject>* JsonObjectPtr = &JsonObject;

		// Create a copy of our field name
		FString OriginalFieldName = FieldName;
		if (FieldName.Contains(TEXT(".")))
		{
			TArray<FString> FieldComponents;
			FieldName.ParseIntoArray(FieldComponents, TEXT("."));
			FieldName = FieldComponents.Pop();

			for (const FString& FieldComponent : FieldComponents)
			{
				if (!(*JsonObjectPtr)->TryGetObjectField(FieldComponent, JsonObjectPtr))
				{
					// If we can't get the nested object, try parse the key as a whole just incase the key contains a period
					Success = (*JsonObjectPtr)->TryGetStringField(OriginalFieldName, StringValue);
					return;
				}
			}
		}

		Success = (*JsonObjectPtr)->TryGetStringField(FieldName, StringValue);
	}
	else
	{
		Success = false;
	}
}

void UDLCADataChannelActorComponent::AddJsonStringValue(const FString& Descriptor, FString FieldName, FString StringValue, FString& NewDescriptor, bool& Success) 
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!Descriptor.IsEmpty())
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
		{
			Success = false;
			return;
		}
	}

	TSharedRef<FJsonValueString> JsonValueObject = MakeShareable(new FJsonValueString(StringValue));
	JsonObject->SetField(FieldName, JsonValueObject);

	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&NewDescriptor);
	Success = FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
}