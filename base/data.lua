
data{
	file = {"brazilstates.dbf", "brazilstates.shp", "brazilstates.shx"},
	summary = "A shapefile describing the 27 Brazilian states.",
	source = "IBGE (www.ibge.br)"
}

data{
	file = "agents.csv",
	summary = "A simple set of four agents.",
	source = "TerraME team",
	attributes = {"name", "age", "wealth", "vision", "metabolism", "immune"},
	types = {"string", "number", "number", "number", "number", "boolean"},
	description = {"Name of the agents", "Age of the agent", "Amount of sugar the agent startw with", "Distance in cells the agent can see", "Energy consumed by time step", "Whether the agent is immune"}
}

data{
	file = {"cabecadeboi.mdb", "cabecadeboi.sql"},
	source = "TerraME team",
	summary = "Topography data from Cabeca de Boi, Minas Gerais, Brazil."
}

data{
	file = {"emas-distance.gal", "emas-distance.gwt", "emas-distance.gpm", "emas-pollin.gpm", "gpmdistanceDbEmasCells.gpm", "gpmlinesDbEmas.gpm"},
	summary = "Neighborhood files to be used with emas database.",
	source = "TerraME team"
}

data{
	file = {"cabecadeboi-neigh.gal", "cabecadeboi-neigh.gpm", "cabecadeboi-neigh.gwt", "gpmAreaCellsPols.gpm"},
	summary = "Neighborhood files to be used with cabecadeboi database.",
	source = "TerraME team"
}

data{
	file = {"cabecadeboi-invalid-neigh.gpm"},
	summary = "A wrong neighborhood file to be used with alternative tests within TerraME.",
	source = "TerraME team"
}

data{
	file = {"simple.map"},
	summary = "A simple CellularSpace with one attribute for sugarscape model.",
	source = "TerraME team"
}

data{
	file = {"simple-cs.csv"},
	summary = "A simple map with an attribute called maxSugar (number).",
	source = "TerraME team"
}

data{
	file = {"emas.mdb", "emas.sql"},
	source = "TerraME team",
	reference = "Almeida, Rodolfo M., et al. 'Simulando padroes de incendios no Parque Nacional das Emas, Estado de Goias, Brasil.' X Simposio Brasileiro de Geoinfoamatica (2008)",
	summary = "Land cover data on Parque Nacional das Emas, Brazil."
}

