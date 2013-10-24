/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008-2012, Markus Schütz
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////
//
// Add some new macros to the gtest (google test) testing framework.
//
///////////////////////////////////////////////////////////////////////////////
*/
#pragma once
#include <gtest/gtest.h>

// Assert pointer 'p' is null
#define ASSERT_NU(p) ASSERT_TRUE(p == 0)

// Assert pointer 'p' is not null
#define ASSERT_NN(p) ASSERT_TRUE(p != 0)

// Expect pointer 'p' is null
#define EXPECT_NU(p) EXPECT_TRUE(p == 0)

// Expect pointer 'p' is not null
#define EXPECT_NN(p) EXPECT_TRUE(p != 0)
