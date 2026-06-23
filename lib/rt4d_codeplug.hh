//
// Created by hannes on 23.06.26.
//

#ifndef LIBDMRCONF_RT4D_CODEPLUG_HH
#define LIBDMRCONF_RT4D_CODEPLUG_HH

#include "codeplug.hh"

/** Implements the codeplug for the Radtel RT4D.
 *
 * This is a relatively compact, sparse but fixed-size codeplug. See
 * https://dmr-tools.github.io/codeplugs for details.
 *
 * @ingroup rt4d */
class RT4DCodeplug: public Codeplug
{
protected:
  class FirstSettingsElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0400; }
  };


  class ChannelBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0xc000; }
  };


  class SecondSettingsElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x0400; }
  };

  class ZoneBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x20000; }
  };

  class ContactBankElement: public Element
  {
  public:
    /** Returns the size of the element. */
    static constexpr unsigned int size() { return 0x34000; }
  };

  class GroupListBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x5000; }
  };

  class EncryptionKeyBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x3000; }
  };

  class MessageBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x1000; }
  };

  class FMBroadcastChannelBankElement: public Element
  {
  public:
    static constexpr unsigned int size() { return 0x1000; }
  };

public:
  /** Default constructor, also pre-allocates the entire codeplug. */
  explicit RT4DCodeplug(QObject *parent = nullptr);

  bool index(Config *config, Context &ctx, const ErrorStack &err) const override;
  bool encode(Config *config, const Flags &flags, const ErrorStack &err) override;

  bool decode(Config *config, const ErrorStack &err) override;

protected:
  /** Some internal offsets. */
  struct Offset {
    /// @cond DO_NOT_DOCUMENT
    static constexpr unsigned int firstSettings()     { return 0x002000; }
    static constexpr unsigned int channels()          { return 0x004000; }
    static constexpr unsigned int secondSettings()    { return 0x01c000; }
    static constexpr unsigned int zones()             { return 0x01e000; }
    static constexpr unsigned int contacts()          { return 0x05e000; }
    static constexpr unsigned int groupLists()        { return 0x0c6000; }
    static constexpr unsigned int keys()              { return 0x0d0000; }
    static constexpr unsigned int messages()          { return 0x0d6000; }
    static constexpr unsigned int fmBroadcast()       { return 0x0f0000; }
    /// @endcond
  };
};

#endif //LIBDMRCONF_RT4D_CODEPLUG_HH
