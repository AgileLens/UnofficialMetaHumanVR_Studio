/*
Copyright (c) Meta Platforms, Inc. and affiliates.
All rights reserved.
This source code is licensed under the license found in the
LICENSE file in the root directory of this source tree.
*/

#include "OculusXRPassthroughModule.h"

#define LOCTEXT_NAMESPACE "OculusXRPassthrough"

//-------------------------------------------------------------------------------------------------
// FOculusXRPassthroughModule
//-------------------------------------------------------------------------------------------------

FOculusXRPassthroughModule::FOculusXRPassthroughModule()
{
}

void FOculusXRPassthroughModule::StartupModule()
{
}

void FOculusXRPassthroughModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FOculusXRPassthroughModule, OculusXRPassthrough)

#undef LOCTEXT_NAMESPACE
