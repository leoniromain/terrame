-------------------------------------------------------------------------------------------
-- TerraME - a software platform for multiple scale spatially-explicit dynamic modeling.
-- Copyright (C) 2001-2014 INPE and TerraLAB/UFOP.
--
-- This code is part of the TerraME framework.
-- This framework is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library.
--
-- The authors reassure the license terms regarding the warranties.
-- They specifically disclaim any warranties, including, but not limited to,
-- the implied warranties of merchantability and fitness for a particular purpose.
-- The framework provided hereunder is on an "as is" basis, and the authors have no
-- obligation to provide maintenance, support, updates, enhancements, or modifications.
-- In no event shall INPE and TerraLAB / UFOP be held liable to any party for direct,
-- indirect, special, incidental, or caonsequential damages arising out of the use
-- of this library and its documentation.
--
-- Authors: Pedro R. Andrade
--          Rodrigo Reis Pereira
-------------------------------------------------------------------------------------------

return{
	assert = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assert(2)
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(1, "boolean", 2))

		for i = 1, 5 do

			error_func = function()
				u:assert(false)
			end

			if i == 1 then
				unitTest:assertError(error_func, "Test should be true, got false.")
			else
				error_func()
			end
		end

		local error_func = function()
			u:assert(false)
		end

		unitTest:assertError(error_func, "[The error above occurs more 4 times.]")
	end,
	assertEquals = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertEquals()
		end

		unitTest:assertError(error_func, mandatoryArgumentMsg(1))

		local error_func = function()
			u:assertEquals(1)
		end

		unitTest:assertError(error_func, mandatoryArgumentMsg(2))

		local error_func = function()
			u:assertEquals(2, 2, "a")
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(3, "number", "a"))

		local error_func = function()
			u:assertEquals("abc", "abc", 2)
		end

		unitTest:assertError(error_func, "#3 should be used only when comparing numbers (#1 is string).")

		local error_func = function()
			u:assertEquals(2, 3)
		end

		unitTest:assertError(error_func, "Values should be equal, but got '2' and '3'.")

		local error_func = function()
			u:assertEquals("2", "3")
		end

		unitTest:assertError(error_func, "Values should be equal, but got \n'2' and \n'3'.")

		local error_func = function()
			u:assertEquals("2", 3)
		end

		unitTest:assertError(error_func, "Values should be equal, but they have different types (string and number).")
	
		local error_func = function()
			u:assertEquals(true, false)
		end

		unitTest:assertError(error_func, "Values have the same type (boolean) but different values.")
	end,
	assertError = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertError(2)
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(1, "function", 2))

		local error_func = function()
			u:assertError(function() end, 2)
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(2, "string", 2))

		local error_func = function()
			u:assertError(function() end, "aaa", false)
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(3, "number", false))
	end,
	assertNil = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertNil(2)
		end

		unitTest:assertError(error_func, "Test should be nil, got number.")
	end,
	assertNotNil = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertNotNil()
		end

		unitTest:assertError(error_func, "Test should not be nil.")
	end,
	assertSnapshot = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertSnapshot()
		end

		unitTest:assertError(error_func, "Argument #1 should be Chart, Map, TextScreen or Clock, got nil.")

		local ce = Cell{value = 5}
		local ch = Chart{target = ce}

		local error_func = function()
			u:assertSnapshot(ch)
		end

		unitTest:assertError(error_func, mandatoryArgumentMsg(2))
	end,
	assertType = function(unitTest)
		local u = UnitTest{unittest = true}

		local error_func = function()
			u:assertType()
		end

		unitTest:assertError(error_func, mandatoryArgumentMsg(1))

		local error_func = function()
			u:assertType(2, 2)
		end

		unitTest:assertError(error_func, incompatibleTypeMsg(2, "string", 2))

		local error_func = function()
			u:assertType(2, "string")
		end

		unitTest:assertError(error_func, "Test should be string got number.")
	end
}

