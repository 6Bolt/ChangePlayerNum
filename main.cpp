#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>

#include <iostream>

#define MAXPLAYERASSIGN                 8
#define MAXGAMEPLAYERS                  4


void ShowHelp();
QStringList ReadFile(QString filePath);
bool WriteFile(QString filePath, QStringList fileData);



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.

    if(argc < 3)
    {
        ShowHelp();
        return 1;
    }

    bool firstArg = true;
    bool isNumber, doesExist;
    bool writeOver = false;
    bool oneFile = false;
    QString oneFileName;
    bool nameDirectory = false;
    QString directoryName;
    quint8 numberOfPlayers;
    quint8 playerAssignment[MAXPLAYERASSIGN];
    quint8 playerCount = 0;
    quint8 checkGamePlayerNumber;

    QString currentPath = QCoreApplication::applicationDirPath();
    QString newPath;
    QDir currentPathDir;
    QString fileAndPath;
    QString fileName;

    QStringList arguments;

    for(quint8 i = 1; i < argc; i++)
        arguments << argv[i];


    //Process arguments
    for(quint8 i = 0; i < arguments.count(); i++)
    {
        if(firstArg)
        {
            if(i == 0)
            {
                numberOfPlayers = arguments[i].toUInt (&isNumber);
                if(!isNumber)
                {
                    qDebug() << "1st argument is wrong. Number of Game Players needs to be a number.";
                    return 1;
                }
                else if(numberOfPlayers > MAXGAMEPLAYERS)
                {
                    qDebug() << "1st argument is wrong. Number of Game Players needs to be 1-4.";
                    return 1;
                }
                checkGamePlayerNumber = numberOfPlayers;
            }
            else
            {
                playerAssignment[i-1] = arguments[i].toUInt (&isNumber);
                if(!isNumber)
                {
                    qDebug() << "Player Assignment argument is wrong. Player Assignment needs to be a number.";
                    return 1;
                }
                else if(numberOfPlayers > MAXGAMEPLAYERS)
                {
                    qDebug() << "Player Assignment argument is wrong. Player Assignment needs to be 1-8.";
                    return 1;
                }

                playerCount++;
                if(playerCount == numberOfPlayers)
                    firstArg = false;
            }
        }
        else
        {

            if(arguments[i] == "-h" || arguments[i] == "--help")
            {
                //Display Help then Quit
                ShowHelp();
                return 1;
            }
            else if(arguments[i] == "-w" || arguments[i] == "--write")
            {
                writeOver = true;

                if(nameDirectory)
                {
                    qDebug() << "New Directory is not needed, if writting over the files. Please correct";
                    return 1;
                }

            }
            else if(arguments[i] == "-p" || arguments[i] == "--path")
            {
                if(arguments.count() > i+1)
                    newPath = QDir::fromNativeSeparators (arguments[i+1]);
                else
                {
                    qDebug() << "No path entered with the -p or --path option.";
                    return 1;
                }

                currentPathDir.setPath (newPath);
                doesExist = currentPathDir.exists ();

                if(!doesExist)
                {
                    qDebug() << "Entered path doesn't exist in file system.";
                    return 1;
                }
                else
                    currentPath = newPath;

                i++;
            }
            else if(arguments[i] == "-f" || arguments[i] == "--file")
            {
                oneFile = true;
                if(arguments.count() > i+1)
                    oneFileName = arguments[i+1];
                else
                {
                    qDebug() << "No file name entered with the -f or --file option.";
                    return 1;
                }

                if(!oneFileName.endsWith (".txt"))
                {
                    qDebug() << "Default Light Gun game file needs to be a .txt file";
                    return 1;
                }

                i++;
            }
            else if(arguments[i] == "-o" || arguments[i] == "--output")
            {
                nameDirectory = true;

                if(writeOver)
                {
                    qDebug() << "New Directory is not needed, if writting over the files. Please correct";
                    return 1;
                }

                if(arguments.count() > i+1)
                    directoryName = arguments[i+1];
                else
                {
                    qDebug() << "No directory name entered with the -o or --output option.";
                    return 1;
                }

                i++;
            }
            else
            {
                qDebug() << "Unknown data entered.   Data: " << arguments[i];
            }

        }
    }

    //firstArg should be falsed after the arguments are done
    if(firstArg)
    {
        qDebug() << "Player Assignment argument is wrong. Not Enough Player Assignments to match Number of Game Players. If Number of Game Plaers is 4, then 4 Player Assignments is needed";
        return 1;
    }


    //Make new directory, if needed
    if(!writeOver)
    {
        currentPathDir.setPath (currentPath);

        if(!nameDirectory)
        {
            for(quint8 i = 0; i < numberOfPlayers; i++)
            {
                directoryName.append ("P");
                directoryName.append (QString::number (playerAssignment[i]));
            }

        }

        bool didMkDir = currentPathDir.mkdir (directoryName);

        if(!didMkDir)
        {
            qDebug() << "Couldn't make " << directoryName << " directory in the path. Path: " << currentPath;
            return 1;
        }
    }

    //Start Reading the File or File(s)
    if(oneFile)
    {
        fileAndPath = currentPath+"/"+oneFileName;

        QStringList oldFile = ReadFile(fileAndPath);

        if(oldFile.isEmpty ())
            return 1;

        quint8 fileNumberPlayer = oldFile[1].toUInt (&isNumber);

        if(!isNumber)
        {
            qDebug() << "Read bad file. File number of players, is not a number. Data: " << oldFile[1];
            return 1;
        }

        quint8 numberPlayer;

        if(fileNumberPlayer <= checkGamePlayerNumber)
        {
            if(numberOfPlayers > fileNumberPlayer)
                numberPlayer = fileNumberPlayer;
            else
                numberPlayer = numberOfPlayers;

            for(quint8 i = 0; i < numberPlayer; i++)
            {
                quint16 length = oldFile[i+2].length ();
                quint16 indexOfP = oldFile[i+2].indexOf ('P');

                oldFile[i+2].chop(length-(indexOfP+1));
                oldFile[i+2].append (QString::number (playerAssignment[i]));
                //oldFile[i+2].append ("\n");
            }

            bool didWriteFile;

            if(writeOver)
                didWriteFile = WriteFile(fileAndPath, oldFile);
            else
            {
                QString newFilePath = currentPath+"/"+directoryName+"/"+oneFileName;
                didWriteFile = WriteFile(newFilePath, oldFile);
            }

            if(!didWriteFile)
                return 1;

        }


    }
    else
    {
        //Makes an Iterator of all the *.ini Files in the currentPath
        QDirIterator it(currentPath, {"*.txt"}, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

        //Now Looping Through All the Default LG Game Files
        while (it.hasNext())
        {
            it.next();

            fileName = it.fileName ();
            fileAndPath = it.filePath ();

            QStringList oldFile = ReadFile(fileAndPath);

            if(oldFile.isEmpty ())
                return 1;

            quint8 fileNumberPlayer = oldFile[1].toUInt (&isNumber);

            if(!isNumber)
            {
                qDebug() << "Read bad file. File number of players, is not a number. Data: " << oldFile[1];
                return 1;
            }

            quint8 numberPlayer;

            if(fileNumberPlayer <= checkGamePlayerNumber)
            {
                if(numberOfPlayers > fileNumberPlayer)
                    numberPlayer = fileNumberPlayer;
                else
                    numberPlayer = numberOfPlayers;


                for(quint8 i = 0; i < numberPlayer; i++)
                {
                    quint16 length = oldFile[i+2].length ();
                    quint16 indexOfP = oldFile[i+2].indexOf ('P');

                    oldFile[i+2].chop(length-(indexOfP+1));
                    oldFile[i+2].append (QString::number (playerAssignment[i]));
                    //oldFile[i+2].append ("\n");
                }


                bool didWriteFile;

                if(writeOver)
                    didWriteFile = WriteFile(fileAndPath, oldFile);
                else
                {
                    QString newFilePath = currentPath+"/"+directoryName+"/"+fileName;
                    didWriteFile = WriteFile(newFilePath, oldFile);
                }

                if(!didWriteFile)
                    return 1;

            }



        }  // while (it.hasNext())


    }   // else of if(oneFile)






    //a.exit();
    return 1;
    //return a.exec();
}



void ShowHelp()
{
    qDebug() << "ChangePlayerNum NumberOfGamePlayer[1-4] Player[1:4]Assignment[1-8] --options or -x";
    qDebug() << "The NumberOfGamePlayers & PlayerAssignments must come first, then the options. The options is below.";
    qDebug() << "-h or --help: Shows Help Info, which is this. Also doesn't run program.";
    qDebug() << "-w or --write: Overwrites the original file. Default is to make a new directory";
    qDebug() << "-p or --path follwed with \\path\\to\\gameFile.txt: Changes the directory where program works. Default uses directory that the program is running from.";
    qDebug() << "-f or --file followed by file name: Changes only that file. Default is to change all DefaultLG game files in directory, that equal Number of Game Players or ";
    qDebug() << "-o or --output followed by the name: New directory is called name. Default is based on number of players";
}

QStringList ReadFile(QString filePath)
{
    QStringList fileData;
    QFile readFile(filePath);

    if(!readFile.open (QIODevice::ReadOnly  | QIODevice::Text))
    {
        qDebug() << "Couldn't open read file. File: " << filePath;
        return fileData;
    }

    QTextStream in(&readFile);

    while(!in.atEnd())
    {
        fileData << in.readLine();
    }

    readFile.close();

    return fileData;
}

bool WriteFile(QString filePath, QStringList fileData)
{
    QFile writeFile(filePath);

    if(!writeFile.open (QIODevice::WriteOnly  | QIODevice::Text))
    {
        qDebug() << "Couldn't open write file. File: " << filePath;
        return false;
    }

    QTextStream out(&writeFile);

    for(quint16 i = 0; i < fileData.count (); i++)
    {
        out << fileData[i] << "\n";
    }

    writeFile.close();

    return true;
}


