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

local function upperFirst(str)
	return str:gsub("^%l", string.upper)
end

_Gtme.checkFile = function(file, prefixMsg)
	local luacheck = require("luacheck.init")
	local files = {file}
	local options = {std = "min", cache = true, global = false}				
	local issues = luacheck.check_files(files, options)
	
	if (issues.errors == 0) and (issues.fatals == 0) then	
		issues = issues[1]
		for _, issue in ipairs(issues) do
			print(prefixMsg..": "..upperFirst(luacheck.get_message(issue))..". In file '"..file.."', line "..issue.line..".")
		end		
		
		return #issues
	end
	
	return 0
end

local function getLuaFiles(dirPath)
	local files = {}

	if not dirPath:exists() then return files end

	_Gtme.forEachDirectory(dirPath, function(dir)
		for _, v in ipairs(getLuaFiles(dir)) do
			table.insert(files, v)
		end			
	end)
 
	_Gtme.forEachFile(dirPath, function(file)
		if file:extension() == "lua" then
			table.insert(files, file)
		end
	end)
	
	return files
end

local function getRelativePath(full, absoluteLength)
	return string.sub(tostring(full), absoluteLength + 2)
end

_Gtme.checkPackage = function(package, packagePath)
	_Gtme.printNote("Running code analyzer for package '"..package.."'")
	local clock0 = os.clock()
	
	local testsPath = Directory(packagePath.."tests")
	local luaPath = Directory(packagePath.."lua")
	local testFiles = getLuaFiles(testsPath)
	local luaFiles = getLuaFiles(luaPath)	
	
	local srcPath
	local srcFiles = {}
	local srcPathLenght
	if package == "base" then
		srcPath = Directory(sessionInfo().path.."lua")
		srcFiles = getLuaFiles(srcPath)
		srcPathLenght = string.len(tostring(srcPath)) 
	end
	
	local luacheck = require("luacheck.init")	
	local numIssues = 0
	local pkgPathLenght = string.len(tostring(packagePath))
	local options = {std = "min", cache = true, global = false}	
	
	_Gtme.printNote("Analysing source code")
	for _, file in ipairs(luaFiles) do
		local files = {tostring(file)}
		local issues = luacheck.check_files(files, options)[1]
		for _, issue in ipairs(issues) do
			_Gtme.printError("Warning: "..upperFirst(luacheck.get_message(issue))..". In file "..getRelativePath(file, pkgPathLenght)..", line "..issue.line..".")
		end	
		numIssues = numIssues + #issues
	end

	if #srcFiles > 0 then
		for _, file in ipairs(srcFiles) do
			local files = {tostring(file)}
			local issues = luacheck.check_files(files, options)[1]
			for _, issue in ipairs(issues) do
				_Gtme.printError("Warning: "..upperFirst(luacheck.get_message(issue))..". In file "..getRelativePath(file, srcPathLenght)..", line "..issue.line..".")
			end	
			numIssues = numIssues + #issues
		end	
	end
	
	_Gtme.printNote("Analysing tests")
	for _, file in ipairs(testFiles) do
		local files = {tostring(file)}
		local issues = luacheck.check_files(files, options)[1]
		for _, issue in ipairs(issues) do
			_Gtme.printError("Warning: "..upperFirst(luacheck.get_message(issue))..". In file "..getRelativePath(file, pkgPathLenght)..", line "..issue.line..".")
		end	
		numIssues = numIssues + #issues
	end
	
	_Gtme.printNote("\nCode analyzer report for package '"..package.."':")
	
	local clock1 = os.clock()
	local dt = clock1 - clock0
	_Gtme.printNote("Analysis executed in "..round(dt, 2).." seconds.")
	
	local totalFiles = #testFiles + #luaFiles + #srcFiles
	
	if totalFiles > 0 then
		if totalFiles == 1 then
			_Gtme.printNote("One file was analysed.")
		else
			_Gtme.printNote(totalFiles.." files were analysed.")
		end
		
		if numIssues > 0 then
			if numIssues == 1 then
				_Gtme.printError("One issue was found.")
			else
				_Gtme.printWarning(numIssues.." issues were found.")
			end
		else
			_Gtme.printNote("Success, no issue found.")
		end
	else
		_Gtme.printError("No file was found.")
		numIssues = 1
	end
	
	return numIssues
end
