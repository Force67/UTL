-- UTL - The Universal Tool Library
-- Copyright (C) Force67 2020

-- This file provides a premake5 example

project("UTL")
    language("C++")
    kind("StaticLib")
    files({
        "./utl/*.cpp",
        "./utl/*.h"
    })
    includedirs({
        "./utl",

        -- only real hard dep:
        "../fmtlib/include"
    })