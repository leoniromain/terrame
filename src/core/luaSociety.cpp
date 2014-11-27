/************************************************************************************
TerraLib - a library for developing GIS applications.
Copyright 2001-2007 INPE and Tecgraf/PUC-Rio.

This code is part of the TerraLib library.
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The library provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements, or modifications.
In no event shall INPE and Tecgraf / PUC-Rio be held liable to any party for direct,
indirect, special, incidental, or consequential damages arising out of the use
of this library and its documentation.
*************************************************************************************/
/*! \file luaSociety.cpp
	\brief This file contains the implementation for the luaSociety objects.
		\author Tiago Garcia de Senna Carneiro
*/

#include "luaSociety.h"

#include "observerTextScreen.h"
#include "observerGraphic.h"
#include "observerLogFile.h"
#include "observerTable.h"
#include "observerUDPSender.h"
#include "agentObserverMap.h"
#include "agentObserverImage.h"
#include "observerLua.h"
#include "luaUtils.h"
#include "terrameGlobals.h"

// File automatically generated by Protocol Buffers
#include "protocol.pb.h"

#define TME_STATISTIC_UNDEF

#ifdef TME_STATISTIC
	// Estatisticas de desempenho
	#include "statistic.h"
#endif

///< Gobal variabel: Lua stack used for comunication with C++ modules.
extern lua_State * L;
extern ExecutionModes execModes;

/// Constructor
luaSociety::luaSociety(lua_State *L)
{
	luaL = L;
	subjectType = TObsSociety;
	observedAttribs.clear();

	attrNeighName = "";
}

/// destructor
luaSociety::~luaSociety( void ) { }

/// Gets the luaSociety identifier
int luaSociety::getID( lua_State *L )
{
	lua_pushstring(L, objectId_.c_str() );
	return 1;
}

/// Sets the luaSociety identifier
int luaSociety::setID( lua_State *L )
{
	const char* id = luaL_checkstring( L , -1);
	objectId_ = string( id );
	return 0;
}

/// Creates several types of observers
/// parameters: observer type, observeb attributes table, observer type parameters
// verif. ref (endereco na pilha lua)
// olhar a classe event
int luaSociety::createObserver( lua_State * luaL)
{

#ifdef DEBUG_OBSERVER
	luaStackToQString(7);
#endif

	// retrieve Lua object reference
	Reference<luaSociety>::getReference(luaL);

	// flags para a definicao do uso de compressao
	// na transmissao de datagramas e da visibilidade
	// dos observadores Udp Sender
	bool compressDatagram = false, obsVisible = true;

	// recupero a tabela de
	// atributos da celula
	int top = lua_gettop(luaL);

	// Nao modifica em nada a pilha
	// recupera o enum referente ao tipo
	// do observer
	TypesOfObservers typeObserver = (TypesOfObservers)luaL_checkinteger(luaL, 1);

	if ((typeObserver !=  TObsMap) && (typeObserver !=  TObsImage))
	{
		QStringList allAgentsAttribs, allSocietyAttribs, obsAttribs, obsParams, cols;
		
#ifdef DEBUG_OBSERVER
		luaStackToQString(12);
		stackDump(luaL);
#endif
		
		// Transverse the society and retrieve all its attribs and the attribs of the agents
		lua_pushnil(luaL);
		while(lua_next(luaL, top) != 0)
		{
			if (lua_type(luaL, -2) == LUA_TSTRING)
			{
				QString key = luaL_checkstring(luaL, -2);
				allSocietyAttribs.append(key);
				
				if (key == "agents")
				{
					int agentstop = lua_gettop(luaL);
					int stop = false;
					
					lua_pushnil(luaL);
					while ((! stop) && (lua_next(luaL, agentstop) != 0))
					{
						int agentTop = lua_gettop(luaL);
						lua_pushnumber(luaL, 1);
						lua_gettable(luaL, agentTop);
						
						lua_pushnil(luaL);
						while(lua_next(luaL, agentTop) != 0)
						{
							if (lua_type(luaL, -2) == LUA_TSTRING)
								allAgentsAttribs.append(luaL_checkstring(luaL, -2));
							stop = true;
							lua_pop(luaL, 1);
						}
						lua_pop(luaL, 1); // lua_pushnumber/lua_pushstring
						lua_pop(luaL, 1); // lua_pushnil
						lua_pop(luaL, 1); // breaks the loop
					}
				} // (key == "agents")
			} // lua_type == LUA_TSTRING
			lua_pop(luaL, 1);
		}
		
#ifdef DEBUG_OBSERVER
		qDebug() << "allSocietyAttribs: " << allSocietyAttribs;
		qDebug() << "allAgentsAttribs: " << allAgentsAttribs;
#endif

		// qDebug() << "Recupera a tabela de parametros";
		lua_pushnil(luaL);
		while(lua_next(luaL, top - 1 ) != 0)
		{
			QString key;

			if (lua_type(luaL, -2) == LUA_TSTRING)
			{
				key = luaL_checkstring(luaL, -2);
			}
			else if (lua_type(luaL, -2) == LUA_TNUMBER)
			{
				char aux[100];
				double number = luaL_checknumber(luaL, -2);
				sprintf(aux, "%g", number);
				key = aux;
			}

			switch (lua_type(luaL, -1))
			{
				case LUA_TBOOLEAN:
				{
					bool val = lua_toboolean(luaL, -1);
					if (key == "visible")
						obsVisible = val;
					
					if (key == "compress")
						compressDatagram = val;
					break;
				}

				case LUA_TSTRING:
				{
					const char *strAux = luaL_checkstring(luaL, -1);
					cols.append(strAux);
					break;
				}
				case LUA_TTABLE:
				{
					int pos = lua_gettop(luaL);
					QString k;

					lua_pushnil(luaL);
					while(lua_next(luaL, pos) != 0)
					{
						if (lua_type(luaL, -2) == LUA_TSTRING)
						{
							obsParams.append(luaL_checkstring(luaL, -2));
						}

						switch (lua_type(luaL, -1))
						{
						case LUA_TSTRING:
							k = QString(luaL_checkstring(luaL, -1));
							break;

						case LUA_TNUMBER:
							{
								char aux[100];
								double number = luaL_checknumber(luaL, -1);
								sprintf(aux, "%g", number);
								k = QString(aux);
								break;
							}
						default:
							break;
						}
						cols.append(k);
						lua_pop(luaL, 1);
					}
					break;
				}
				default:
					break;
			}
			lua_pop(luaL, 1);
		}

		// qDebug() << "Recupera a tabela de atributos";
		lua_pushnil(luaL);
		while(lua_next(luaL, top - 2) != 0)
		{
			if (lua_type(luaL, -1) == LUA_TSTRING)
			{
				obsAttribs.push_back(luaL_checkstring(luaL, -1));
			}
			lua_pop(luaL, 1);
		}

		// Retrieves all subject attributes
		lua_pushnil(luaL);
		while(lua_next(luaL, top) != 0)
		{
			if (obsAttribs.empty())
			{
				obsAttribs = allSocietyAttribs;

				foreach(const QString &key, allSocietyAttribs)
					observedAttribs.insert(key, "");
			}
			else
			{
				// Verifica se o atributo informado realmente existe na celula
				for (int i = 0; i < obsAttribs.size(); i++)
				{
					if (! observedAttribs.contains(obsAttribs.at(i)) )
						observedAttribs.insert(obsAttribs.at(i), "");

					if (! allSocietyAttribs.contains(obsAttribs.at(i)))
					{
						string errorMsg = string("Attribute name ") + string(obsAttribs.at(i).toAscii().data()) + string(" not found.");
						lua_getglobal(L, "customError");
						lua_pushstring(L,errorMsg.c_str());
						//lua_pushnumber(L,5);
						lua_call(L,1,0);
						return 0;
					}
				}
			}

			ObserverTextScreen *obsText = 0;
			ObserverTable *obsTable = 0;
			ObserverGraphic *obsGraphic = 0;
			ObserverLogFile *obsLog = 0;
			ObserverUDPSender *obsUDPSender = 0;
			int obsId = -1;

			switch (typeObserver)
			{
				case TObsTextScreen:
					obsText = (ObserverTextScreen*)SocietySubjectInterf::createObserver(TObsTextScreen);
					if (obsText)
					{
						obsId = obsText->getId();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				case TObsLogFile:
					obsLog = (ObserverLogFile*) SocietySubjectInterf::createObserver(TObsLogFile);
					if (obsLog)
					{
						obsId = obsLog->getId();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				case TObsTable:
					obsTable = (ObserverTable *)
					SocietySubjectInterf::createObserver(TObsTable);
					if (obsTable)
					{
						obsId = obsTable->getId();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				case TObsDynamicGraphic:
					obsGraphic = (ObserverGraphic *) SocietySubjectInterf::createObserver(TObsDynamicGraphic);
					if (obsGraphic)
					{
						obsGraphic->setObserverType(TObsDynamicGraphic);
						obsId = obsGraphic->getId();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				case TObsGraphic:
					obsGraphic = (ObserverGraphic *) SocietySubjectInterf::createObserver(TObsGraphic);
					if (obsGraphic)
					{
						obsId = obsGraphic->getId();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				case TObsUDPSender:
					obsUDPSender = (ObserverUDPSender *) SocietySubjectInterf::createObserver(TObsUDPSender);
					if (obsUDPSender)
					{
						obsId = obsUDPSender->getId();
						obsUDPSender->setCompress(compressDatagram);

						if (obsVisible)
							obsUDPSender->show();
					}
					else
					{
						if (execModes != Quiet)
							qWarning("%s", qPrintable(TerraMEObserver::MEMORY_ALLOC_FAILED));
					}
					break;

				default:
					if (execModes != Quiet )
					{
						qWarning("In this context, the code '%s' does not correspond to a "
								 "valid type of Observer.",  getObserverName(typeObserver) );
					}
					return 0;
			}

#ifdef DEBUG_OBSERVER
			qDebug() << "obsParams: " << obsParams;
			qDebug() << "\nobsAttribs: " << obsAttribs;
			qDebug() << "\nallAttribs: " << allAttribs;
			qDebug() << "\ncols: " << cols;
#endif

			if (obsLog)
			{
				obsLog->setAttributes(obsAttribs);

				if (cols.at(0).isNull() || cols.at(0).isEmpty())
				{
					if (execModes != Quiet )
						qWarning("Filename was not specified, using a "
								 "default \"%s\".", qPrintable(DEFAULT_NAME));
					obsLog->setFileName(DEFAULT_NAME + ".csv");
				}
				else
				{
					obsLog->setFileName(cols.at(0));
				}

				// caso nao seja definido, utiliza o default ";"
				if ((cols.size() < 2) || cols.at(1).isNull() || cols.at(1).isEmpty())
				{
					if (execModes != Quiet )
						qWarning("Separator not defined, using \";\".");
					obsLog->setSeparator();
				}
				else
				{
					obsLog->setSeparator(cols.at(1));
				}

				lua_pushnumber(luaL, obsId);
				return 1;
			}

			if (obsText)
			{
				obsText->setAttributes(obsAttribs);
				lua_pushnumber(luaL, obsId);
				return 1;
			}

			if (obsTable)
			{
				if ((cols.size() < 2) || cols.at(0).isNull() || cols.at(0).isEmpty()
					|| cols.at(1).isNull() || cols.at(1).isEmpty())
				{
					if (execModes != Quiet )
						qWarning("Column title not defined.");
				}

				obsTable->setColumnHeaders(cols);
				obsTable->setAttributes(obsAttribs);

				lua_pushnumber(luaL, obsId);
				return 1;
			}

			if (obsGraphic)
			{
				obsGraphic->setLegendPosition();

				// Takes titles of three first locations
				obsGraphic->setTitles(cols.at(0), cols.at(1), cols.at(2));
				cols.removeFirst(); // remove graphic title
				cols.removeFirst(); // remove axis x title
				cols.removeFirst(); // remove axis y title

				// Splits the attribute labels in the cols list
				obsGraphic->setAttributes(obsAttribs, cols.takeFirst().split(";", QString::SkipEmptyParts),
										  obsParams, cols);

				lua_pushnumber(luaL, obsId);
				return 1;
			}

			if(obsUDPSender)
			{
				obsUDPSender->setAttributes(obsAttribs);

				if (cols.isEmpty())
				{
					if (execModes != Quiet )
						qWarning("Port not defined.");
				}
				else
				{
					obsUDPSender->setPort(cols.at(0).toInt());
				}

				// broadcast
				if ((cols.size() == 1) || ((cols.size() == 2) && cols.at(1).isEmpty()) )
				{
					if (execModes != Quiet )
					{
						string err_out = string("Observer will send to broadcast.");
						lua_getglobal(L, "customWarningMsg");
						lua_pushstring(L,err_out.c_str());
						//lua_pushnumber(L,5);
						lua_call(L,1,0);
					}
					obsUDPSender->addHost(BROADCAST_HOST);
				}
				else
				{
					// multicast or unicast
					for(int i = 1; i < cols.size(); i++)
					{
						if (! cols.at(i).isEmpty())
							obsUDPSender->addHost(cols.at(i));
					}
				}
				lua_pushnumber(luaL, obsId);
				return 1;
			}
		}
	//   ((typeObserver !=  TObsMap) && (typeObserver !=  TObsImage))
	// Creation of spatial observers
	}
	else
	{
		QStringList allAttribs, obsAttribs;
		QStringList obsParams, obsParamsAtribs; // parametros/atributos da legenda
		QString key;

		bool getObserverId = false, isLegend = false;
		int obsId = -1;

		cellSpace = 0;
		AgentObserverMap *obsMap = 0;
		AgentObserverImage *obsImage = 0;

		// Recupera todos os atributos do agente
		// buscando apenas a classe do agente
		lua_pushnil(luaL);
		while(lua_next(luaL, top ) != 0)
		{
			if (lua_type(luaL, -2) == LUA_TSTRING)
			{
				key = luaL_checkstring(luaL, -2);
				allAttribs.append(key);

				if (key == "class")
				{
					attrClassName.append(" (");
					attrClassName.append( luaL_checkstring(luaL, -1) );
					attrClassName.append(")");
				}
			}
			lua_pop(luaL, 1);
		}

		// Recupera os parametros
		lua_pushnil(luaL);
		while(lua_next(luaL, top - 1) != 0)
		{
			// Recupera o ID do observer map
			if ( (lua_isnumber(luaL, -1) && (! getObserverId)) )
			{
				obsId = luaL_checknumber(luaL, -1);
				getObserverId = true;
				isLegend = true;
			}

			// recupera o espao celular
			if (lua_istable(luaL, -1))
			{
				int paramTop = lua_gettop(luaL);

				lua_pushnil(luaL);
				while(lua_next(luaL, paramTop) != 0)
				{
					if (isudatatype(luaL, -1, "TeCellularSpace"))
					{
						cellSpace = Luna<luaCellularSpace>::check(L, -1);
					}
					else
					{
						if (isLegend)
						{
							key = luaL_checkstring(luaL, -2);

							obsParams.push_back(key);

							bool boolAux;
							double numAux;
							QString strAux;

							switch( lua_type(luaL, -1) )
							{
							case LUA_TBOOLEAN:
								boolAux = lua_toboolean(luaL, -1);
								break;

							case LUA_TNUMBER:
								numAux = luaL_checknumber(luaL, -1);
								obsParamsAtribs.push_back(QString::number(numAux));
								break;

							case LUA_TSTRING:
								strAux = luaL_checkstring(luaL, -1);
								obsParamsAtribs.push_back(QString(strAux));
								break;

							default:
								break;
							}
						} // isLegend
					}
					lua_pop(luaL, 1);
				}
			}
			lua_pop(luaL, 1);
		}

		QString errorMsg = QString("\nError: The Observer ID \"%1\" was not found. "
			"Check the declaration of this observer.\n").arg(obsId);

		if (! cellSpace)
		{
			lua_getglobal(L, "customError");
			lua_pushstring(L,errorMsg.toAscii().data());
			//lua_pushnumber(L,5);
			lua_call(L,1,0);
			return 0;
		}

		if (typeObserver == TObsMap)
		{
			obsMap = (AgentObserverMap *)cellSpace->getObserver(obsId);

			if (! obsMap)
			{
				lua_getglobal(L, "customError");
				lua_pushstring(L,errorMsg.toAscii().data());
				//lua_pushnumber(L,5);
				lua_call(L,1,0);
				return 0;
			}

			obsMap->registry(this);
		}
		else
		{
			obsImage = (AgentObserverImage *)cellSpace->getObserver(obsId);

			if (! obsImage)
			{
				lua_getglobal(L, "customError");
				lua_pushstring(L,errorMsg.toAscii().data());
				//lua_pushnumber(L,5);
				lua_call(L,1,0);
				return 0;
			}

			obsImage->registry(this);
		}

		// Recupera os atributos
		lua_pushnil(luaL);
		while(lua_next(luaL, top - 2) != 0)
		{
			key = luaL_checkstring(luaL, -1);

			if (key == "currentState")
				key += attrClassName;

			obsAttribs.push_back(key);
			lua_pop(luaL, 1);
		}
		
		for(int i = 0; i < obsAttribs.size(); i++)
		{
			if (! observedAttribs.contains(obsAttribs.at(i)) )
				// observedAttribs.push_back(obsAttribs.at(i));
				observedAttribs.insert(obsAttribs.at(i), "");
		}

#ifdef DEBUG_OBSERVER
		qDebug() << "\n\nluaSociety::createObserver()" << getId() << "attrClassName" << attrClassName;
		// qDebug() << "\nobsParamsLeg: " << obsParams;
		// qDebug() << "\nobsParamsAtribs: " << obsParamsAtribs;
		qDebug() << "\n-- obsAttribs: " << obsAttribs;
		qDebug() << "\n--allAttribs: " << allAttribs;

		// qDebug() << "observedAttribs.keys()" << observedAttribs.keys();
#endif

		if (typeObserver == TObsMap)
		{
			// ao definir os valores dos atributos do agente,
			// redefino o tipo do atributos na super classe ObserverMap
			obsMap->setAttributes(obsAttribs, obsParams, obsParamsAtribs, TObsSociety);
			obsMap->setSubjectAttributes(obsAttribs, getId(), attrClassName);
		}
		else
		{
			obsImage->setAttributes(obsAttribs, obsParams, obsParamsAtribs, TObsSociety);
			obsImage->setSubjectAttributes(obsAttribs, getId(), attrClassName);
		}
		lua_pushnumber(luaL, obsId);
		return 1;
	}

	return 0;
}

const TypesOfSubjects luaSociety::getType() const
{
	return subjectType;
}

/// Notifies observers about changes in the luaSociety internal state
int luaSociety::notify(lua_State *L )
{
#ifdef TME_STATISTIC
	double t = Statistic::getInstance().startTime();

	double time = luaL_checknumber(L, -1);
	SocietySubjectInterf::notifyObservers(time);

	t = Statistic::getInstance().endTime() - t;
	Statistic::getInstance().addElapsedTime("Total Response Time - cell", t);
	Statistic::getInstance().collectMemoryUsage();
#else
	double time = luaL_checknumber(L, -1);
	SocietySubjectInterf::notify(time);
#endif
	return 0;
}

QDataStream& luaSociety::getState(QDataStream& in, Subject *, int /*observerId*/, const QStringList & /* attribs */)
{
	int obsCurrentState = 0; //serverSession->getState(observerId);
	QByteArray content;

	switch(obsCurrentState)
	{
	case 0:
		content = getAll(in, (QStringList)observedAttribs.keys());
			
		// serverSession->setState(observerId, 1);
		//if (! QUIET_MODE )
		// 	qWarning(QString("Observer %1 passou ao estado %2").arg(observerId).arg(1).toAscii().constData());
		break;

	case 1:
		content = getChanges(in, (QStringList) observedAttribs.keys());
			
		// serverSession->setState(observerId, 0);
		//if (! QUIET_MODE )
		// 	qWarning(QString("Observer %1 passou ao estado %2").arg(observerId).arg(0).toAscii().constData());
		break;
	}
	// cleans the stack
	// lua_settop(L, 0);

#ifdef DEBUG_OBSERVER
	qDebug() << "\nluaSociety::getState() - byteArray" <<
		"\n\tcontent.size()" << content.size() << "\n";
#endif

	in << content;
	return in;
}

QByteArray luaSociety::getAll(QDataStream& /*in*/, const QStringList &attribs)
{
	// recupero a referencia na pilha lua
	Reference<luaSociety>::getReference(luaL);
	ObserverDatagramPkg::SubjectAttribute socSubj;
	return pop(luaL, attribs, &socSubj, 0);
}

QByteArray luaSociety::getChanges(QDataStream& in, const QStringList &attribs)
{
	return getAll(in, attribs);
}

QByteArray luaSociety::pop(lua_State *luaL, const QStringList& attribs, 
	ObserverDatagramPkg::SubjectAttribute *currSubj,
	ObserverDatagramPkg::SubjectAttribute *parentSubj)
{
#ifdef TME_STATISTIC 
	double t = Statistic::getInstance().startMicroTime();
#endif 

	bool valueChanged = false;
	char result[20];
	double num = 0.0;

	Reference<luaSociety>::getReference(luaL);
	int position = lua_gettop(luaL);
	
	QByteArray key, valueTmp;
	ObserverDatagramPkg::RawAttribute *raw = 0;

	lua_pushnil(luaL);
	while(lua_next(luaL, position) != 0)
	{
		key = luaL_checkstring(luaL, -2);

		if((attribs.contains(key)) || (key == "agents"))
		{
			switch(lua_type(luaL, -1))
			{
			case LUA_TBOOLEAN:
				valueTmp = QByteArray::number(lua_toboolean(luaL, -1));

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_number(valueTmp.toDouble());

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			case LUA_TNUMBER:
				num = luaL_checknumber(luaL, -1);
				doubleToText(num, valueTmp, 20);
					
				
				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_number(num);

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			case LUA_TSTRING:
				valueTmp = luaL_checkstring(luaL, -1);

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_text(valueTmp);

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			case LUA_TTABLE:
			{
				sprintf(result, "%p", lua_topointer(luaL, -1) );
				valueTmp = result;

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_text(LUA_ADDRESS_TABLE + static_cast<const char*>(result));

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}

				// Recupera a tabela de agentes e delega a cada um sua serializacao
				if(key == "agents")
				{
					int top = lua_gettop(luaL);

					//qDebug() << "\n --- key:" << key  
					//	<< "attribs" << attribs << "\n";

					lua_pushnil(luaL);
					while(lua_next(luaL, top) != 0)
					{
						int agentTop = lua_gettop(luaL);
						lua_pushstring(luaL, "cObj_");
						lua_gettable(luaL, agentTop);

						luaGlobalAgent* agent;
						agent = (luaGlobalAgent*)Luna<luaGlobalAgent>::check(luaL, -1);
						lua_pop(luaL, 1);

						int internalCount = currSubj->internalsubject_size();
						agent->pop(luaL, attribs, 0, currSubj);

						if (currSubj->internalsubject_size() != internalCount)
							valueChanged = true;
					
						lua_pop(luaL, 1);
					}
				}
				break;
			}
			case LUA_TUSERDATA:
			{
				sprintf(result, "%p", lua_topointer(luaL, -1) );
				valueTmp = result;

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_text(LUA_ADDRESS_USER_DATA + static_cast<const char*>(result));

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			}
			case LUA_TFUNCTION:
			{
				sprintf(result, "%p", lua_topointer(luaL, -1) );
				valueTmp = result;

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_text(LUA_ADDRESS_FUNCTION + static_cast<const char*>(result));

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			}

			default:
			{
				sprintf(result, "%p", lua_topointer(luaL, -1) );
				valueTmp = result;

				if(observedAttribs.value(key) != valueTmp)
				{
					if((parentSubj) && (! currSubj))
						currSubj = parentSubj->add_internalsubject();

					raw = currSubj->add_rawattributes();
					raw->set_key(key);
					raw->set_text(LUA_ADDRESS_OTHER + static_cast<const char*>(result));

					valueChanged = true;
					observedAttribs.insert(key, valueTmp);
				}
				break;
			}
			} // switch
		}
		lua_pop(luaL, 1);
	}
	
	if(valueChanged)
	{
		 if((parentSubj) && (! currSubj))
			currSubj = parentSubj->add_internalsubject();

		// id
		currSubj->set_id(getId());

		// subjectType
		currSubj->set_type(ObserverDatagramPkg::TObsSociety);

		// #attrs
		currSubj->set_attribsnumber(currSubj->rawattributes_size());

		// #elements
		currSubj->set_itemsnumber(currSubj->internalsubject_size());
	
#ifdef TME_STATISTIC
		if (! parentSubj)
		{
			t = Statistic::getInstance().endMicroTime() - t;
			Statistic::getInstance().addElapsedTime("pop lua", t);
 
			// std::string serialized;
			// csSubj->SerializeToString(&serialized);
			// QString serialized;
			QByteArray byteArray(currSubj->SerializeAsString().c_str(), currSubj->ByteSize());

#ifdef DEBUG_OBSERVER
			qDebug() << "\nluaSociety:pop - currSubj size:" << currSubj->internalsubject_size();
			std::cout << currSubj->DebugString();
			std::cout.flush();
#endif
			return byteArray;
		}

		t = Statistic::getInstance().endMicroTime() - t;
		Statistic::getInstance().addElapsedTime("pop lua", t);

		return QByteArray();

#else

		if (! parentSubj)
		{
			QByteArray byteArray(currSubj->SerializeAsString().c_str(), currSubj->ByteSize());

#ifdef DEBUG_OBSERVER
			qDebug() << "\n\nluaSociety::pop()" 
				<< "\n\tByteSize()" << currSubj->ByteSize() 
				<< "\n\tbyteArray.size()" << byteArray.size() 
				<< "\ncurrSubj->DebugString()\n";

			std::cout << currSubj->DebugString() << "\n";
			std::cout.flush();

			std::string parseCheck;
			if (! currSubj->SerializeToString(&parseCheck))
			{
				qDebug() << "\n\n SerializeToString FALHOU !!! \n\n";
				std::abort();
			}

			if (! currSubj->ParseFromString(parseCheck))
			{
				qDebug() << "\n\n ParseFromString FALHOU !!! \n\n";
				std::abort();
			}
			std::cout.flush();
#endif

			return byteArray;
		}
	}
	return QByteArray();

#endif
}

int luaSociety::kill(lua_State *luaL)
{
	int id = luaL_checknumber(luaL, 1);

	bool result = SocietySubjectInterf::kill(id);
	if (! result)
	{
		if (cellSpace)
		{
			Observer *obs = cellSpace->getObserverById(id);

			if (obs)
			{
				if (obs->getType() == TObsMap)
					result = ((AgentObserverMap *)obs)->unregistry(this, attrClassName);
				else
					result = ((AgentObserverImage *)obs)->unregistry(this, attrClassName);
			}
		}
	}
	lua_pushboolean(luaL, result);
	return 1;
}

/// Gets the luaSociety position of the luaSociety in the Lua stack
/// \param L is a pointer to the Lua stack
/// \param cell is a pointer to the cell within the Lua stack
void getReference( lua_State *L, luaSociety *cell )
{
	cell->getReference(L);
}

