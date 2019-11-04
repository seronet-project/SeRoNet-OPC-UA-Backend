/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  \author Christian von Arnim  - ISW, University of Stuttgart
 *  \author Sebastian Friedl - ISW, University of Stuttgart
 **/

#pragma once

#if defined(_WIN32) && defined(SERONET_SDK_AS_SHARED_LIBRARY)

#ifdef SERONETSDK_BUILD_SHARED
#define SERONETSDK_EXPORT __declspec(dllexport)
#else
#define SERONETSDK_EXPORT __declspec(dllimport)
#endif
#else
#define SERONETSDK_EXPORT
#endif


