// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCAClientMicComponent.h"

#include <string>

#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

// Sets default values for this component's properties
UDLCAClientMicComponent::UDLCAClientMicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UDLCAClientMicComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	FModuleManager::Get().LoadModuleChecked("WebSockets");

	if(GetPort())
		Connect();
	else
		UE_LOG(LogTemp, Warning, TEXT("pull mic websocket begin connect failed. port is 0, plase check enable micphone module from cms"));
}

void UDLCAClientMicComponent::BeginDestroy() {
	Super::BeginDestroy();
	StopFlag = true;

	DesktoryWebSocketObject();
}

bool UDLCAClientMicComponent::Connect()
{
	StopFlag = false;
	const FString ServerURL = FString::Printf(TEXT("ws://127.0.0.1:%d/pull_mic?t=123&sign=xMDVFKx5pH5Eeu9NpGVQGA=="),GetPort()); // Your server URL. You can use ws, wss or wss+insecure.
	UE_LOG(LogTemp, Warning, TEXT("pull mic websocket begin connect %s"),*ServerURL);
	const FString ServerProtocol = TEXT("ws");              // The WebServer protocol you want to use.
	IpcSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	// We bind all available events
	IpcSocket->OnConnected().AddLambda([=]() -> void {
		// This code will run once connected.
		UE_LOG(LogTemp, Display, TEXT("pull mic websocket conencted!"));
	});

	IpcSocket->OnConnectionError().AddLambda([=](const FString& Error) -> void {
		// This code will run if the connection failed. Check Error to see what happened.
		UE_LOG(LogTemp, Warning, TEXT("pull mic websocket connect error! Error:%s"),*Error);

		GetWorld()->GetTimerManager().SetTimer(ReConnectTimerHandle,this,&UDLCAClientMicComponent::ReConnect,1.0f,false,1.0f);
	});
	
	IpcSocket->OnClosed().AddLambda([=](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().
		UE_LOG(LogTemp, Warning, TEXT("pull mic websocket on closed! StatusCode %d Reason:%s bWasClean:%d"),StatusCode,*Reason,bWasClean);

		GetWorld()->GetTimerManager().SetTimer(ReConnectTimerHandle,this,&UDLCAClientMicComponent::ReConnect,1.0f,false,1.0f);
	});

	IpcSocket->OnMessage().AddLambda([=](const FString& Message) -> void {
		// This code will run when we receive a string message from the server.
		ClientMicPcmFormatDelegate.Broadcast(Message);
	});

	IpcSocket->OnRawMessage().AddLambda([=](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		// This code will run when we receive a raw (binary) message from the server.
		TArray<uint8> Binray((uint8*)Data,Size);
		ClientMicPcmBinaryDelegate.Broadcast(Binray);
	});

	// And we finally connect to the server. 
	IpcSocket->Connect();
	return true;
}

void UDLCAClientMicComponent::DisConnect()
{
	StopFlag = true;
	DesktoryWebSocketObject();
}

int UDLCAClientMicComponent::GetPort()
{
	FString Param;
	FParse::Value(FCommandLine::Get(), TEXT("-DLCAPort="), Param);
	return FCString::Atoi(*Param);
}

void UDLCAClientMicComponent::DesktoryWebSocketObject() {
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

void UDLCAClientMicComponent::ReConnect() {
	if (!StopFlag) {
		IpcSocket->Connect();
	}
}