-------------------------------------------------------------------------------------------
-- TerraME - a software platform for multiple scale spatially-explicit dynamic modeling.
-- Copyright (C) 2001-2017 INPE and TerraLAB/UFOP -- www.terrame.org

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

return {
	Layer = function(unitTest)
		local projName = File("cellular_layer_basic.tview")

		projName:deleteIfExists()
		local proj = Project{
			file = projName:name(),
			clean = true
		}

		local layerName1 = "Sampa"

		local layer0 = Layer{
			project = proj,
			name = layerName1,
			file = filePath("test/sampa.shp", "gis")
		}

		local filePath1 = "setores_cells_basic.shp"

		if not File(filePath1):exists() then
			local mf = io.open(filePath1, "w")
			mf:write("aaa")
			io.close(mf)
		end

		local clName1 = "Sampa_Cells"

		local cl = Layer{
			project = proj,
			source = "shp",
			input = layerName1,
			name = clName1,
			clean = true,
			resolution = 0.3,
			file = filePath1
		}

		unitTest:assertEquals(projName, cl.project.file)
		unitTest:assertEquals(clName1, cl.name)

		local cl2 = Layer{
			project = projName:name(true),
			name = clName1
		}

		unitTest:assertEquals(cl2.source, "shp")
		unitTest:assertEquals(cl2.file, currentDir()..filePath1)

		projName:deleteIfExists()
		File(filePath1):deleteIfExists()

		projName = File("setores_2000.tview")

		projName:deleteIfExists()

		local proj1 = Project {
			file = projName:name(true)
		}

		local layer1 = Layer{
			project = proj1,
			name = layerName1,
			file = filePath("test/sampa.shp", "gis"),
			encoding = "utf8"
		}
		unitTest:assertEquals(layer1.name, layerName1)
		unitTest:assertEquals("utf8", layer1.encoding)
		unitTest:assertEquals("latin1", layer0.encoding)

		local proj2 = Project {
			file = projName:name(true)
		}

		local layerName2 = "MG"
		local layer2 = Layer{
			project = proj2,
			name = layerName2,
			file = filePath("test/MG_cities.shp", "gis")
		}

		unitTest:assertEquals(layer1.name, layerName1)
		unitTest:assertEquals(layer2.name, layerName2)

		local layerName21 = "MG_2"
		local layer21 = Layer{
			project = proj2,
			name = layerName21,
			file = filePath("test/MG_cities.shp", "gis")
		}

		unitTest:assert(layer21.name ~= layer2.name)
		unitTest:assertEquals(layer21.epsg, layer2.epsg)

		local layerName3 = "CBERS1"
		local layer3 = Layer{
			project = proj2,
			name = layerName3,
			file = filePath("test/cbers_rgb342_crop1.tif", "gis")
		}

		unitTest:assertEquals(layer3.name, layerName3)

		local layerName4 = "CBERS2"
		local layer4 = Layer{
			project = proj2,
			name = layerName4,
			file = filePath("test/cbers_rgb342_crop1.tif", "gis")
		}

		unitTest:assert(layer4.name ~= layer3.name)
		unitTest:assertEquals(layer4.epsg, layer3.epsg)

		projName:deleteIfExists()

		projName = File("cells_setores_2000.tview")
		proj = Project{
			file = projName:name(true),
			clean = true
		}

		layerName1 = "Sampa"
		Layer{
			project = proj,
			name = layerName1,
			file = filePath("test/sampa.shp", "gis")
		}

		layerName2 = "MG"
		Layer{
			project = proj,
			name = layerName2,
			file = filePath("test/MG_cities.shp", "gis")
		}

		layerName3 = "CBERS"
		Layer{
			project = proj,
			name = layerName3,
			file = filePath("test/cbers_rgb342_crop1.tif", "gis")
		}

		filePath1 = "sampa_cells.shp"

		File(filePath1):deleteIfExists()

		clName1 = "Sampa_Cells"
		local l1 = Layer{
			project = proj,
			input = layerName1,
			name = clName1,
			resolution = 0.7,
			file = filePath1
		}

		unitTest:assertEquals(l1.name, clName1)

		local filePath2 = "mg_cells.shp"
		local filePath3

		File(filePath2):deleteIfExists()

		local clName2 = "MG_Cells"
		local l2 = Layer{
			project = proj,
			input = layerName2,
			name = clName2,
			resolution = 1,
			file = filePath2
		}

		unitTest:assertEquals(l2.name, clName2)

		filePath3 = "another_sampa_cells.shp"

		File(filePath3):deleteIfExists()

		local clName3 = "Another_Sampa_Cells"
		local l3 = Layer{
			project = proj,
			input = layerName2,
			name = clName3,
			resolution = 0.7,
			file = filePath3
		}

		unitTest:assertEquals(l3.name, clName3)

		-- BOX TEST
		local clSet = TerraLib().getDataSet(proj, clName1)
		unitTest:assertEquals(getn(clSet), 68)

		clName1 = clName1.."_Box"
		local filePath4 = clName1..".shp"

		File(filePath4):deleteIfExists()

		Layer{
			project = proj,
			input = layerName1,
			name = clName1,
			resolution = 0.7,
			box = true,
			file = filePath4
		}

		clSet = TerraLib().getDataSet(proj, clName1)
		unitTest:assertEquals(getn(clSet), 104)

		projName:deleteIfExists()

		File(filePath1):deleteIfExists()
		File(filePath2):deleteIfExists()
		File(filePath3):deleteIfExists()
		File(filePath4):deleteIfExists()
	end,
	delete = function(unitTest)
		local projName = File("cellular_layer_del.tview")

		projName:deleteIfExists()
		local proj = Project{
			file = projName:name(),
			clean = true
		}

		local layerName1 = "Sampa"

		Layer{
			project = proj,
			name = layerName1,
			file = filePath("test/sampa.shp", "gis")
		}

		local filePath1 = "setores_cells_basic.shp"

		local clName1 = "Sampa_Cells"

		local cl = Layer{
			project = proj,
			source = "shp",
			input = layerName1,
			name = clName1,
			clean = true,
			resolution = 0.3,
			file = filePath1
		}

		cl:delete()
		projName:delete()
		unitTest:assert(not File(filePath1):exists())
	end,
	fill = function(unitTest)
		local projName = File("cellular_layer_basic.tview")
		local layerName1 = "Setores_2000"
		local localidades = "Localidades"
		local rodovias = "Rodovias"

		local proj = Project {
			file = projName,
			clean = true,
			[layerName1] = filePath("itaituba-census.shp", "gis"),
			[localidades] = filePath("test/Localidades_pt.shp", "gis"),
			[rodovias] = filePath("itaituba-roads.shp", "gis")
		}

		local clName1 = "Setores_Cells"
		local filePath1 = clName1..".shp"

		File(filePath1):deleteIfExists()

		local cl = Layer{
			project = proj,
			source = "shp",
			input = layerName1,
			name = clName1,
			resolution = 30000,
			file = filePath1
		}


		cl:fill{
			operation = "presence",
			layer = localidades,
			attribute = "presence"
		}
--[[
		local areaLayerName = clName1.."_Area"
		local filePath3 = areaLayerName..".shp"

		File(filePath3):deleteIfExists()

		cl:fill{
			operation = "area",
			layer = layerName1,
			attribute = "area"
		}
--]]

		cl:fill{
			operation = "count",
			layer = localidades,
			attribute = "num"
		}

		-- local distanceLayerName = clName1.."_Distance"
		-- local filePath5 = distanceLayerName..".shp"

		-- File(filePath5):deleteIfExists()

		-- cl:fill{
			-- operation = "distance",
			-- layer = localidades,
			-- attribute = "distance"
		-- }

		cl:fill{
			operation = "minimum",
			layer = localidades,
			attribute = "minimum",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "maximum",
			layer = localidades,
			attribute = "maximum",
			select = "UCS_FATURA"
		}

	--[[
		cl:fill{
			operation = "coverage",
			layer = localidades,
			attribute = "coverage",
			select = "LOCALIDADE"
		}
		--]]

		cl:fill{
			operation = "stdev",
			layer = localidades,
			attribute = "stdev",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "average",
			layer = localidades,
			attribute = "mean",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "average",
			layer = localidades,
			attribute = "weighted",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "mode",
			layer = localidades,
			attribute = "high_inter",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "mode",
			layer = localidades,
			attribute = "high_occur",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "sum",
			layer = localidades,
			attribute = "ucs_sum",
			select = "UCS_FATURA"
		}

		cl:fill{
			operation = "sum",
			layer = localidades,
			attribute = "wsum",
			select = "UCS_FATURA",
			area = true
		}

		-- RASTER TESTS ------------------------------------------	issue #928
		-- local desmatamento = "Desmatamento"
		-- Layer{
			-- name = desmatamento,
			-- file = filePath("itaituba-deforestation.tif", "gis")
		-- }

		-- local rmeanLayerName = clName1.."_Mean_Raster"
		-- local filePath16 = shp16 = rmeanLayerName..".shp"

		-- File(filePath16):deleteIfExists()

		-- cl:fill{
			-- operation = "average",
			-- layer = desmatamento,
			-- attribute = "mean_0",
			-- select = 0
		-- }

		-- local rminLayerName = clName1.."_Minimum_Raster"
		-- local filePath17 = rminLayerName..".shp"

		-- File(filePath17):deleteIfExists()

		-- cl:fill{
			-- operation = "minimum",
			-- layer = desmatamento,
			-- attribute = "minimum_0",
			-- select = 0
		-- }

		-- local rmaxLayerName = clName1.."_Maximum_Raster"
		-- local filePath18 = rmaxLayerName..".shp"

		-- File(filePath18):deleteIfExists()

		-- cl:fill{
			-- operation = "maximum",
			-- layer = desmatamento,
			-- attribute = "maximum_0",
			-- select = 0
		-- }

		-- local rpercentLayerName = clName1.."_Percentage_Raster"
		-- local filePath19 = rpercentLayerName..".shp"

		-- File(filePath19):deleteIfExists()

		-- cl:fill{
			-- operation = "coverage",
			-- layer = desmatamento,
			-- attribute = "percent_0",
			-- select = 0
		-- }

		-- local rstdevLayerName = clName1.."_Stdev_Raster"
		-- local filePath20 = rstdevLayerName..".shp"

		-- File(filePath20):deleteIfExists()

		-- cl:fill{
			-- operation = "stdev",
			-- layer = desmatamento,
			-- attribute = "stdev_0",
			-- select = 0
		-- }

		-- local rsumLayerName = clName1.."_Sum_Raster"
		-- local filePath21 = rstdevLayerName..".shp"

		-- File(filePath21):deleteIfExists()

		-- cl:fill{
			-- operation = "sum",
			-- layer = desmatamento,
			-- attribute = "sum_0",
			-- select = 0
		-- }

		local cs = CellularSpace{
			project = proj,
			layer = cl.name
		}

		forEachCell(cs, function(cell)
			cell.past_ucs_sum = cell.ucs_sum
			cell.ucs_sum = cell.ucs_sum + 10000
		end)

		local cellSpaceLayerName = clName1.."_CellSpace_Sum"
		local filePath22 = cellSpaceLayerName..".shp"

		File(filePath22):deleteIfExists()

		cs:save(cellSpaceLayerName, "past_ucs_sum")

		local cellSpaceLayer = Layer{
			project = proj,
			name = cellSpaceLayerName
		}

		unitTest:assertEquals(cellSpaceLayer.source, "shp")
		unitTest:assertEquals(cellSpaceLayer.file, currentDir()..filePath22)

		projName:deleteIfExists()

		File(filePath1):deleteIfExists()
--		File(filePath3):deleteIfExists()
--		File(filePath5):deleteIfExists()
		--File(filePath16):deleteIfExists()
		--File(filePath17):deleteIfExists()
		--File(filePath18):deleteIfExists()
		--File(filePath19):deleteIfExists()
		--File(filePath20):deleteIfExists()
		--File(filePath21):deleteIfExists()
		File(filePath22):deleteIfExists()

		projName:deleteIfExists()
	end,
	representation = function(unitTest)
		local projName = "cellular_layer_representation.tview"

		local proj = Project {
			file = projName,
			clean = true
		}

		local layerName1 = "Setores_2000"
		local l = Layer{
			project = proj,
			name = layerName1,
			file = filePath("itaituba-census.shp", "gis")
		}

		unitTest:assertEquals(l:representation(), "polygon")

		local localidades = "Localidades"
		l = Layer{
			project = proj,
			name = localidades,
			file = filePath("itaituba-localities.shp", "gis")
		}

		unitTest:assertEquals(l:representation(), "point")

		local rodovias = "Rodovias"
		l = Layer{
			project = proj,
			name = rodovias,
			file = filePath("itaituba-roads.shp", "gis")
		}

		unitTest:assertEquals(l:representation(), "line")

		proj.file:deleteIfExists()
	end,
	__tostring = function(unitTest)
		local projName = File("cellular_layer_print.tview")

		local proj = Project {
			file = projName:name(true),
			clean = true
		}

		local layerName1 = "Setores_2000"
		local l = Layer{
			project = proj,
			name = layerName1,
			file = filePath("itaituba-census.shp", "gis")
		}

		local expected = [[
encoding  string [latin1]
epsg      number [29191]
file      string [itaituba-census.shp]
name      string [Setores_2000]
project   Project
rep       string [polygon]
source    string [shp]
]]
		unitTest:assertEquals(tostring(l), expected, 0, true)
		projName:deleteIfExists()
	end
}
