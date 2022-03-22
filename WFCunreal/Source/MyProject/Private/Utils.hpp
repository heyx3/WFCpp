#pragma once

#include "MyProject/MyProject.h"

//Short-hand for writing to the WFC logger.
#define UWFC_LOG(Verbosity, msg_str, ...) UE_LOG(LogWFC, Verbosity, TEXT(msg_str), __VA_ARGS__)

//Checks a condition, and logs an error if it's wrong.
//Does not stop execution (it seems like Unreal doesn't have a good way of doing that cleanly...)
#define UWFC_CHECK(expr, msg_str, ...) if (!(expr)) { \
	UE_LOG(LogWFC, Error, TEXT(msg_str), __VA_ARGS__); \
	if (GEngine != 0) \
		GEngine->AddOnScreenDebugMessage(0, 200.0, FColor::Red, FString("WFC error! Check the Output log.")); \
}

