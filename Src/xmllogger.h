#ifndef XMLLOGGER_H
#define	XMLLOGGER_H
#include "tinyxml2.h"
#include "ilogger.h"
#include "node_comparator.h"

#include <set>


//That's the class that flushes the data to the output XML


class XmlLogger : public ILogger {

public:
    XmlLogger(std::string loglevel):ILogger(loglevel){}

    virtual ~XmlLogger() {};

    bool getLog(const char *FileName, const std::string *LogParams);

    void saveLog();

    void writeToLogMap(const Map &Map, const std::list<Node> &path);

    void writeToLogOpenClose(std::set<Node, bool(*)(const Node&, const Node&)> open, std::set<Node, NodePosComparator> &close, int number);

    void writeToLogPath(const std::list<Node> &path);

    void writeToLogHPpath(const std::list<Node> &hppath);

    void writeToLogNotFound();

    void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time, double cellSize);

private:
    std::string LogFileName;
    tinyxml2::XMLDocument doc;
};

#endif

