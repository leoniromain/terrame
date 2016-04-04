-------------------------------------------------------------------------------------------
-- TerraME - a software platform for multiple scale spatially-explicit dynamic modeling.
-- Copyright (C) 2001-2016 INPE and TerraLAB/UFOP -- www.terrame.org

-- This code is part of the TerraME framework.
-- This framework is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.

-- You should have received a copy of the GNU Lesser General Public
-- License along with this library.

-- The authors reassure the license terms regarding the warranties.
-- They specifically disclaim any warranties, including, but not limited to,
-- the implied warranties of merchantability and fitness for a particular purpose.
-- The framework provided hereunder is on an "as is" basis, and the authors have no
-- obligation to provide maintenance, support, updates, enhancements, or modifications.
-- In no event shall INPE and TerraLAB / UFOP be held liable to any party for direct,
-- indirect, special, incidental, or consequential damages arising out of the use
-- of this software and its documentation.
--
-------------------------------------------------------------------------------------------

return{
	filesByExtension = function(unitTest)
		local error_func = function()
			local files = filesByExtension()
		end
		unitTest:assertError(error_func, mandatoryArgumentMsg(1))

		local error_func = function()
			local files = filesByExtension(2)
		end
		unitTest:assertError(error_func, incompatibleTypeMsg(1, "string", 2))
	
		local error_func = function()
			local files = filesByExtension("base")
		end
		unitTest:assertError(error_func, mandatoryArgumentMsg(2))

		local error_func = function()
			local files = filesByExtension("base", 2)
		end
		unitTest:assertError(error_func, incompatibleTypeMsg(2, "string", 2))
	end,
	import = function(unitTest)
		local error_func = function()
			import()
		end
		unitTest:assertError(error_func, mandatoryArgumentMsg(1))

		error_func = function()
			import("asdfgh")
		end
		unitTest:assertError(error_func, "Package 'asdfgh' is not installed.")

		local error_func = function()
			import("base")
		end
		unitTest:assertError(error_func, "Package 'base' is already loaded.")
	end,
	isLoaded = function(unitTest)
		local error_func = function()
			isLoaded()
		end
		unitTest:assertError(error_func, mandatoryArgumentMsg(1))
	end,
	getPackage = function(unitTest)
		local error_func = function()
			getPackage()
		end
		unitTest:assertError(error_func, mandatoryArgumentMsg(1))

		error_func = function()
			getPackage("asdfgh")
		end
		unitTest:assertError(error_func, "Package 'asdfgh' is not installed.")
	end,
	packageInfo = function(unitTest)
		local error_func = function()
			local r = packageInfo(2)
		end
		unitTest:assertError(error_func, incompatibleTypeMsg(1, "string", 2))
	
		error_func = function()
			local r = packageInfo("asdfgh")
		end
		unitTest:assertError(error_func, "Package 'asdfgh' is not installed.")
	end
}
