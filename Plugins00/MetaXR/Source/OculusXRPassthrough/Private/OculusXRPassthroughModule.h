/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#pragma once
#include "IOculusXRPassthroughModule.h"

#define LOCTEXT_NAMESPACE "OculusXRPassthrough"

//-------------------------------------------------------------------------------------------------
// FOculusXRPassthroughModule
//-------------------------------------------------------------------------------------------------

class FOculusXRPassthroughModule : public IOculusXRPassthroughModule
{
public:
	FOculusXRPassthroughModule();

	static inline FOculusXRPassthroughModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRPassthroughModule>("OculusXRPassthrough");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#undef LOCTEXT_NAMESPACE
