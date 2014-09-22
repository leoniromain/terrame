/************************************************************************************
TerraME - a software platform for multiple scale spatially-explicit dynamic modeling.
Copyright © 2001-2008 INPE and TerraLAB/UFOP.

This code is part of the TerraME framework.
This framework is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The framework provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements, or modifications.
In no event shall INPE and TerraLAB / UFOP be held liable to any party for direct,
indirect, special, incidental, or consequential damages arising out of the use
of this library and its documentation.

Author: Tiago Garcia de Senna Carneiro
Raian Vargas Maretto
        Antônio José da Cunha Rodrigues
Rodrigo Reis Pereira
*************************************************************************************/

#include <QtGui/QApplication>
#include <QtCore/QSystemLocale>

#define TME_STATISTIC_UNDEF

#ifdef TME_STATISTIC
    // Estatisticas de desempenho
    #include "statistic.h"
    #include <QFile>
#endif

#ifdef TME_BLACK_BOARD
	#include "blackBoard.h"
#endif

#ifdef TME_PROTOCOL_BUFFERS
	#include "protocol.pb.h"
#endif

#ifndef TME_OBSERVER_CLIENT_MODE

#include "terrameLua.h"

#ifndef TME_NO_TERRALIB
	#include "TeVersion.h"
#endif

#include "player.h"
#include "registryObjects.h"

//////////////////////////////////////////////////////////////////////////////
//// Percorre a lista de widget fechando cada um deles
//// Método responsável por evitar a mensagem abaixo na tela
//// "QObject::killTimers: timers cannot be stopped from another thread"
//void closeAllWidgets()
//{
//  int i = 0;
//  foreach (QWidget *widget, QApplication::allWidgets()){
//    widget->close();
//    i++;
//    printf("%i", i);
//  }
//}

////////////////////////////////////////////////////////////////////////////
// Percorre a lista de widget verificando se
// algum widget foi inicializado
bool existWindows()
{
    foreach (QWidget *widget, QApplication::allWidgets())
    {
        if (widget) // && widget->isVisible())
            return true;
    }
    return false;
}

void outputHandle(QtMsgType type, const char *msg)
{
    // ModelConsole &console = ModelConsole::getInstance();
    //if(! console.isVisible())
    //    console.show();

    Player &player = Player::getInstance();

    //in this function, you can write the message to any stream!
    switch (type) {
        case QtDebugMsg:
            player.appendMessage("Debug: " + QString(msg));
            break;

        case QtWarningMsg:
            player.appendMessage(QString(msg));
            break;

        case QtCriticalMsg:
            player.appendMessage("Critical: " + QString(msg));
            break;

        case QtFatalMsg:
            player.appendMessage("Fatal: " + QString(msg));
            fprintf(stderr, "Fatal: %s\n", msg);
            abort();

        default:
            fprintf(stdout, "%s\n", msg);
            break;
    }
}

// Shows the usage of TerraME
void usage()
{
	qWarning("\n");
    qWarning("Usage: TerraME [[-gui] | [-mode=normal|debug|quiet]] file1.lua file2.lua ...");
    qWarning("       or TerraME [-version]\n");
    qWarning("Options: ");
	qWarning(" -autoclose                 Automatically close the platform after simulation.");
	qWarning(" -config-tests <file_name>  Generate a file used to configure the execution of the tests.");
	qWarning(" -draw-all-higher <value>   Draw all subjects when percentage of changes was higher");
    qWarning("                            than <value>. Value must be between interval [0, 1].");
    qWarning(" -gui                       Show the player for the application (it works only ");
    qWarning("                            when an Environment and/or a Timer objects are used).");
	qWarning(" -ide                       Configure TerraME for running from IDEs in Windows systems.");
    qWarning(" -mode=normal (default)     Warnings enabled.");
	qWarning(" -mode=debug                Warnings treated as errors.");
	qWarning(" -mode=quiet                Warnings disabled.");
    qWarning(" -version                   TerraME general information.");
	qWarning(" -test                      Execute tests.");
    qWarning(" -workers <value>           Sets the number of threads used for spatial observers.");

    //fprintf(stderr, "\nYou should provide, at least, a model file as parameter.\n");
    //fprintf(stderr, "\nUsage examples: ");
    //fprintf(stderr, "\n\tTerraME [-gui] [-quiet] myModel1.lua [myModel2.lua .. myModelN.lua]\n");
    //fprintf(stderr, "\n\tTerraME -version\n");
}

// Shows the TerraMe and dependecies versions
void versions()
{
    //qWarning("\nTerraLab -- Earth System Modelling and Simulation Laboratory");
    qWarning("\nTerraME - Terra Modeling Environment");
    qWarning(" Version: %s ", TME_VERSION);     // macro in the file "terrameLua5_1.h"
    // string buffer = "TME_PATH_";

    QString tmeVersion = QString("%1").arg(TME_VERSION);
    // buffer.append(tmeVersion.replace(QString("."),QString("_")).toAscii().constData());
    // qWarning("        Location: '%s' ", getenv(buffer.c_str()));
    qWarning(" Location (TME_PATH): '%s'", getenv(TME_PATH));

    qWarning(" Compiled with: ");
    qWarning("    %s ", LUA_RELEASE);                  // macro in the file "lua.h"
    qWarning("    Qt %s ", qVersion());                // Qt version method
    qWarning("    Qwt %s ", QWT_VERSION_STR);          // macro in the file "qwt_global.h"                   

#ifndef TME_NO_TERRALIB
    qWarning("    TerraLib %s (Database version: %s) ", 
        TERRALIB_VERSION,       // macro in the file "TeVersion.h"
        TeDBVERSION.c_str());   // macro in the file "TeDefines.h" linha 221
#else
    qWarning("    TerraLib (disabled at compilation time) ");
#endif

    qWarning("\nFor more information, please visit: www.terrame.org\n");
}

/// Opens Lua environment and Lua libraries 
void openLuaEnvironment()
{
    // tentando utilizar um tipo meu em lua
    //L = lua_open();
    L = luaL_newstate();

#if defined( TME_LUA_5_0 )
    luaopen_base(L);             // opens the basic library
    luaopen_table(L);            // opens the table library
    luaopen_io(L);               // opens the I/O library
    luaopen_string(L);           // opens the string lib.
    luaopen_math(L);             // opens the math lib.
#else
    luaL_openlibs(L);  // open libraries
#endif
}

/// Records TerraME classes into Lua environment
void registerClasses()
{
    //lua_register(L, "msgbox",  lua_msgbox);

    Luna<luaCellIndex>::Register(L);

    Luna<luaCell >::Register(L);
    Luna<luaNeighborhood >::Register(L);
    Luna<luaCellularSpace >::Register(L);

    Luna<luaFlowCondition >::Register(L);
    Luna<luaJumpCondition >::Register(L);
    Luna<luaControlMode >::Register(L);
    Luna<luaLocalAgent >::Register(L);
    Luna<luaGlobalAgent >::Register(L);

    Luna<luaTimer >::Register(L);
    Luna<luaEvent >::Register(L);
    Luna<luaMessage >::Register(L);

    Luna<luaEnvironment > ::Register(L);

    Luna<luaTrajectory > ::Register(L);    

    //@RODRIGO
    Luna<LuaRandomUtil > ::Register(L);
    Luna<luaSociety > ::Register(L);
}

void createEmptyObserver(lua_State* L)
{
	luaL_loadstring(L,
			"local __cellEmpty = Cell{attrib = 1} \n"
			"local __obsEmpty = Observer{subject = __cellEmpty, type = \"chart\", attributes = {\"attrib\"}}\n"
			"__obsEmpty:kill()");
	lua_call(L, 0, 0);
}

extern ExecutionModes execModes;

int main ( int argc, char *argv[] )
{
#ifdef TME_PROTOCOL_BUFFERS
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION; 
#endif

    Q_INIT_RESOURCE(observerResource);

    // TODO
    // retrive lua version from TerraME.lua
    TME_VERSION = "2.0";
    TME_PATH = "TME_PATH";

    QApplication app(argc, argv);
    //app.setQuitOnLastWindowClosed(true);

#ifdef TME_STATISTIC
    {
		Statistic::getInstance();

        QFile statFile(app.applicationDirPath() + "/output_MemoryUsage.txt");
        if (statFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&statFile);

#ifdef TME_WIN32
            out << "Name                Pid      VM      WS    Priv Priv Pk   Faults   NonP Page\n";
#else
            out << "Mem\n";
#endif // TME_WIN32

        }
        statFile.close();
    }
#endif // TME_STATISTIC

    execModes = Normal;
    SHOW_GUI = false;
    WORKERS_NUMBER = 505;
    paused = false;
    step = false;
    bool autoClose = false;

    // Register the message handle of Observer Player
    if ((argc > 2) && (! strcmp(argv[1], "-gui")))
    {
        SHOW_GUI = true;

        qInstallMsgHandler(outputHandle);
        Player::getInstance().show();
        Player::getInstance().setEnabled(false);

        qWarning("Warning: The TerraME Player will be able to execute only when "
            "an Environment and/or a Timer object are used in the model file.");
        app.processEvents();
    }    

    // Loads the TerrME constructors for LUA
    QString tmePath(getenv(TME_PATH));

    if (tmePath.isEmpty())
    {
        qFatal("%s environment variable should exist and point to TerraME "
            "installation folder.", TME_PATH);
    }

    openLuaEnvironment();  // Opens Lua environment and libraries
    registerClasses();      // records TerraME Classes in Lua environment

#if defined ( TME_WIN32 )
    tmePath.append("\\lua\\terrame.lua");
#else
    tmePath.append("/lua/terrame.lua");
#endif

    //char buff[256];

    // runs the lua core files 
    int error = luaL_loadfile(L, tmePath.toAscii().constData()) || lua_pcall(L, 0, 0, 0);
    if (error)
    {
        fprintf(stderr, "\n%s", lua_tostring(L, -1));
        lua_pop(L, 1);  // pop error message from the stack
        lua_close( L );
        return -1;
    }

    // Execute the lua files passe
    if( argc < 2)
    {
        qWarning("\nYou should provide, at least, a model file as parameter.");
        usage();
        //qWarning("\nPlease, try again...");
        lua_close( L );
        return -1;
    }

    int argument = 1;
    while( argument < argc )
    {
        if ( argv[argument][0] == '-')
        {
            if ( ! strcmp(argv[argument],"-version") )
            {
                versions();
            }
            else if( !strcmp(argv[argument], "-ide"))
			{
				createEmptyObserver(L);
			}
            else if ( ! strcmp(argv[argument],"-mode=quiet") )
            {
				execModes = Quiet;
			}
			//else if(! strcmp(argv[argument],"-mode=strict"))
			//{
			//	execModes = Strict;
			//}
			else if(! strcmp(argv[argument],"-mode=normal"))
			{
				execModes = Normal;
			}
			else if(! strcmp(argv[argument],"-mode=debug"))
			{
				execModes = Debug;
			}
			else if ( ! strcmp(argv[argument], "-autoclose") )
			{
				autoClose = true;
			}
			else if ( ! strcmp(argv[argument], "-draw-all-higher") )
			{
				bool ok = false;
				double time = QString( argv[argument + 1] ).toDouble(&ok);
				if (ok) //  && (time >= 0 && time <= 100))
					BlackBoard::getInstance().setPercent(time * 0.01);
				else
					BlackBoard::getInstance().setPercent((double) 0.8);
			
				argument++;
			}
			else if ( ! strcmp(argv[argument], "-workers") )
			{
				bool ok = false;
				int number = QString( argv[argument + 1] ).toInt(&ok);
				if (ok)
					WORKERS_NUMBER = number;

				argument++;
			}
			else if(!strcmp(argv[argument], "-config-tests"))
			{
				lua_getglobal(L, "configureTests");
				
				argument++;
				if(!argv[argument])
				{
					lua_pushnil(L);
				}
				else
				{
					string fileName = argv[argument];
					lua_pushstring(L, fileName.c_str());
				}
				lua_call(L, 1, 0);
				return 0;
			}
			else if(!strcmp(argv[argument], "-test"))
			{
				lua_getglobal(L, "executeTests");
				
				argument++;
				if(!argv[argument])
				{
					lua_pushnil(L);
				}
				else
				{
					string fileName = argv[argument];
					lua_pushstring(L, fileName.c_str());
				}
				lua_call(L, 1, 0);
				return 0;
			}
			// argv[argument] is not "-gui"
			else if ( strcmp(argv[argument],"-gui") )
			{
				qWarning("\nInvalid arguments.");
				usage();
				return -1;
			}
			
			if (argc < 3)
			{
				usage();
				return -1;
			}
        }
        else
        {
            // creates the "TME_MODE" variable in the Lua namespace
            lua_pushnumber(L, execModes);
            lua_setglobal(L, "TME_MODE");

#ifdef TME_STATISTIC
            // double t = Statistic::getInstance().startMiliTime();
            double t0 = Statistic::getInstance().startMiliTime(), t1 = 0.0;
#endif
            // runs the lua files received as paremeters
            error =  luaL_loadfile(L, argv[argument] ) || lua_pcall(L, 0, 0, 0);

#ifdef TME_STATISTIC
            t1 = Statistic::getInstance().endMicroTime();
            // t1 = Statistic::getInstance().endMiliTime();
            qDebug() << "total simulation time - inicial: " << t0 
                << " final: " << t1 << " = " << t1 - t0 << " ms";
#endif

            if (error)
            {
                fprintf(stderr, "\n%s\n", lua_tostring(L, -1));
                lua_pop(L, 1);  // pop error message from the stack
                lua_close( L );
                return false;
            }
        }
        argument++;

    }
    //// Lua interpreter line-by-line
    //while (fgets(buff, sizeof(buff), stdin) != NULL)
    //{
    //error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||lua_pcall(L, 0, 0, 0);
    //if (error)
    //{
    //   fprintf(stderr, "%s", lua_tostring(L, -1));
    //   lua_pop(L, 1);  // pop error message from the stack
    //}
    //}

    lua_close( L );

    // Caso no exista nenhum janela entao finaliza
    // a aplicacao
    if (! existWindows())
	{
        app.exit();
        return 0;
    }

    // Percorre uma lista fechando todos os widgets
    //closeAllWidgets();

    //int ret = app.exec();
    //return ret;

#ifdef TME_STATISTIC
    Statistic::getInstance().collectMemoryUsage();
    Statistic::getInstance().saveData();
    QFile::copy(app.applicationDirPath() + "/output_MemoryUsage.txt",
        app.applicationDirPath() + "/memoryUsage_"
                        + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")
                        + "_.csv");

    // exit(0);
#endif

    if (autoClose)
    {
        // app.processEvents();
        exit(0);
        qDebug() << "\ncloseAllWindows()"; std::cout.flush();
        return 0;
    }

    return app.exec();
}
#else

extern ExecutionModes execModes;

int WORKERS_NUMBER = 0;
execModes = Normal;
bool step = false;
bool paused = false;

#include "receiverUDP.h"
#include "receiverTcpServer.h"

void receiverUsage()
{
    qWarning() << "You need to put the mode of receiver.";
    qWarning() << "Specific the mode: ";
    qWarning() << "   terrame -help                   \t Show this help and exit";
    qWarning() << "   terrame ";
    qWarning() << "   terrame -workers <value> [option] \t Show this helps";
    qWarning() << "\nOption";   
    qWarning() << "    -tcp              \t Receiver in mode TCP (default mode)";
    qWarning() << "    -udp              \t Receiver in mode UDP";
    qWarning() << "";
}

int main ( int argc, char *argv[] )
{
    Q_INIT_RESOURCE(observerResource);
    QApplication app(argc, argv);

    int ret = -1;    
    
    if (argc < 2)
    {
        qWarning() << "Running in receiver in mode TCP..";
        ReceiverTcpServer receiver;
	    receiver.show();
        return app.exec();
    }
    else
    {
        QStringList argsList = app.arguments();

        int index = argsList.indexOf("-workers");
        if ( index > 1 )
        {
            bool ok = false;
            int number = argsList.at(index++).toInt(&ok);
            if (ok)
                WORKERS_NUMBER = number;
        }

        index = argsList.indexOf("-help");
        if ( index > 1 )
        {
            receiverUsage();
            return 0;
        }

        index = argsList.indexOf("-tcp");
        if ( index > 1 )
        {
            qWarning() << "Running in receiver in mode TCP..";
            ReceiverTcpServer receiver;
            receiver.show();

            ret = app.exec();
            // return app.exec();
        }
        
        index = argsList.indexOf("-udp");
        if ( index > 1 )
        {
            qWarning() << "Running in receiver in mode UDP..";
            ReceiverUDP receiver;
            receiver.show();

            ret = app.exec();
            // return app.exec();
        }
    }
    
#ifdef TME_STATISTIC
    Statistic::getInstance().collectMemoryUsage();
    Statistic::getInstance().saveData("client_");
#endif
    return ret;
}    

#endif
