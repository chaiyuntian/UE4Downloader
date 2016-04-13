// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BPDownloader.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDownloadFileDelegate, float, CurrentProgress); 

UCLASS()
class DOWNLOADER_API UBPDownloader :public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	//GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UBPDownloader* DownloadFile(FString URL, FString LocalFilePath);

public:

	UPROPERTY(BlueprintAssignable)
		FDownloadFileDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FDownloadFileDelegate OnFail;


	UPROPERTY(BlueprintAssignable)
		FDownloadFileDelegate OnProgressUpdate;

public:

	void InitDownload(FString URL, FString LocalFilePath);

	void QueryHeader(FString URL);

private:

	FString FilePath;

	FString DownloadURL;

	float FileTotalBytes;

	FString TSF_Encoding;

	void StartDownload();

	void HandleFileRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void HandleProgress(FHttpRequestPtr FHttpRequestPtr, int32 UploadProgress, int32 DownloadProgress);

	void HandleHeaderReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

};
