// Fill out your copyright notice in the Description page of Project Settings.

#include "DownloaderPrivatePCH.h"
#include "BPDownloader.h"
#include "Kismet/KismetStringLibrary.h"
#include "Http.h"

UBPDownloader::UBPDownloader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UBPDownloader* UBPDownloader::DownloadFile(FString URL,FString InFilePath)
{
	UBPDownloader* DownloadTask = NewObject<UBPDownloader>();
	
	DownloadTask->InitDownload(URL,InFilePath);
	return DownloadTask;
}


void UBPDownloader::InitDownload(FString URL, FString InFilePath)
{
	DownloadURL = URL;
	FilePath = InFilePath;
	QueryHeader(DownloadURL);
}

// Set DownloadURL and FilePath before calling this function.
void UBPDownloader::StartDownload()
{
	// Create the Http request and add to pending request list
	TSharedRef<class IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBPDownloader::HandleFileRequest);
	HttpRequest->OnRequestProgress().BindUObject(this, &UBPDownloader::HandleProgress);
	HttpRequest->SetURL(DownloadURL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	
}

void UBPDownloader::QueryHeader(FString URL)
{
	TSharedRef<class IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBPDownloader::HandleHeaderReceived);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("HEAD"));
	HttpRequest->ProcessRequest();
}


void UBPDownloader::HandleHeaderReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString FileLength = HttpResponse->GetHeader(L"Content-Length");// ();
		FileTotalBytes = (float)UKismetStringLibrary::Conv_StringToInt(FileLength);
		//Start Downloading
		StartDownload();
	}
	else
	{
		RemoveFromRoot();
		OnFail.Broadcast(0.f);//FilePath
	}
}


void UBPDownloader::HandleProgress(FHttpRequestPtr FHttpRequestPtr, int32 UploadProgress, int32 DownloadBytes)
{

	float progress = FileTotalBytes==0?0.f:((float)DownloadBytes / FileTotalBytes*1.0f);
	OnProgressUpdate.Broadcast(progress);
}

void UBPDownloader::HandleFileRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	RemoveFromRoot();

	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{

		TArray<uint8> data = HttpResponse->GetContent();

		if (!FFileHelper::SaveArrayToFile(data, *FilePath))
		{
			OnFail.Broadcast(0.f);
			UE_LOG(LogTemp, Warning, TEXT("Unable to save file!"));
		}
		else
		{
			OnProgressUpdate.Broadcast(1.f);
			UE_LOG(LogTemp, Warning, TEXT("File download completed!"));
		}

	}

	OnFail.Broadcast(0.f);//FilePath	
}