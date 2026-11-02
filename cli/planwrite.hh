#ifndef PLANWRITE_HH
#define PLANWRITE_HH

class QCommandLineParser;
class QCoreApplication;

/** Offline preserving-write plan for AnyTone radios: reads the radio's memory from a file
 * instead of the radio, plans the preserving write of an edited YAML codeplug, and saves the
 * plan. Nothing touches a radio. Same planner as "write --preserve". */
int planWrite(QCommandLineParser &parser, QCoreApplication &app);

#endif // PLANWRITE_HH
