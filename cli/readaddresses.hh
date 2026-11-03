#ifndef READADDRESSES_HH
#define READADDRESSES_HH

class QCommandLineParser;
class QCoreApplication;

/** Read-only probe for AnyTone radios: "read-addr <ranges-file> <memory-file>".
 * Reads every 16-byte block in the listed address ranges and saves them as a radio memory
 * file (one line per block: address data, in hex; the format plan-write --memory reads).
 * Ranges file: one range per line, "START END" in hex, END exclusive, 16-byte aligned;
 * lines starting with '#' are ignored. Writes nothing to the radio. */
int readAddresses(QCommandLineParser &parser, QCoreApplication &app);

#endif // READADDRESSES_HH
