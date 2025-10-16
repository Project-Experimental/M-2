// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//
// Provides std::initializer_list<T> on behalf of the Magenta toolchain
// when compiling without the C++ standard library.
//

#pragma once

#include <initializer_list>

namespace m2l
{

using std::initializer_list;

};