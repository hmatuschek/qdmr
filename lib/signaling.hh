#ifndef SIGNALING_HH
#define SIGNALING_HH

#include <yaml-cpp/yaml.h>
#include <QString>
#include <QObject>


/** Encodes a selective call.
 * This can be CTCSS sub tones or DSC codes.
 * @ingroup conf */
struct SelectiveCall
{
protected:
  /** Type of the subtone. */
  enum class Type {
    None, CTCSS, DCS
  };

public:
  /** Empty constructor, no selective call defined. */
  SelectiveCall();
  /** Constructs a CTCSS sub tone for the specified frequency in Hz. */
  SelectiveCall(double ctcssFreq);
  /** Constructs a DCS code for the specified ocal code and inversion. */
  SelectiveCall(unsigned int octalDSCCode, bool inverted);

  /** Comparison operator. */
  bool operator==(const SelectiveCall &other) const;
  /** Comparison operator. */
  bool operator!=(const SelectiveCall &other) const;

  /** Returns @c false, if a selective call is set. */
  bool isInvalid() const;
  /** Returns @c true, if a selective call is set. */
  bool isValid() const;
  /** Returns @c true, if a CTCSS sub tone is set. */
  bool isCTCSS() const;
  /** Returns @c true, if a DCS code is set. */
  bool isDCS() const;

  /** If a CTCSS sub tone is set, returns the frequency in Hz (floating point). */
  double Hz() const;
  /** If a CTCSS sub tone is set, returns the frequency in mHz (integer). */
  unsigned int mHz() const;

  /** If a DCS code is set, returns the binary code. */
  unsigned int binCode() const;
  /** If a DCS code is set, returns the octal code. */
  unsigned int octalCode() const;
  /** If a DCS code is set, returns the inversion flag. */
  bool isInverted() const;

  /** Formats the selective call. */
  QString format() const;

public:
  /** Parses a CTCSS frequency. */
  static SelectiveCall parseCTCSS(const QString &text);
  /** Parses a DCS code. */
  static SelectiveCall parseDCS(const QString &text);
  /** Construct from binary DCS code. */
  static SelectiveCall fromBinaryDCS(unsigned int code, bool inverted);
  /** Returns a vector of standard selective calls. */
  static const QVector<SelectiveCall> &standard();

protected:
  /// Specifies the selective call type.
  Type type;
  union {
    /// CTCSS frequency in 0.1Hz
    uint16_t ctcss;
    struct {
      /// Binary DCS code
      uint16_t code;
      /// If @c true, the code is inverted.
      bool inverted;
    } dcs;
  };

protected:
  /** Fixed table of standard values. */
  static QVector<SelectiveCall> _standard;
};

Q_DECLARE_METATYPE(SelectiveCall)


namespace YAML
{
  /** Implements the conversion to and from YAML::Node. */
  template<>
  struct convert<SelectiveCall>
  {
    /** Serializes the selective call. */
    static Node encode(const SelectiveCall& rhs) {
      Node node;
      if (rhs.isCTCSS())
        node["ctcss"] = rhs.format().toStdString();
      else if (rhs.isDCS())
        node["dcs"] = rhs.format().toStdString();
      return node;
    }

    /** Parses the selective call. */
    static bool decode(const Node& node, SelectiveCall& rhs) {
      if (node.IsNull()) {
        rhs = SelectiveCall();
        return true;
      }

      if ((! node.IsMap()) || (1 != node.size()))
        return false;

      if (node["ctcss"])
        rhs = SelectiveCall::parseCTCSS(QString::fromStdString(node["ctcss"].as<std::string>()));

      if (node["dcs"])
        rhs = SelectiveCall::parseDCS(QString::fromStdString(node["dcs"].as<std::string>()));

      return rhs.isValid();
    }
  };
}

#endif // SIGNALING_HH
